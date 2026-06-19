#include <catch2/catch_shim.hpp>

#include "converter/converter.h"
#include "filters/ampl_filter.h"
#include "waveform/waveform.h"

#include <cstdint>
#include <vector>

static Waveform makeWaveform(std::vector<int16_t> samples)
{
    Waveform waveform(samples.size(), 44100, 1, 16);
    for(size_t i = 0; i < samples.size(); ++i)
        waveform.setSampleAt(i, samples[i]);
    return waveform;
}

TEST_CASE("Converter creates and applies pipeline", "[converter]")
{
    CmdLineArgs2PipelineConverter converter;
    converter.addFilterProducer(
        "ampl",
        [](const FilterDescriptor& filterDescriptor) -> IFilter*
        {
            (void)filterDescriptor;
            return new AmplFilter(2.0);
        });

    Pipeline pipeline =
        converter.createPipeline({FilterDescriptor{"ampl", {"2.0"}}});
    REQUIRE(pipeline.getFilterCount() == 1);

    Waveform waveform = makeWaveform({10, -20});
    pipeline.apply(waveform);

    CHECK(waveform.getSampleAt(0) == 20);
    CHECK(waveform.getSampleAt(1) == -40);
}
