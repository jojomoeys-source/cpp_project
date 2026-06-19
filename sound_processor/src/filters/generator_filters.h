#pragma once

#include "filter/filter.h"
#include "waveform/waveform.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class AbstractGeneratorFilter: public IFilter
{
public:
    ~AbstractGeneratorFilter() override;

    void apply(Waveform& waveform) override;

protected:
    // Renamed from sample_count to camelCase
    virtual size_t sampleCount() const = 0;
    virtual void generate(std::vector<int16_t>& buf) const = 0;
};

class SinGeneratorFilter: public AbstractGeneratorFilter
{
public:
    SinGeneratorFilter(double frequency_hz, double duration_ms);
    ~SinGeneratorFilter() override;

    double getFrequencyHz() const;

protected:
    size_t sampleCount() const override;
    void generate(std::vector<int16_t>& buf) const override;

private:
    double _frequency;
    size_t _sampleCount;
};

class AmGeneratorFilter: public AbstractGeneratorFilter
{
public:
    AmGeneratorFilter(double amplitude, double carrier_hz, double modulation_hz,
                      double depth, double duration_ms);
    ~AmGeneratorFilter() override;

protected:
    size_t sampleCount() const override;
    void generate(std::vector<int16_t>& buf) const override;

private:
    double _amplitude;
    double _carrierHz;
    double _modulationHz;
    double _depth;
    size_t _sampleCount;
};

class FmGeneratorFilter: public AbstractGeneratorFilter
{
public:
    FmGeneratorFilter(double amplitude, double carrier_hz, double modulation_hz,
                      double deviation_hz, double duration_ms);
    ~FmGeneratorFilter() override;

protected:
    size_t sampleCount() const override;
    void generate(std::vector<int16_t>& buf) const override;

private:
    double _amplitude;
    double _carrierHz;
    double _modulationHz;
    double _deviationHz;
    size_t _sampleCount;
};
