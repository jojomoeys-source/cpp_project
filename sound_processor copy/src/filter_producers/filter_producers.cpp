#include "filter_producers/filter_producers.h"

#include "filters/ampl_filter.h"
#include "filters/bandpass_filter.h"
#include "filters/generator_filters.h"
#include "filters/highpass_filter.h"
#include "filters/lowpass_filter.h"
#include "filters/mix_filter.h"
#include "filters/mute_filter.h"
#include "filters/normalize_filter.h"
#include "filters/reject_filter.h"
#include "filters/silence_filter.h"
#include "filters/timestretch_filter.h"
#include "wav/wav_io.h"

#include <cmath>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

namespace filter_producers {

namespace detail {

double parseDouble(const std::string& text, const char* filter,
                   const char* param)
{
    try
    {
        size_t pos = 0;
        const double VALUE = std::stod(text, &pos);
        if(pos != text.size() || !std::isfinite(VALUE))
            throw std::invalid_argument("invalid number");
        return VALUE;
    }
    catch(const std::exception&)
    {
        throw std::runtime_error(
            std::string("Filter '") + filter + "': " + param +
            " must be a valid number, got: '" + text + "'");
    }
}

int parseInt(const std::string& text, const char* filter, const char* param)
{
    try
    {
        size_t pos = 0;
        const int VALUE = std::stoi(text, &pos);
        if(pos != text.size())
            throw std::invalid_argument("invalid integer");
        return VALUE;
    }
    catch(const std::exception&)
    {
        throw std::runtime_error(
            std::string("Filter '") + filter + "': " + param +
            " must be a valid integer, got: '" + text + "'");
    }
}

void requireParams(const FilterDescriptor& filterDescriptor,
                   size_t requiredCount)
{
    if(filterDescriptor.params.size() != requiredCount)
    {
        throw std::runtime_error(
            "Filter '" + filterDescriptor.name + "' requires " +
            std::to_string(requiredCount) + " argument(s), got " +
            std::to_string(filterDescriptor.params.size()));
    }
}

void requireParamsRange(const FilterDescriptor& filterDescriptor,
                        size_t minCount, size_t maxCount)
{
    if(filterDescriptor.params.size() < minCount ||
       filterDescriptor.params.size() > maxCount)
    {
        throw std::runtime_error(
            "Filter '" + filterDescriptor.name + "' requires from " +
            std::to_string(minCount) + " to " + std::to_string(maxCount) +
            " argument(s), got " +
            std::to_string(filterDescriptor.params.size()));
    }
}

}  // namespace detail

IFilter* makeAmpl(const FilterDescriptor& filterDescriptor)
{
    detail::requireParams(filterDescriptor, 1);
    const double FACTOR =
        detail::parseDouble(filterDescriptor.params[0], "ampl", "factor");
    return new AmplFilter(FACTOR);
}

IFilter* makeNormalize(const FilterDescriptor& filterDescriptor)
{
    detail::requireParamsRange(filterDescriptor, 0, 1);
    if(filterDescriptor.params.empty())
        return new NormalizeFilter();
    const double PEAK =
        detail::parseDouble(filterDescriptor.params[0], "normalize", "peak");
    return new NormalizeFilter(PEAK);
}

IFilter* makeSilence(const FilterDescriptor& filterDescriptor)
{
    detail::requireParams(filterDescriptor, 3);
    const SilenceFilter::Unit UNIT =
        SilenceFilter::parseUnit(filterDescriptor.params[0]);
    const double START =
        detail::parseDouble(filterDescriptor.params[1], "silence", "start");
    const double END =
        detail::parseDouble(filterDescriptor.params[2], "silence", "end");
    return new SilenceFilter(UNIT, START, END);
}

IFilter* makeTimestretch(const FilterDescriptor& filterDescriptor)
{
    detail::requireParams(filterDescriptor, 1);
    const double FACTOR = detail::parseDouble(filterDescriptor.params[0],
                                              "timestretch", "factor");
    return new TimestretchFilter(FACTOR);
}

IFilter* makeLowpass(const FilterDescriptor& filterDescriptor)
{
    detail::requireParams(filterDescriptor, 1);
    const int LOW_WINDOW =
        detail::parseInt(filterDescriptor.params[0], "lowpass", "window_size");
    return new LowpassFilter(LOW_WINDOW);
}

IFilter* makeHighpass(const FilterDescriptor& filterDescriptor)
{
    detail::requireParams(filterDescriptor, 1);
    const int HIGH_WINDOW =
        detail::parseInt(filterDescriptor.params[0], "highpass", "window_size");
    return new HighpassFilter(HIGH_WINDOW);
}

IFilter* makeBandpass(const FilterDescriptor& filterDescriptor)
{
    detail::requireParams(filterDescriptor, 2);
    const int LOW_WINDOW = detail::parseInt(filterDescriptor.params[0],
                                            "bandpass", "low_window_size");
    const int HIGH_WINDOW = detail::parseInt(filterDescriptor.params[1],
                                             "bandpass", "high_window_size");
    return new BandpassFilter(LOW_WINDOW, HIGH_WINDOW);
}

IFilter* makeReject(const FilterDescriptor& filterDescriptor)
{
    detail::requireParams(filterDescriptor, 2);
    const int LOW_WINDOW = detail::parseInt(filterDescriptor.params[0],
                                            "reject", "low_window_size");
    const int HIGH_WINDOW = detail::parseInt(filterDescriptor.params[1],
                                             "reject", "high_window_size");
    return new RejectFilter(LOW_WINDOW, HIGH_WINDOW);
}

IFilter* makeMute(const FilterDescriptor& filterDescriptor)
{
    detail::requireParams(filterDescriptor, 2);
    const double START =
        detail::parseDouble(filterDescriptor.params[0], "mute", "start_sec");
    const double END =
        detail::parseDouble(filterDescriptor.params[1], "mute", "end_sec");
    return new MuteFilter(START, END);
}

IFilter* makeMix(const FilterDescriptor& filterDescriptor)
{
    detail::requireParams(filterDescriptor, 2);
    const double START =
        detail::parseDouble(filterDescriptor.params[1], "mix", "start_sec");
    Waveform additional = WavReader::read(filterDescriptor.params[0]);
    return new MixFilter(std::move(additional), START);
}

IFilter* makeGeneratorSin(const FilterDescriptor& filterDescriptor,
                          size_t offset)
{
    detail::requireParams(filterDescriptor, offset + 2);
    const double FREQ = detail::parseDouble(filterDescriptor.params[offset],
                                            "generator sin", "frequency_hz");
    const double DUR = detail::parseDouble(filterDescriptor.params[offset + 1],
                                           "generator sin", "duration_ms");
    return new SinGeneratorFilter(FREQ, DUR);
}

IFilter* makeGeneratorAm(const FilterDescriptor& filterDescriptor,
                         size_t offset)
{
    detail::requireParams(filterDescriptor, offset + 5);
    const double AMPLITUDE = detail::parseDouble(
        filterDescriptor.params[offset], "generator am", "amplitude");
    const double CARRIER_HZ = detail::parseDouble(
        filterDescriptor.params[offset + 1], "generator am", "carrier_hz");
    const double MODULATION_HZ = detail::parseDouble(
        filterDescriptor.params[offset + 2], "generator am", "modulation_hz");
    const double DEPTH = detail::parseDouble(
        filterDescriptor.params[offset + 3], "generator am", "depth");
    const double DURATION_MS = detail::parseDouble(
        filterDescriptor.params[offset + 4], "generator am", "duration_ms");
    return new AmGeneratorFilter(AMPLITUDE, CARRIER_HZ, MODULATION_HZ, DEPTH,
                                 DURATION_MS);
}

IFilter* makeGeneratorFm(const FilterDescriptor& filterDescriptor,
                         size_t offset)
{
    detail::requireParams(filterDescriptor, offset + 5);
    const double AMPLITUDE = detail::parseDouble(
        filterDescriptor.params[offset], "generator fm", "amplitude");
    const double CARRIER_HZ = detail::parseDouble(
        filterDescriptor.params[offset + 1], "generator fm", "carrier_hz");
    const double MODULATION_HZ = detail::parseDouble(
        filterDescriptor.params[offset + 2], "generator fm", "modulation_hz");
    const double DEVIATION_HZ = detail::parseDouble(
        filterDescriptor.params[offset + 3], "generator fm", "deviation_hz");
    const double DURATION_MS = detail::parseDouble(
        filterDescriptor.params[offset + 4], "generator fm", "duration_ms");
    return new FmGeneratorFilter(AMPLITUDE, CARRIER_HZ, MODULATION_HZ,
                                 DEVIATION_HZ, DURATION_MS);
}

IFilter* makeGenerator(const FilterDescriptor& filterDescriptor)
{
    if(filterDescriptor.params.empty())
    {
        throw std::runtime_error(
            "Filter 'generator' requires subtype: sin, am, or fm");
    }
    const std::string& subtype = filterDescriptor.params[0];
    if(subtype == "sin")
        return makeGeneratorSin(filterDescriptor, 1);
    if(subtype == "am")
        return makeGeneratorAm(filterDescriptor, 1);
    if(subtype == "fm")
        return makeGeneratorFm(filterDescriptor, 1);
    throw std::runtime_error("Filter 'generator': unknown subtype '" + subtype +
                             "'. Expected: sin, am, fm");
}

}  // namespace filter_producers
