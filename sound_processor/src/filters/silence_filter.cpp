#include "filters/silence_filter.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>

SilenceFilter::SilenceFilter(Unit unit, double start, double end)
    : _unit(unit), _start(start), _end(end)
{
    if(!std::isfinite(start) || start < 0.0)
        throw std::invalid_argument("silence: start must be finite and >= 0");
    if(!std::isfinite(end) || end < start)
        throw std::invalid_argument("silence: end must be finite and >= start");
}

SilenceFilter::~SilenceFilter() = default;

SilenceFilter::Unit SilenceFilter::parseUnit(const std::string& str)
{
    if(str == "sec")
        return Unit::sec;
    if(str == "ms")
        return Unit::ms;
    throw std::invalid_argument("silence: unit must be 'sec' or 'ms', got: '" +
                                str + "'");
}

void SilenceFilter::apply(Waveform& waveform)
{
    const double TO_SEC = (_unit == Unit::ms) ? 0.001 : 1.0;
    const double START_SEC = _start * TO_SEC;
    const double END_SEC = _end * TO_SEC;

    const size_t START_INDEX = waveform.secondsToSamples(START_SEC);
    const size_t END_INDEX = waveform.secondsToSamples(END_SEC);

    if(END_INDEX < START_INDEX)
        return;
    const size_t SILENCE_COUNT = END_INDEX - START_INDEX + 1;

    auto& samples = waveform.getSamples();
    const size_t OLD_SIZE = samples.size();

    if(START_INDEX >= OLD_SIZE)
    {
        samples.resize(START_INDEX + SILENCE_COUNT, 0);
        return;
    }

    samples.insert(samples.begin() + static_cast<std::ptrdiff_t>(START_INDEX),
                   SILENCE_COUNT, 0);
}

SilenceFilter::Unit SilenceFilter::getUnit() const { return _unit; }

double SilenceFilter::getStart() const { return _start; }

double SilenceFilter::getEnd() const { return _end; }
