#include <catch2/catch_shim.hpp>

#include "app/application.h"
#include "wav/wav_io.h"
#include "waveform/waveform.h"

#include <filesystem>
#include <string>
#include <vector>

namespace {

void removeIfExists(const std::filesystem::path& path)
{
    std::error_code errorCode;
    std::filesystem::remove(path, errorCode);
}

int runApplication(const std::vector<std::string>& args)
{
    std::vector<std::string> storage = args;
    std::vector<char*> argv;
    argv.reserve(storage.size());
    for(auto& arg: storage)
        argv.push_back(arg.data());

    Application app;
    app.configure();
    return app.start(static_cast<int>(argv.size()), argv.data());
}

}  // namespace

TEST_CASE("Application returns success without arguments", "[application]")
{
    CHECK(runApplication({"sound_processor"}) == 0);
}

TEST_CASE("Application returns error for bad arguments", "[application]")
{
    CHECK(runApplication({"sound_processor", "-f", "-i"}) == 1);
}

TEST_CASE("Application generates WAV without input file", "[application]")
{
    const std::filesystem::path OUTPUT =
        std::filesystem::temp_directory_path() /
        "sound_processor_app_generator.wav";
    removeIfExists(OUTPUT);

    CHECK(runApplication({"sound_processor", "-o", OUTPUT.string(), "-f",
                          "generator", "sin", "440", "100"}) == 0);

    const Waveform WAVEFORM = WavReader::read(OUTPUT.string());
    CHECK(WAVEFORM.getSampleCount() == 4410);
    CHECK(WAVEFORM.getSampleRate() == 44100);
    CHECK(WAVEFORM.getNumChannels() == 1);
    CHECK(WAVEFORM.getBitsPerSample() == 16);

    removeIfExists(OUTPUT);
}

TEST_CASE("Application copies input to output without filters", "[application]")
{
    const std::filesystem::path INPUT = std::filesystem::temp_directory_path() /
                                        "sound_processor_app_input.wav";
    const std::filesystem::path OUTPUT =
        std::filesystem::temp_directory_path() /
        "sound_processor_app_output.wav";
    removeIfExists(INPUT);
    removeIfExists(OUTPUT);

    Waveform source(3, 44100, 1, 16);
    source.setSampleAt(0, 1000);
    source.setSampleAt(1, -2000);
    source.setSampleAt(2, 3000);
    WavWriter::write(INPUT.string(), source);

    CHECK(runApplication({"sound_processor", "-i", INPUT.string(), "-o",
                          OUTPUT.string()}) == 0);

    const Waveform COPIED = WavReader::read(OUTPUT.string());
    REQUIRE(COPIED.getSampleCount() == 3);
    CHECK(COPIED.getSampleAt(0) == 1000);
    CHECK(COPIED.getSampleAt(1) == -2000);
    CHECK(COPIED.getSampleAt(2) == 3000);

    removeIfExists(INPUT);
    removeIfExists(OUTPUT);
}