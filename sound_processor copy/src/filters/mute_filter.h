#pragma once

#include "filter/filter.h"
#include "waveform/waveform.h"

class MuteFilter: public IFilter
{
public:
    MuteFilter(double startSec, double endSec);
    ~MuteFilter() override;

    void apply(Waveform& waveform) override;

    double getStartSec() const;
    double getEndSec() const;

private:
    double _startSec;
    double _endSec;
};
