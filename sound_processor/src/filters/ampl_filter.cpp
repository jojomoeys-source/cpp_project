#include "filters/ampl_filter.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

AmplFilter::AmplFilter(double factor): _factor(factor)
{
    if(!std::isfinite(factor) || factor < 0.0)
        throw std::invalid_argument("ampl: factor must be finite and >= 0");
}

AmplFilter::~AmplFilter() = default;

void AmplFilter::apply(Waveform& waveform)
{
    constexpr auto MIN_SAMPLE_VALUE =
        static_cast<double>(std::numeric_limits<int16_t>::min());
    constexpr auto MAX_SAMPLE_VALUE =
        static_cast<double>(std::numeric_limits<int16_t>::max());

    for(size_t i = 0; i < waveform.getSampleCount(); ++i)
    {
        const double SCALED =
            static_cast<double>(waveform.getSampleAt(i)) * _factor;
        const double CLAMPED =
            std::clamp(SCALED, MIN_SAMPLE_VALUE, MAX_SAMPLE_VALUE);
        waveform.setSampleAt(i, static_cast<int16_t>(std::round(CLAMPED)));
    }
}

double AmplFilter::getFactor() const { return _factor; }
