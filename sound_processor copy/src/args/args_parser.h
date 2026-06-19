#pragma once

#include <string>
#include <vector>

struct FilterDescriptor
{
    std::string name;
    std::vector<std::string> params;
};

class ArgsParser
{
public:
    enum class Result
    {
        ok,
        noArgs,
        helpRequested,
        badArgs
    };

    ArgsParser() = default;

    Result parse(int argc, char* argv[]);

    const std::string& getInputFile() const { return _inFileName; }
    const std::string& getOutputFile() const { return _outFileName; }
    const std::vector<FilterDescriptor>& getFilters() const
    {
        return _filterDescriptors;
    }

private:
    std::string _inFileName;
    std::string _outFileName;
    std::vector<FilterDescriptor> _filterDescriptors;
};
