#include "filters/mix_filter.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

MixFilter::MixFilter(Waveform additional, double startSec)
    : _additional(std::move(additional)), _startSec(startSec)
{
    if(!std::isfinite(startSec) || startSec < 0.0)
        throw std::invalid_argument("mix: start_sec must be finite and >= 0");
    if(_additional.getSampleRate() != 44100 ||
       _additional.getNumChannels() != 1 ||
       _additional.getBitsPerSample() != 16)
    {
        throw std::invalid_argument(
            "mix: additional waveform must be 44100 Hz mono 16-bit");
    }
}

MixFilter::~MixFilter() = default;

void MixFilter::apply(Waveform& waveform)
{
    if(_additional.getSampleCount() == 0)
        return;

    constexpr auto MIN_SAMPLE_VALUE =
        static_cast<double>(std::numeric_limits<int16_t>::min());
    constexpr auto MAX_SAMPLE_VALUE =
        static_cast<double>(std::numeric_limits<int16_t>::max());

    if(waveform.getSampleCount() == 0)
    {
        waveform.setMetaInfo(_additional.getSampleRate(),
                             _additional.getNumChannels(),
                             _additional.getBitsPerSample());
    }

    const size_t OFFSET = waveform.secondsToSamples(_startSec);
    const size_t NEEDED = OFFSET + _additional.getSampleCount();

    if(NEEDED > waveform.getSampleCount())
        waveform.resize(NEEDED);

    for(size_t i = 0; i < _additional.getSampleCount(); ++i)
    {
        const double SUM =
            static_cast<double>(waveform.getSampleAt(OFFSET + i)) +
            static_cast<double>(_additional.getSampleAt(i));
        const double CLAMPED =
            std::clamp(SUM, MIN_SAMPLE_VALUE, MAX_SAMPLE_VALUE);
        waveform.setSampleAt(OFFSET + i,
                             static_cast<int16_t>(std::round(CLAMPED)));
    }
}

double MixFilter::getStartSec() const { return _startSec; }

const Waveform& MixFilter::getAdditionalWaveform() const { return _additional; }
