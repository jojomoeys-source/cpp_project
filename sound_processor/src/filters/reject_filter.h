#pragma once

#include "filter/filter.h"
#include "waveform/waveform.h"

class RejectFilter: public IFilter
{
public:
    RejectFilter(int lowWindowSize, int highWindowSize);
    ~RejectFilter() override;

    void apply(Waveform& waveform) override;

    int getLowWindowSize() const;
    int getHighWindowSize() const;

private:
    int _lowWindowSize;
    int _highWindowSize;
};
