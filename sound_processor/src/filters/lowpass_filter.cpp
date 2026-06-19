#include "filters/lowpass_filter.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

LowpassFilter::LowpassFilter(int windowSize): _windowSize(windowSize)
{
    if(windowSize < 1 || windowSize % 2 == 0)
        throw std::invalid_argument(
            "lowpass: window_size must be odd and >= 1");
}

LowpassFilter::~LowpassFilter() = default;

void LowpassFilter::apply(Waveform& waveform)
{
    const size_t COUNT = waveform.getSampleCount();
    if(COUNT == 0)
        return;

    const int HALF = _windowSize / 2;
    const auto& src = waveform.getSamples();
    std::vector<int16_t> dst(COUNT);

    for(size_t i = 0; i < COUNT; ++i)
    {
        double sum = 0.0;
        for(int k = -HALF; k <= HALF; ++k)
        {
            const int IDX_RAW = static_cast<int>(i) + k;
            const size_t IDX = static_cast<size_t>(
                std::clamp(IDX_RAW, 0, static_cast<int>(COUNT) - 1));
            sum += static_cast<double>(src[IDX]);
        }
        dst[i] = static_cast<int16_t>(std::round(sum / _windowSize));
    }

    waveform.getSamples() = std::move(dst);
}

int LowpassFilter::getWindowSize() const { return _windowSize; }
