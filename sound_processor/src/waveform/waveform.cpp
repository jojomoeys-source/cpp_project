#include "waveform/waveform.h"
#include <cmath>
#include <limits>
#include <stdexcept>

Waveform::Waveform(size_t sampleCount): _samples(sampleCount) {}

Waveform::Waveform(size_t sampleCount, uint32_t sampleRate,
                   uint16_t numChannels, uint16_t bitsPerSample)
    : _sampleRate(sampleRate), _numChannels(numChannels),
      _bitsPerSample(bitsPerSample), _samples(sampleCount)
{
}

int16_t Waveform::getSampleAt(size_t index) const { return _samples.at(index); }

void Waveform::setSampleAt(size_t index, int16_t value)
{
    _samples.at(index) = value;
}

double Waveform::getDurationSeconds() const
{
    if(_sampleRate == 0)
        throw std::runtime_error("Sample rate cannot be zero.");
    return static_cast<double>(_samples.size()) /
           static_cast<double>(_sampleRate);
}

size_t Waveform::secondsToSamples(double seconds) const
{
    if(seconds < 0.0)
        throw std::invalid_argument("Seconds cannot be negative.");
    const double SAMPLE_COUNT = seconds * static_cast<double>(_sampleRate);
    if(!std::isfinite(SAMPLE_COUNT) ||
       SAMPLE_COUNT > static_cast<double>(std::numeric_limits<size_t>::max()))
    {
        throw std::overflow_error("Seconds value is too large.");
    }
    return static_cast<size_t>(std::round(SAMPLE_COUNT));
}

double Waveform::samplesIndexToSeconds(size_t sampleIndex) const
{
    if(_sampleRate == 0)
        throw std::runtime_error("Sample rate cannot be zero.");
    return static_cast<double>(sampleIndex) / static_cast<double>(_sampleRate);
}

void Waveform::resize(size_t newSampleCount)
{
    _samples.resize(newSampleCount);
}

void Waveform::clear() { *this = Waveform{}; }
