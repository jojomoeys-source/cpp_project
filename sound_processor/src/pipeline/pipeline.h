#pragma once
#include "filter/filter.h"
#include "waveform/waveform.h"
#include <vector>

class Pipeline
{
public:
    Pipeline() = default;

    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    Pipeline(Pipeline&& other) noexcept;
    Pipeline& operator=(Pipeline&& other) noexcept;

    void addFilter(IFilter* filter);

    size_t getFilterCount() const { return _filters.size(); }

    IFilter* operator[](size_t index) const { return _filters.at(index); }

    void apply(Waveform& waveform);

private:
    std::vector<IFilter*> _filters;
};