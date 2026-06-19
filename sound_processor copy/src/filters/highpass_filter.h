#pragma once

#include "filter/filter.h"
#include "waveform/waveform.h"

class HighpassFilter: public IFilter
{
public:
    explicit HighpassFilter(int windowSize);
    ~HighpassFilter() override;

    void apply(Waveform& waveform) override;

    int getWindowSize() const;

private:
    int _windowSize;
};
