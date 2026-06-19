#include "converter/converter.h"

#include <stdexcept>

void CmdLineArgs2PipelineConverter::addFilterProducer(
    const std::string& filterName, FilterProducer producer)
{
    if(!producer)
    {
        throw std::invalid_argument("Filter producer for '" + filterName +
                                    "' is null");
    }
    _producers[filterName] = producer;
}

FilterProducer
CmdLineArgs2PipelineConverter::findProducer(const std::string& filterName) const
{
    const auto IT_NAME = _producers.find(filterName);
    if(IT_NAME == _producers.end())
        return nullptr;
    return IT_NAME->second;
}

Pipeline CmdLineArgs2PipelineConverter::createPipeline(
    const std::vector<FilterDescriptor>& descriptors) const
{
    Pipeline pipeline;

    for(const auto& filterDescriptor: descriptors)
    {
        FilterProducer producer = findProducer(filterDescriptor.name);

        if(!producer)
        {
            throw std::runtime_error("No filter producer registered for: '" +
                                     filterDescriptor.name + "'");
        }

        IFilter* filter = producer(filterDescriptor);

        if(!filter)
        {
            throw std::runtime_error("Filter producer returned null for: '" +
                                     filterDescriptor.name + "'");
        }

        try
        {
            pipeline.addFilter(filter);
        }
        catch(...)
        {
            delete filter;
            throw;
        }
    }

    return pipeline;
}