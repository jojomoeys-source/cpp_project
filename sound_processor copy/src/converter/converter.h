#pragma once

#include "args/args_parser.h"
#include "filter/filter.h"
#include "pipeline/pipeline.h"

#include <map>
#include <string>
#include <vector>

using FilterProducer = IFilter* (*)(const FilterDescriptor&);

class CmdLineArgs2PipelineConverter
{
public:
    CmdLineArgs2PipelineConverter() = default;

    void addFilterProducer(const std::string& filterName,
                           FilterProducer producer);

    Pipeline
    createPipeline(const std::vector<FilterDescriptor>& descriptors) const;

private:
    FilterProducer findProducer(const std::string& filterName) const;

    std::map<std::string, FilterProducer> _producers;
};