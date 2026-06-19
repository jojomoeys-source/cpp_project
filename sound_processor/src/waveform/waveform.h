#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

class Waveform
{
public:
    Waveform() = default;

    explicit Waveform(size_t sampleCount);

    Waveform(size_t sampleCount, uint32_t sampleRate, uint16_t numChannels,
             uint16_t bitsPerSample);

    uint32_t getSampleRate() const { return _sampleRate; }
    uint16_t getNumChannels() const { return _numChannels; }
    uint16_t getBitsPerSample() const { return _bitsPerSample; }

    void setMetaInfo(uint32_t sampleRate, uint16_t numChannels,
                     uint16_t bitsPerSample)
    {
        _sampleRate = sampleRate;
        _numChannels = numChannels;
        _bitsPerSample = bitsPerSample;
    }

    size_t getSampleCount() const { return _samples.size(); }

    const std::vector<int16_t>& getSamples() const { return _samples; }

    std::vector<int16_t>& getSamples() { return _samples; }

    int16_t getSampleAt(size_t index) const;
    void setSampleAt(size_t index, int16_t value);

    double getDurationSeconds() const;

    size_t secondsToSamples(double seconds) const;

    double samplesIndexToSeconds(size_t sampleIndex) const;

    void resize(size_t newSampleCount);

    void clear();

private:
    uint32_t _sampleRate = 44100;
    uint16_t _numChannels = 1;
    uint16_t _bitsPerSample = 16;

    std::vector<int16_t> _samples;
};
