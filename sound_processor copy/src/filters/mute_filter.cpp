#include "filters/mute_filter.h"

#include <cmath>
#include <stdexcept>

MuteFilter::MuteFilter(double startSec, double endSec)
    : _startSec(startSec), _endSec(endSec)
{
    if(!std::isfinite(startSec) || startSec < 0.0)
        throw std::invalid_argument("mute: start_sec must be finite and >= 0");
    if(!std::isfinite(endSec) || endSec < startSec)
        throw std::invalid_argument(
            "mute: end_sec must be finite and >= start_sec");
}

MuteFilter::~MuteFilter() = default;

void MuteFilter::apply(Waveform& waveform)
{
    if(waveform.getSampleCount() == 0)
        return;

    const size_t START = waveform.secondsToSamples(_startSec);
    const size_t END = waveform.secondsToSamples(_endSec);
    const size_t COUNT = waveform.getSampleCount();

    for(size_t i = START; i < END && i < COUNT; ++i)
        waveform.setSampleAt(i, 0);
}

double MuteFilter::getStartSec() const { return _startSec; }

double MuteFilter::getEndSec() const { return _endSec; }
