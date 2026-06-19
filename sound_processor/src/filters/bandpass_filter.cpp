#include "filters/bandpass_filter.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr double INT16_MIN_VALUE =
    static_cast<double>(std::numeric_limits<int16_t>::min());
constexpr double INT16_MAX_VALUE =
    static_cast<double>(std::numeric_limits<int16_t>::max());

void validateOddWindow(int windowSize, const char* filterName)
{
    if(windowSize < 1 || windowSize % 2 == 0)
    {
        throw std::invalid_argument(std::string(filterName) +
                                    ": window_size must be odd and >= 1");
    }
}

int16_t clampSample(double value)
{
    return static_cast<int16_t>(
        std::round(std::clamp(value, INT16_MIN_VALUE, INT16_MAX_VALUE)));
}

double movingAverage(const std::vector<int16_t>& samples, size_t index,
                     int half, int windowSize)
{
    double sum = 0.0;
    for(int k = -half; k <= half; ++k)
    {
        const int IDX_RAW = static_cast<int>(index) + k;
        const size_t IDX = static_cast<size_t>(
            std::clamp(IDX_RAW, 0, static_cast<int>(samples.size()) - 1));
        sum += static_cast<double>(samples[IDX]);
    }
    return sum / static_cast<double>(windowSize);
}

std::vector<int16_t> movingAverageSignal(const std::vector<int16_t>& samples,
                                         int windowSize)
{
    const int HALF = windowSize / 2;
    std::vector<int16_t> result(samples.size());
    for(size_t i = 0; i < samples.size(); ++i)
        result[i] = clampSample(movingAverage(samples, i, HALF, windowSize));
    return result;
}

}  // namespace

BandpassFilter::BandpassFilter(int lowWindowSize, int highWindowSize)
    : _lowWindowSize(lowWindowSize), _highWindowSize(highWindowSize)
{
    validateOddWindow(_lowWindowSize, "bandpass low_window");
    validateOddWindow(_highWindowSize, "bandpass high_window");
}

BandpassFilter::~BandpassFilter() = default;

void BandpassFilter::apply(Waveform& waveform)
{
    auto& samples = waveform.getSamples();
    if(samples.empty())
        return;

    const std::vector<int16_t> LOWPASSED =
        movingAverageSignal(samples, _lowWindowSize);
    const int HIGH_HALF = _highWindowSize / 2;
    std::vector<int16_t> result(LOWPASSED.size());

    for(size_t i = 0; i < LOWPASSED.size(); ++i)
    {
        const double LOW =
            movingAverage(LOWPASSED, i, HIGH_HALF, _highWindowSize);
        result[i] = clampSample(static_cast<double>(LOWPASSED[i]) - LOW);
    }

    samples = std::move(result);
}

int BandpassFilter::getLowWindowSize() const { return _lowWindowSize; }

int BandpassFilter::getHighWindowSize() const { return _highWindowSize; }
