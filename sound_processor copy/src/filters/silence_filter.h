#pragma once

#include "filter/filter.h"
#include "waveform/waveform.h"

#include <string>

class SilenceFilter: public IFilter
{
public:
    enum class Unit
    {
        sec,
        ms
    };

    SilenceFilter(Unit unit, double start, double end);
    ~SilenceFilter() override;

    static Unit parseUnit(const std::string& s);

    void apply(Waveform& waveform) override;

    Unit getUnit() const;
    double getStart() const;
    double getEnd() const;

private:
    Unit _unit;
    double _start;
    double _end;
};
