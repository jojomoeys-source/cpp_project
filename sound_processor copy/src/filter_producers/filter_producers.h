#pragma once

#include "args/args_parser.h"
#include "filter/filter.h"

#include <cstddef>

namespace filter_producers {

namespace detail {

double parseDouble(const std::string& text, const char* filter,
                   const char* param);
int parseInt(const std::string& text, const char* filter, const char* param);
void requireParams(const FilterDescriptor& filterDescriptor,
                   size_t requiredCount);
void requireParamsRange(const FilterDescriptor& filterDescriptor,
                        size_t minCount, size_t maxCount);

}  // namespace detail

IFilter* makeAmpl(const FilterDescriptor& filterDescriptor);
IFilter* makeNormalize(const FilterDescriptor& filterDescriptor);
IFilter* makeSilence(const FilterDescriptor& filterDescriptor);
IFilter* makeTimestretch(const FilterDescriptor& filterDescriptor);
IFilter* makeLowpass(const FilterDescriptor& filterDescriptor);
IFilter* makeHighpass(const FilterDescriptor& filterDescriptor);
IFilter* makeBandpass(const FilterDescriptor& filterDescriptor);
IFilter* makeReject(const FilterDescriptor& filterDescriptor);
IFilter* makeMute(const FilterDescriptor& filterDescriptor);
IFilter* makeMix(const FilterDescriptor& filterDescriptor);

IFilter* makeGeneratorSin(const FilterDescriptor& filterDescriptor,
                          size_t offset);
IFilter* makeGeneratorAm(const FilterDescriptor& filterDescriptor,
                         size_t offset);
IFilter* makeGeneratorFm(const FilterDescriptor& filterDescriptor,
                         size_t offset);
IFilter* makeGenerator(const FilterDescriptor& filterDescriptor);

}  // namespace filter_producers
