#include "filters/timestretch_filter.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

TimestretchFilter::TimestretchFilter(double factor): _factor(factor)
{
    if(!std::isfinite(factor) || factor <= 0.0)
        throw std::invalid_argument(
            "timestretch: factor must be finite and > 0");
}

TimestretchFilter::~TimestretchFilter() = default;

void TimestretchFilter::apply(Waveform& waveform)
{
    const size_t OLD_SIZE = waveform.getSampleCount();
    if(OLD_SIZE == 0)
        return;

    const double NEW_SIZE_D = static_cast<double>(OLD_SIZE) * _factor;
    if(!std::isfinite(NEW_SIZE_D) ||
       NEW_SIZE_D > static_cast<double>(std::numeric_limits<size_t>::max()))
    {
        throw std::overflow_error("timestretch: result is too large");
    }
    const auto NEW_SIZE = static_cast<size_t>(std::round(NEW_SIZE_D));
    if(NEW_SIZE == 0)
    {
        waveform.getSamples().clear();
        return;
    }

    constexpr auto MIN_SAMPLE_VALUE =
        static_cast<double>(std::numeric_limits<int16_t>::min());
    constexpr auto MAX_SAMPLE_VALUE =
        static_cast<double>(std::numeric_limits<int16_t>::max());

    const auto& src = waveform.getSamples();
    std::vector<int16_t> dst(NEW_SIZE);

    for(size_t i = 0; i < NEW_SIZE; ++i)
    {
        const double POS = static_cast<double>(i) / _factor;
        const auto LEFT = static_cast<size_t>(std::floor(POS));
        const double FRAC = POS - static_cast<double>(LEFT);

        double value = 0.0;
        if(LEFT + 1 < OLD_SIZE)
        {
            value = static_cast<double>(src[LEFT]) * (1.0 - FRAC) +
                    static_cast<double>(src[LEFT + 1]) * FRAC;
        }
        else
        {
            value = static_cast<double>(src[LEFT]);
        }

        dst[i] = static_cast<int16_t>(
            std::round(std::clamp(value, MIN_SAMPLE_VALUE, MAX_SAMPLE_VALUE)));
    }

    waveform.getSamples() = std::move(dst);
}

double TimestretchFilter::getFactor() const { return _factor; }
