#pragma once

#include "filter/filter.h"
#include "waveform/waveform.h"

class NormalizeFilter: public IFilter
{
public:
    static constexpr double DEFAULT_PEAK = 1.0;

    explicit NormalizeFilter(double peak = DEFAULT_PEAK);
    ~NormalizeFilter() override;

    void apply(Waveform& waveform) override;

    double getPeak() const;

private:
    double _peak;
};
