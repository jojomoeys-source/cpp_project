#include "app/application.h"

#include "filter_producers/filter_producers.h"
#include "wav/wav_io.h"
#include "waveform/waveform.h"

#include <iostream>

void Application::configure()
{
    _converter.addFilterProducer("ampl", filter_producers::makeAmpl);
    _converter.addFilterProducer("normalize", filter_producers::makeNormalize);
    _converter.addFilterProducer("silence", filter_producers::makeSilence);
    _converter.addFilterProducer("timestretch",
                                 filter_producers::makeTimestretch);
    _converter.addFilterProducer("lowpass", filter_producers::makeLowpass);
    _converter.addFilterProducer("highpass", filter_producers::makeHighpass);
    _converter.addFilterProducer("bandpass", filter_producers::makeBandpass);
    _converter.addFilterProducer("reject", filter_producers::makeReject);
    _converter.addFilterProducer("notch", filter_producers::makeReject);
    _converter.addFilterProducer("mute", filter_producers::makeMute);
    _converter.addFilterProducer("mix", filter_producers::makeMix);
    _converter.addFilterProducer("generator", filter_producers::makeGenerator);
}

int Application::start(int argc, char* argv[])
{
    ArgsParser parser;
    const ArgsParser::Result PARSE_RESULT = parser.parse(argc, argv);

    if(PARSE_RESULT == ArgsParser::Result::noArgs ||
       PARSE_RESULT == ArgsParser::Result::helpRequested)
    {
        printHelp();
        return 0;
    }

    if(PARSE_RESULT == ArgsParser::Result::badArgs)
    {
        std::cerr
            << "Error: invalid arguments. Run without arguments for usage.\n";
        return 1;
    }

    Pipeline pipeline = _converter.createPipeline(parser.getFilters());

    Waveform waveform;
    const std::string& inputFile = parser.getInputFile();
    const bool FIRST_FILTER_IS_GENERATOR =
        !parser.getFilters().empty() &&
        parser.getFilters().front().name == "generator";
    if(!inputFile.empty() && !FIRST_FILTER_IS_GENERATOR)
    {
        std::cout << "Reading: " << inputFile << "\n";
        waveform = WavReader::read(inputFile);
    }

    pipeline.apply(waveform);

    const std::string& outputFile = parser.getOutputFile();
    if(!outputFile.empty())
    {
        std::cout << "Writing: " << outputFile << "\n";
        WavWriter::write(outputFile, waveform);
    }

    std::cout << "Done.\n";
    return 0;
}

void Application::printHelp() const
{
    std::cout
        << "Usage: sound_processor [-i <input.wav>] [-o <output.wav>]"
           " [-f <filter> [args...]] ...\n"
        << "\n"
        << "Transforming filters:\n"
        << "  ampl        <factor>                         multiply every "
           "sample by factor (factor >= 0)\n"
        << "  normalize   [peak]                           scale so max sample "
           "= peak*32767 (default peak=1)\n"
        << "  silence     <sec|ms> <start> <end>           insert silence in "
           "[start, end]\n"
        << "  timestretch <factor>                         stretch/compress "
           "signal (factor > 0)\n"
        << "  lowpass     <window_size>                    smooth with moving "
           "average (odd window)\n"
        << "  highpass    <window_size>                    remove low "
           "frequencies (odd window)\n"
        << "  bandpass    <low_window> <high_window>       keep middle "
           "frequencies (odd windows)\n"
        << "  reject      <low_window> <high_window>       remove middle "
           "frequencies (odd windows)\n"
        << "  notch       <low_window> <high_window>       alias for reject\n"
        << "  mute        <start_sec> <end_sec>            zero out samples in "
           "[start, end]\n"
        << "  mix         <file.wav> <start_sec>           overlay another wav "
           "at start_sec\n"
        << "\n"
        << "Generator filters (ignore input signal):\n"
        << "  generator sin <freq_hz> <dur_ms>                               "
           "sine wave\n"
        << "  generator am  <amp> <carrier_hz> <mod_hz> <depth> <dur_ms>    AM "
           "signal\n"
        << "  generator fm  <amp> <carrier_hz> <mod_hz> <dev_hz> <dur_ms>   FM "
           "signal\n";
}