#include "wav/wav_io.h"
#include "wav/wav_headers.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>

namespace {

constexpr uint16_t EXPECTED_SAMPLE_RATE = 44100;
constexpr uint16_t EXPECTED_CHANNELS = 1;
constexpr uint16_t EXPECTED_BITS_PER_SAMPLE = 16;
constexpr uint16_t EXPECTED_BLOCK_ALIGN =
    EXPECTED_CHANNELS * (EXPECTED_BITS_PER_SAMPLE / 8);
constexpr uint32_t EXPECTED_BYTE_RATE =
    EXPECTED_SAMPLE_RATE * EXPECTED_BLOCK_ALIGN;

uintmax_t fileSizeOrThrow(const std::filesystem::path& filePath)
{
    std::error_code errorCode;
    const uintmax_t SIZE = std::filesystem::file_size(filePath, errorCode);
    if(errorCode)
    {
        throw std::runtime_error("Cannot determine file size: " +
                                 filePath.string());
    }
    return SIZE;
}

void requireBytesAvailable(std::istream& file, uintmax_t fileSize,
                           uintmax_t bytes,
                           const std::filesystem::path& filePath)
{
    const std::streampos CURRENT_POS = file.tellg();
    if(CURRENT_POS < std::streampos{0})
    {
        throw std::runtime_error("Cannot determine current file position: " +
                                 filePath.string());
    }

    const auto CURRENT_OFFSET = static_cast<std::streamoff>(CURRENT_POS);
    const auto BYTES_OFFSET = static_cast<std::streamoff>(bytes);
    if(BYTES_OFFSET < 0 || CURRENT_OFFSET < 0 ||
       bytes >
           static_cast<uintmax_t>(std::numeric_limits<std::streamoff>::max()))
    {
        throw std::runtime_error("Invalid WAV chunk position: " +
                                 filePath.string());
    }

    const auto END_OFFSET = CURRENT_OFFSET + BYTES_OFFSET;
    if(END_OFFSET < CURRENT_OFFSET ||
       static_cast<uintmax_t>(END_OFFSET) > fileSize)
    {
        throw std::runtime_error("Unexpected EOF while reading WAV chunk: " +
                                 filePath.string());
    }
}

void skipBytes(std::istream& file, uint32_t size, uintmax_t fileSize,
               const std::filesystem::path& filePath)
{
    requireBytesAvailable(file, fileSize, size, filePath);
    file.seekg(size, std::ios::cur);

    if(size % 2 == 1)
    {
        requireBytesAvailable(file, fileSize, 1, filePath);
        file.seekg(1, std::ios::cur);
    }
}

}  // namespace

Waveform WavReader::read(const std::string& filePath)
{
    const std::filesystem::path PATH(filePath);
    const uintmax_t TOTAL_SIZE = fileSizeOrThrow(PATH);
    if(TOTAL_SIZE < sizeof(RiffHeader))
        throw std::runtime_error("WAV file is too small: " + filePath);

    std::ifstream file(filePath, std::ios::binary);
    if(!file.is_open())
        throw std::runtime_error("Cannot open file for reading: " + filePath);

    RiffHeader riff{};
    file.read(reinterpret_cast<char*>(&riff), sizeof(riff));
    if(!file)
        throw std::runtime_error("Failed to read RIFF header: " + filePath);
    if(std::strncmp(riff.chunkId, "RIFF", 4) != 0)
        throw std::runtime_error("Not a RIFF file: " + filePath);
    if(std::strncmp(riff.format, "WAVE", 4) != 0)
        throw std::runtime_error("Not a WAVE file: " + filePath);
    if(riff.chunkSize != TOTAL_SIZE - 8)
        throw std::runtime_error("Invalid RIFF chunk size: " + filePath);

    FmtHeader fmt{};
    requireBytesAvailable(file, TOTAL_SIZE, sizeof(fmt), PATH);
    file.read(reinterpret_cast<char*>(&fmt), sizeof(fmt));
    if(!file)
        throw std::runtime_error("Failed to read fmt header: " + filePath);
    if(std::strncmp(fmt.subchunkId, "fmt ", 4) != 0)
        throw std::runtime_error("Missing fmt chunk: " + filePath);
    if(fmt.subchunkSize < EXPECTED_BITS_PER_SAMPLE)
        throw std::runtime_error("Invalid fmt chunk size: " + filePath);
    if(fmt.audioFormat != 1)
    {
        throw std::runtime_error(
            "Unsupported audio format (only PCM=1 is supported): " + filePath);
    }
    if(fmt.numChannels != EXPECTED_CHANNELS)
    {
        throw std::runtime_error(
            "Unsupported channel count (only mono is supported): " + filePath);
    }
    if(fmt.sampleRate != EXPECTED_SAMPLE_RATE)
    {
        throw std::runtime_error(
            "Unsupported sample rate (only 44100 Hz is supported): " +
            filePath);
    }
    if(fmt.blockAlign != EXPECTED_BLOCK_ALIGN)
        throw std::runtime_error("Invalid WAV block_align: " + filePath);
    if(fmt.byteRate != EXPECTED_BYTE_RATE)
        throw std::runtime_error("Invalid WAV byte_rate: " + filePath);
    if(fmt.bitsPerSample != EXPECTED_BITS_PER_SAMPLE)
    {
        throw std::runtime_error(
            "Unsupported bit depth (only 16-bit is supported): " + filePath);
    }

    if(fmt.subchunkSize > EXPECTED_BITS_PER_SAMPLE)
    {
        skipBytes(file, fmt.subchunkSize - EXPECTED_BITS_PER_SAMPLE, TOTAL_SIZE,
                  PATH);
    }

    DataHeader data{};
    while(true)
    {
        requireBytesAvailable(file, TOTAL_SIZE, sizeof(data), PATH);
        file.read(reinterpret_cast<char*>(&data), sizeof(data));
        if(!file)
            throw std::runtime_error("Failed to read data header: " + filePath);
        if(std::strncmp(data.subchunkId, "data", 4) == 0)
            break;
        skipBytes(file, data.subchunkSize, TOTAL_SIZE, PATH);
    }

    const std::streampos DATA_PAYLOAD_POS = file.tellg();
    if(DATA_PAYLOAD_POS < std::streampos{0})
    {
        throw std::runtime_error("Cannot determine data chunk position: " +
                                 filePath);
    }
    const uintmax_t REMAINING_AFTER_HEADER =
        TOTAL_SIZE - static_cast<uintmax_t>(DATA_PAYLOAD_POS);
    if(data.subchunkSize > REMAINING_AFTER_HEADER)
    {
        throw std::runtime_error("Data chunk size exceeds file size: " +
                                 filePath);
    }

    const size_t BYTES_PER_SAMPLE = fmt.bitsPerSample / 8;
    if(data.subchunkSize % BYTES_PER_SAMPLE != 0)
    {
        throw std::runtime_error(
            "Data chunk size is not aligned to sample size: " + filePath);
    }

    const size_t SAMPLE_COUNT = data.subchunkSize / BYTES_PER_SAMPLE;
    Waveform waveform(SAMPLE_COUNT, fmt.sampleRate, fmt.numChannels,
                      fmt.bitsPerSample);

    if(data.subchunkSize > 0)
    {
        file.read(reinterpret_cast<char*>(waveform.getSamples().data()),
                  static_cast<std::streamsize>(data.subchunkSize));
        if(!file)
        {
            throw std::runtime_error("Failed to read audio samples: " +
                                     filePath);
        }
    }

    return waveform;
}

