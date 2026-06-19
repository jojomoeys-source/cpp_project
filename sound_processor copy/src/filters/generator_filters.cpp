#include "filters/generator_filters.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace GeneratorDetail {

constexpr double PI_CONST = 3.14159265358979323846;
constexpr double TWO_PI = 2.0 * PI_CONST;
constexpr double SAMPLE_RATE = 44100.0;
constexpr double INT16_MAX_VALUE =
    static_cast<double>(std::numeric_limits<int16_t>::max());
constexpr double INT16_MIN_VALUE =
    static_cast<double>(std::numeric_limits<int16_t>::min());

int16_t clampSample(double value)
{
    return static_cast<int16_t>(
        std::round(std::clamp(value, INT16_MIN_VALUE, INT16_MAX_VALUE)));
}

size_t msToSamples(double durationMs)
{
    if(durationMs < 0.0)
        throw std::invalid_argument("generator: duration_ms must be >= 0");
    const double SAMPLE_COUNT = durationMs * SAMPLE_RATE / 1000.0;
    if(!std::isfinite(SAMPLE_COUNT) ||
       SAMPLE_COUNT > static_cast<double>(std::numeric_limits<size_t>::max()))
    {
        throw std::overflow_error("generator: duration_ms is too large");
    }
    return static_cast<size_t>(std::round(SAMPLE_COUNT));
}

}  // namespace GeneratorDetail

AbstractGeneratorFilter::~AbstractGeneratorFilter() = default;

void AbstractGeneratorFilter::apply(Waveform& waveform)
{
    waveform.setMetaInfo(static_cast<uint32_t>(GeneratorDetail::SAMPLE_RATE), 1,
                         16);

    const size_t SAMPLE_COUNT = sampleCount();
    waveform.getSamples().resize(SAMPLE_COUNT);
    generate(waveform.getSamples());
}

SinGeneratorFilter::SinGeneratorFilter(double frequencyHz, double durationMs)
    : _frequency(frequencyHz),
      _sampleCount(GeneratorDetail::msToSamples(durationMs))
{
    if(!std::isfinite(frequencyHz) || frequencyHz < 0.0)
        throw std::invalid_argument(
            "generator sin: frequency_hz must be finite and >= 0");
}

SinGeneratorFilter::~SinGeneratorFilter() = default;

double SinGeneratorFilter::getFrequencyHz() const { return _frequency; }

size_t SinGeneratorFilter::sampleCount() const { return _sampleCount; }

void SinGeneratorFilter::generate(std::vector<int16_t>& buf) const
{
    for(size_t i = 0; i < _sampleCount; ++i)
    {
        const double TIME_SECONDS =
            static_cast<double>(i) / GeneratorDetail::SAMPLE_RATE;
        buf[i] = GeneratorDetail::clampSample(
            GeneratorDetail::INT16_MAX_VALUE *
            std::sin(GeneratorDetail::TWO_PI * _frequency * TIME_SECONDS));
    }
}

AmGeneratorFilter::AmGeneratorFilter(double amplitude, double carrierHz,
                                     double modulationHz, double depth,
                                     double durationMs)
    : _amplitude(amplitude), _carrierHz(carrierHz), _modulationHz(modulationHz),
      _depth(depth), _sampleCount(GeneratorDetail::msToSamples(durationMs))
{
    if(!std::isfinite(amplitude) || amplitude < 0.0 || amplitude > 1.0)
        throw std::invalid_argument(
            "generator am: amplitude must be finite and in [0, 1]");
    if(!std::isfinite(carrierHz) || carrierHz < 0.0)
        throw std::invalid_argument(
            "generator am: carrier_hz must be finite and >= 0");
    if(!std::isfinite(modulationHz) || modulationHz < 0.0)
        throw std::invalid_argument(
            "generator am: modulation_hz must be finite and >= 0");
    if(!std::isfinite(depth) || depth < 0.0 || depth > 1.0)
        throw std::invalid_argument(
            "generator am: depth must be finite and in [0, 1]");
}

AmGeneratorFilter::~AmGeneratorFilter() = default;

size_t AmGeneratorFilter::sampleCount() const { return _sampleCount; }

void AmGeneratorFilter::generate(std::vector<int16_t>& buf) const
{
    for(size_t i = 0; i < _sampleCount; ++i)
    {
        const double TIME_SECONDS =
            static_cast<double>(i) / GeneratorDetail::SAMPLE_RATE;
        const double ENVELOPE =
            1.0 + _depth * std::sin(GeneratorDetail::TWO_PI * _modulationHz *
                                    TIME_SECONDS);
        const double CARRIER =
            std::sin(GeneratorDetail::TWO_PI * _carrierHz * TIME_SECONDS);
        buf[i] = GeneratorDetail::clampSample(
            _amplitude * GeneratorDetail::INT16_MAX_VALUE * ENVELOPE * CARRIER);
    }
}

FmGeneratorFilter::FmGeneratorFilter(double amplitude, double carrierHz,
                                     double modulationHz, double deviationHz,
                                     double durationMs)
    : _amplitude(amplitude), _carrierHz(carrierHz), _modulationHz(modulationHz),
      _deviationHz(deviationHz),
      _sampleCount(GeneratorDetail::msToSamples(durationMs))
{
    if(!std::isfinite(amplitude) || amplitude < 0.0 || amplitude > 1.0)
        throw std::invalid_argument(
            "generator fm: amplitude must be finite and in [0, 1]");
    if(!std::isfinite(carrierHz) || carrierHz < 0.0)
        throw std::invalid_argument(
            "generator fm: carrier_hz must be finite and >= 0");
    if(!std::isfinite(modulationHz) || modulationHz <= 0.0)
        throw std::invalid_argument(
            "generator fm: modulation_hz must be finite and > 0");
    if(!std::isfinite(deviationHz) || deviationHz < 0.0)
        throw std::invalid_argument(
            "generator fm: deviation_hz must be finite and >= 0");
}

FmGeneratorFilter::~FmGeneratorFilter() = default;

size_t FmGeneratorFilter::sampleCount() const { return _sampleCount; }

void FmGeneratorFilter::generate(std::vector<int16_t>& buf) const
{
    const double MOD_RATIO = _deviationHz / _modulationHz;
    for(size_t i = 0; i < _sampleCount; ++i)
    {
        const double TIME_SECONDS =
            static_cast<double>(i) / GeneratorDetail::SAMPLE_RATE;
        const double PHASE =
            GeneratorDetail::TWO_PI * _carrierHz * TIME_SECONDS +
            MOD_RATIO * std::sin(GeneratorDetail::TWO_PI * _modulationHz *
                                 TIME_SECONDS);
        buf[i] = GeneratorDetail::clampSample(
            _amplitude * GeneratorDetail::INT16_MAX_VALUE * std::sin(PHASE));
    }
}
