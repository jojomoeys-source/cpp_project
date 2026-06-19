#include "pipeline/pipeline.h"

#include <stdexcept>
#include <utility>

void Pipeline::addFilter(IFilter* filter)
{
    if(!filter)
        throw std::invalid_argument("Pipeline: cannot add null filter");
    _filters.push_back(filter);
}

void Pipeline::apply(Waveform& waveform)
{
    for(IFilter* filter: _filters)
        filter->apply(waveform);
}

Pipeline::~Pipeline()
{
    for(IFilter* filter: _filters)
        delete filter;
    _filters.clear();
}

Pipeline::Pipeline(Pipeline&& other) noexcept
    : _filters(std::move(other._filters))
{
}

Pipeline& Pipeline::operator=(Pipeline&& other) noexcept
{
    if(this != &other)
    {
        for(IFilter* filter: _filters)
            delete filter;
        _filters = std::move(other._filters);
    }
    return *this;
}