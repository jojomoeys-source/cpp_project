#include "filters/normalize_filter.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>

NormalizeFilter::NormalizeFilter(double peak): _peak(peak)
{
    if(!std::isfinite(peak) || peak < 0.0 || peak > 1.0)
        throw std::invalid_argument(
            "normalize: peak must be finite and in [0, 1]");
}

NormalizeFilter::~NormalizeFilter() = default;

void NormalizeFilter::apply(Waveform& waveform)
{
    const size_t COUNT = waveform.getSampleCount();
    if(COUNT == 0)
        return;

    int32_t currentPeak = 0;
    for(size_t i = 0; i < COUNT; ++i)
    {
        const int32_t ABS_SAMPLE =
            std::abs(static_cast<int32_t>(waveform.getSampleAt(i)));
        if(ABS_SAMPLE > currentPeak)
            currentPeak = ABS_SAMPLE;
    }

    if(currentPeak == 0)
        return;

    constexpr auto MIN_SAMPLE_VALUE =
        static_cast<double>(std::numeric_limits<int16_t>::min());
    constexpr auto MAX_SAMPLE_VALUE =
        static_cast<double>(std::numeric_limits<int16_t>::max());
    const double SCALE = _peak * 32767.0 / static_cast<double>(currentPeak);

    for(size_t i = 0; i < COUNT; ++i)
    {
        const double SCALED =
            static_cast<double>(waveform.getSampleAt(i)) * SCALE;
        const double CLAMPED =
            std::clamp(SCALED, MIN_SAMPLE_VALUE, MAX_SAMPLE_VALUE);
        waveform.setSampleAt(i, static_cast<int16_t>(std::round(CLAMPED)));
    }
}

double NormalizeFilter::getPeak() const { return _peak; }
