#include "args/args_parser.h"
#include <cctype>

static bool isFlag(const char* token)
{
    if(token[0] != '-')
        return false;
    if(token[1] == '\0')
        return false;
    return std::isalpha(static_cast<unsigned char>(token[1])) != 0;
}

ArgsParser::Result ArgsParser::parse(int argc, char* argv[])
{
    _inFileName.clear();
    _outFileName.clear();
    _filterDescriptors.clear();

    if(argc < 2)
        return Result::noArgs;

    for(int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if(arg == "-h" || arg == "--help")
        {
            return Result::helpRequested;
        }
        else if(arg == "-i")
        {
            if(i + 1 >= argc || argv[i + 1][0] == '\0' || isFlag(argv[i + 1]))
                return Result::badArgs;
            if(!_inFileName.empty())
                return Result::badArgs;
            _inFileName = argv[++i];
        }
        else if(arg == "-o")
        {
            if(i + 1 >= argc || argv[i + 1][0] == '\0' || isFlag(argv[i + 1]))
                return Result::badArgs;
            if(!_outFileName.empty())
                return Result::badArgs;
            _outFileName = argv[++i];
        }
        else if(arg == "-f")
        {
            if(i + 1 >= argc || isFlag(argv[i + 1]))
                return Result::badArgs;
            FilterDescriptor filterDescriptor;
            filterDescriptor.name = argv[++i];
            if(filterDescriptor.name.empty())
                return Result::badArgs;

            while(i + 1 < argc && !isFlag(argv[i + 1]))
                filterDescriptor.params.push_back(argv[++i]);
            _filterDescriptors.push_back(filterDescriptor);
        }
        else
        {
            return Result::badArgs;
        }
    }

    return Result::ok;
}