void WavWriter::write(const std::string& filePath, const Waveform& waveform)
{
    std::ofstream file(filePath, std::ios::binary);
    if(!file.is_open())
        throw std::runtime_error("Cannot open file for writing: " + filePath);

    const uint64_t SAMPLE_COUNT = waveform.getSampleCount();
    const uint16_t NUM_CHANNELS = waveform.getNumChannels();
    const uint32_t SAMPLE_RATE = waveform.getSampleRate();
    const uint16_t BITS_PER_SAMPLE = waveform.getBitsPerSample();

    if(NUM_CHANNELS != EXPECTED_CHANNELS)
    {
        throw std::invalid_argument(
            "WavWriter: only mono WAV files are supported");
    }
    if(SAMPLE_RATE != EXPECTED_SAMPLE_RATE)
    {
        throw std::invalid_argument(
            "WavWriter: only 44100 Hz WAV files are supported");
    }
    if(BITS_PER_SAMPLE != EXPECTED_BITS_PER_SAMPLE)
    {
        throw std::invalid_argument(
            "WavWriter: only 16-bit WAV files are supported");
    }

    const auto BLOCK_ALIGN =
        static_cast<uint32_t>(NUM_CHANNELS * (BITS_PER_SAMPLE / 8));
    const uint32_t BYTE_RATE = SAMPLE_RATE * BLOCK_ALIGN;
    const uint64_t DATA_SIZE =
        SAMPLE_COUNT * static_cast<uint64_t>(BLOCK_ALIGN);
    const uint64_t RIFF_SIZE = 36 + DATA_SIZE;

    if(DATA_SIZE > std::numeric_limits<uint32_t>::max() ||
       RIFF_SIZE > std::numeric_limits<uint32_t>::max())
    {
        throw std::invalid_argument("WavWriter: WAV file is too large");
    }

    RiffHeader riff{};
    std::memcpy(riff.chunkId, "RIFF", 4);
    riff.chunkSize = static_cast<uint32_t>(RIFF_SIZE);
    std::memcpy(riff.format, "WAVE", 4);
    file.write(reinterpret_cast<const char*>(&riff), sizeof(riff));

    FmtHeader fmt{};
    std::memcpy(fmt.subchunkId, "fmt ", 4);
    fmt.subchunkSize = 16;
    fmt.audioFormat = 1;
    fmt.numChannels = NUM_CHANNELS;
    fmt.sampleRate = SAMPLE_RATE;
    fmt.byteRate = BYTE_RATE;
    fmt.blockAlign = BLOCK_ALIGN;
    fmt.bitsPerSample = BITS_PER_SAMPLE;
    file.write(reinterpret_cast<const char*>(&fmt), sizeof(fmt));

    DataHeader data{};
    std::memcpy(data.subchunkId, "data", 4);
    data.subchunkSize = static_cast<uint32_t>(DATA_SIZE);
    file.write(reinterpret_cast<const char*>(&data), sizeof(data));

    if(DATA_SIZE > 0)
    {
        file.write(reinterpret_cast<const char*>(waveform.getSamples().data()),
                   static_cast<std::streamsize>(DATA_SIZE));
    }

    if(!file)
        throw std::runtime_error("Failed to write WAV file: " + filePath);
}
