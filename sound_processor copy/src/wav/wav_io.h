#pragma once

#include "waveform/waveform.h"
#include <string>

class WavReader
{
public:
    static Waveform read(const std::string& filePath);
};

class WavWriter
{
public:
    static void write(const std::string& filePath, const Waveform& waveform);
};