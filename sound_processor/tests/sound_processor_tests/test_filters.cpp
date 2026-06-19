#include <catch2/catch_shim.hpp>

#include "filter_producers/filter_producers.h"
#include "filters/ampl_filter.h"
#include "filters/generator_filters.h"
#include "filters/highpass_filter.h"
#include "filters/lowpass_filter.h"
#include "filters/mix_filter.h"
#include "filters/mute_filter.h"
#include "filters/normalize_filter.h"
#include "filters/silence_filter.h"
#include "filters/timestretch_filter.h"
#include "waveform/waveform.h"

#include <memory>
#include <utility>
#include <vector>

static Waveform makeWaveform(std::vector<int16_t> samples,
                             uint32_t rate = 44100)
{
    Waveform waveform(samples.size(), rate, 1, 16);
    for(size_t i = 0; i < samples.size(); ++i)
        waveform.setSampleAt(i, samples[i]);
    return waveform;
}

TEST_CASE("AmplFilter scales and clamps samples", "[filters][ampl]")
{
    Waveform waveform = makeWaveform({1000, -1000, 32767, -32768});
    AmplFilter filter(0.5);
    filter.apply(waveform);

    CHECK(waveform.getSampleAt(0) == 500);
    CHECK(waveform.getSampleAt(1) == -500);
    CHECK(waveform.getSampleAt(2) == 16384);
    CHECK(waveform.getSampleAt(3) == -16384);
}

TEST_CASE("NormalizeFilter scales samples by peak", "[filters][normalize]")
{
    Waveform waveform = makeWaveform({0, 16383, -16383});
    NormalizeFilter filter;
    filter.apply(waveform);

    CHECK(waveform.getSampleAt(0) == 0);
    CHECK(waveform.getSampleAt(1) == 32767);
    CHECK(waveform.getSampleAt(2) == -32767);
}

TEST_CASE("MuteFilter zeroes selected range", "[filters][mute]")
{
    Waveform waveform(10, 10, 1, 16);
    for(size_t i = 0; i < waveform.getSampleCount(); ++i)
        waveform.setSampleAt(i, 100);

    MuteFilter filter(0.2, 0.5);
    filter.apply(waveform);

    CHECK(waveform.getSampleAt(0) == 100);
    CHECK(waveform.getSampleAt(1) == 100);
    CHECK(waveform.getSampleAt(2) == 0);
    CHECK(waveform.getSampleAt(3) == 0);
    CHECK(waveform.getSampleAt(4) == 0);
    CHECK(waveform.getSampleAt(5) == 100);
}

TEST_CASE("SilenceFilter inserts silence", "[filters][silence]")
{
    Waveform waveform = makeWaveform({100, 200, 300}, 10);

    SilenceFilter filter(SilenceFilter::Unit::sec, 0.0, 0.1);
    filter.apply(waveform);

    REQUIRE(waveform.getSampleCount() == 5);
    CHECK(waveform.getSampleAt(0) == 0);
    CHECK(waveform.getSampleAt(1) == 0);
    CHECK(waveform.getSampleAt(2) == 100);
    CHECK(waveform.getSampleAt(3) == 200);
    CHECK(waveform.getSampleAt(4) == 300);
}

TEST_CASE("LowpassFilter smooths spike", "[filters][lowpass]")
{
    Waveform waveform = makeWaveform({0, 0, 3000, 0, 0});

    LowpassFilter filter(3);
    filter.apply(waveform);

    CHECK(waveform.getSampleAt(1) == 1000);
    CHECK(waveform.getSampleAt(2) == 1000);
    CHECK(waveform.getSampleAt(3) == 1000);
}

TEST_CASE("HighpassFilter removes constant signal", "[filters][highpass]")
{
    Waveform waveform = makeWaveform({1000, 1000, 1000, 1000});

    HighpassFilter filter(3);
    filter.apply(waveform);

    for(size_t i = 0; i < waveform.getSampleCount(); ++i)
        CHECK(waveform.getSampleAt(i) == 0);
}

TEST_CASE("TimestretchFilter stretches signal", "[filters][timestretch]")
{
    Waveform waveform = makeWaveform({0, 1000});

    TimestretchFilter filter(2.0);
    filter.apply(waveform);

    REQUIRE(waveform.getSampleCount() == 4);
    CHECK(waveform.getSampleAt(0) == 0);
    CHECK(waveform.getSampleAt(1) == 500);
    CHECK(waveform.getSampleAt(2) == 1000);
    CHECK(waveform.getSampleAt(3) == 1000);
}

TEST_CASE("MixFilter mixes overlay", "[filters][mix]")
{
    Waveform base = makeWaveform({1000, 1000, 1000});
    Waveform overlay = makeWaveform({500});

    MixFilter filter(std::move(overlay), 0.0);
    filter.apply(base);

    REQUIRE(base.getSampleCount() == 3);
    CHECK(base.getSampleAt(0) == 1500);
    CHECK(base.getSampleAt(1) == 1000);
    CHECK(base.getSampleAt(2) == 1000);
}

TEST_CASE("SinGeneratorFilter replaces waveform", "[filters][generator]")
{
    Waveform waveform = makeWaveform({1, 2, 3});

    SinGeneratorFilter filter(440.0, 100.0);
    filter.apply(waveform);

    REQUIRE(waveform.getSampleCount() == 4410);
    CHECK(waveform.getSampleRate() == 44100);
    CHECK(waveform.getNumChannels() == 1);
    CHECK(waveform.getBitsPerSample() == 16);
    CHECK(waveform.getSampleAt(0) == 0);
}

TEST_CASE("FilterProducers create filters from descriptors",
          "[filters][producers]")
{
    std::unique_ptr<IFilter> ampl(
        filter_producers::makeAmpl(FilterDescriptor{"ampl", {"2"}}));
    Waveform amplWaveform = makeWaveform({10});
    ampl->apply(amplWaveform);
    CHECK(amplWaveform.getSampleAt(0) == 20);

    std::unique_ptr<IFilter> generator(filter_producers::makeGenerator(
        FilterDescriptor{"generator", {"sin", "440", "100"}}));
    Waveform generated;
    generator->apply(generated);
    CHECK(generated.getSampleCount() == 4410);
}
