#pragma once

#include "filter/filter.h"
#include "waveform/waveform.h"

class LowpassFilter: public IFilter
{
public:
    explicit LowpassFilter(int windowSize);
    ~LowpassFilter() override;

    void apply(Waveform& waveform) override;

    int getWindowSize() const;

private:
    int _windowSize;
};
