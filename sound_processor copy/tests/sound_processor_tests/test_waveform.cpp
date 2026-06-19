#include <catch2/catch_shim.hpp>

#include "waveform/waveform.h"

TEST_CASE("Waveform stores metadata and samples", "[waveform]")
{
    Waveform waveform(2, 44100, 2, 16);
    waveform.setSampleAt(0, 1000);
    waveform.setSampleAt(1, -2000);

    CHECK(waveform.getSampleCount() == 2);
    CHECK(waveform.getSampleRate() == 44100);
    CHECK(waveform.getNumChannels() == 2);
    CHECK(waveform.getBitsPerSample() == 16);
    CHECK(waveform.getSampleAt(0) == 1000);
    CHECK(waveform.getSampleAt(1) == -2000);
    CHECK(waveform.getDurationSeconds() == Catch::Approx(2.0 / 44100.0));
}

TEST_CASE("Waveform converts time and resizes", "[waveform]")
{
    Waveform waveform(10, 44100, 1, 16);
    waveform.resize(20);

    CHECK(waveform.secondsToSamples(0.5) == 22050);
    CHECK(waveform.samplesIndexToSeconds(44100) == Catch::Approx(1.0));
    CHECK(waveform.getSampleCount() == 20);
    CHECK(waveform.getSampleAt(15) == 0);
}
