#ifndef _WAVE_H_
#define _WAVE_H_

struct RIFF_HEADER
{
    char szRiffID[4];  // 'R','I','F','F'
    unsigned int dwRiffSize;
    char szRiffFormat[4];  // 'W','A','V','E'
};

struct WAVE_FORMAT
{
    unsigned short wFormatTag;      // 1:PCM
    unsigned short wChannels;       // 1: mono; 2:stereo
    unsigned int dwSamplesPerSec;   // 0x1F40 = 8000Hz
    unsigned int dwAvgBytesPerSec;  // = dwSamplesPerSec * wChannels * wBitsPerSample / 8
    unsigned short wBlockAlign;     // = wChannels * wBitsPerSample / 8
    unsigned short wBitsPerSample;  // 16
};

struct FMT_BLOCK
{
    char szFmtID[4];         // 'f','m','t',' '
    unsigned int dwFmtSize;  // 16
    struct WAVE_FORMAT wavFormat;
};

struct FACT_BLOCK
{
    char szFactID[4];  // 'f','a','c','t'
    unsigned int dwFactSize;
};

struct DATA_BLOCK
{
    char szDataID[4];  // 'd','a','t','a'
    unsigned int dwDataSize;
};

struct WAVE_HEADER
{
    struct RIFF_HEADER riffHeader;
    struct FMT_BLOCK fmtBlock;
    struct FACT_BLOCK factBlock;
    struct DATA_BLOCK dataBlock;
};

int init_wave_header(struct WAVE_HEADER *h);
int get_wave_header(int fd, struct WAVE_HEADER *h);

#endif
