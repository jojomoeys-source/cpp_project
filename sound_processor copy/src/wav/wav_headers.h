#pragma once
#include <cstdint>

#pragma pack(push, 1)

struct RiffHeader
{
    char chunkId[4];
    uint32_t chunkSize;
    char format[4];
};

struct FmtHeader
{
    char subchunkId[4];
    uint32_t subchunkSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
};

struct DataHeader
{
    char subchunkId[4];
    uint32_t subchunkSize;
};

#pragma pack(pop)