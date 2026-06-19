#pragma once

#include "filter/filter.h"
#include "waveform/waveform.h"

class BandpassFilter: public IFilter
{
public:
    BandpassFilter(int lowWindowSize, int highWindowSize);
    ~BandpassFilter() override;

    void apply(Waveform& waveform) override;

    int getLowWindowSize() const;
    int getHighWindowSize() const;

private:
    int _lowWindowSize;
    int _highWindowSize;
};