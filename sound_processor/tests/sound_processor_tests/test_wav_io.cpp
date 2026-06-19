#include <catch2/catch_shim.hpp>

#include "wav/wav_io.h"

#include <cstdint>
#include <filesystem>
#include <vector>

namespace {

void removeIfExists(const std::filesystem::path& path)
{
    std::error_code errorCode;
    std::filesystem::remove(path, errorCode);
}

Waveform makeWaveform(std::vector<int16_t> samples)
{
    Waveform waveform(samples.size(), 44100, 1, 16);
    for(size_t i = 0; i < samples.size(); ++i)
        waveform.setSampleAt(i, samples[i]);
    return waveform;
}

}  // namespace

TEST_CASE("WavWriter and WavReader round-trip PCM WAV", "[wav_io]")
{
    const std::filesystem::path PATH = std::filesystem::temp_directory_path() /
                                       "sound_processor_roundtrip.wav";
    removeIfExists(PATH);

    WavWriter::write(PATH.string(), makeWaveform({1000, -2000, 3000}));

    const Waveform READ_BACK = WavReader::read(PATH.string());
    REQUIRE(READ_BACK.getSampleCount() == 3);
    CHECK(READ_BACK.getSampleRate() == 44100);
    CHECK(READ_BACK.getNumChannels() == 1);
    CHECK(READ_BACK.getBitsPerSample() == 16);
    CHECK(READ_BACK.getSampleAt(0) == 1000);
    CHECK(READ_BACK.getSampleAt(1) == -2000);
    CHECK(READ_BACK.getSampleAt(2) == 3000);

    removeIfExists(PATH);
}

TEST_CASE("WavWriter and WavReader round-trip empty WAV", "[wav_io]")
{
    const std::filesystem::path PATH =
        std::filesystem::temp_directory_path() / "sound_processor_empty.wav";
    removeIfExists(PATH);

    WavWriter::write(PATH.string(), Waveform{});

    const Waveform READ_BACK = WavReader::read(PATH.string());
    CHECK(READ_BACK.getSampleCount() == 0);
    CHECK(READ_BACK.getSampleRate() == 44100);

    removeIfExists(PATH);
}
