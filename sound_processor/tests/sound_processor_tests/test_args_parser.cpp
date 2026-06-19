#include <catch2/catch_shim.hpp>

#include "args/args_parser.h"

#include <vector>

static ArgsParser parseFull(std::vector<const char*> args)
{
    args.insert(args.begin(), "sound_processor");
    ArgsParser parser;
    parser.parse(static_cast<int>(args.size()),
                 const_cast<char**>(args.data()));
    return parser;
}

TEST_CASE("ArgsParser returns NoArgs without arguments", "[args_parser]")
{
    ArgsParser parser;
    std::vector<const char*> argv = {"sound_processor"};

    CHECK(parser.parse(static_cast<int>(argv.size()),
                       const_cast<char**>(argv.data())) ==
          ArgsParser::Result::noArgs);
};

TEST_CASE("ArgsParser returns HelpRequested for help", "[args_parser]")
{
    ArgsParser parser;
    std::vector<const char*> argv = {"sound_processor", "--help"};

    CHECK(parser.parse(static_cast<int>(argv.size()),
                       const_cast<char**>(argv.data())) ==
          ArgsParser::Result::helpRequested);
}

TEST_CASE("ArgsParser parses input output and filters", "[args_parser]")
{
    const auto PARSER = parseFull({
        "-i",
        "input.wav",
        "-o",
        "output.wav",
        "-f",
        "ampl",
        "-0.5",
        "-f",
        "normalize",
    });

    CHECK(PARSER.getInputFile() == "input.wav");
    CHECK(PARSER.getOutputFile() == "output.wav");

    const auto& filters = PARSER.getFilters();
    REQUIRE(filters.size() == 2);
    CHECK(filters[0].name == "ampl");
    REQUIRE(filters[0].params.size() == 1);
    CHECK(filters[0].params[0] == "-0.5");
    CHECK(filters[1].name == "normalize");
    CHECK(filters[1].params.empty());
}
