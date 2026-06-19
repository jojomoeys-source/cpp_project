#pragma once

#include "filter/filter.h"
#include "waveform/waveform.h"

class MixFilter: public IFilter
{
public:
    MixFilter(Waveform additional, double startSec);
    ~MixFilter() override;

    void apply(Waveform& waveform) override;

    double getStartSec() const;
    const Waveform& getAdditionalWaveform() const;

private:
    Waveform _additional;
    double _startSec;
};
