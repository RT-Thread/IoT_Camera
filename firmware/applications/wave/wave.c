#include <stdio.h>
#include <string.h>
#include <dfs_posix.h>
#include "wave.h"

int init_wave_header(struct WAVE_HEADER *h)
{
    strcpy(h->riffHeader.szRiffID, "RIFF");
    h->riffHeader.dwRiffSize = 0;
    strcpy(h->riffHeader.szRiffFormat, "WAVE");

    strcpy(h->fmtBlock.szFmtID, "fmt ");
    h->fmtBlock.dwFmtSize                  = 16;
    h->fmtBlock.wavFormat.wFormatTag       = 1;  // PCM
    h->fmtBlock.wavFormat.wChannels        = 1;  // mono
    h->fmtBlock.wavFormat.dwSamplesPerSec  = 8000;
    h->fmtBlock.wavFormat.wBitsPerSample   = 16;
    h->fmtBlock.wavFormat.dwAvgBytesPerSec = h->fmtBlock.wavFormat.dwSamplesPerSec * h->fmtBlock.wavFormat.wChannels *
                                             h->fmtBlock.wavFormat.wBitsPerSample / 8;
    h->fmtBlock.wavFormat.wBlockAlign = h->fmtBlock.wavFormat.wChannels * h->fmtBlock.wavFormat.wBitsPerSample / 8;

    strcpy(h->factBlock.szFactID, "fact");
    h->factBlock.dwFactSize = 0;

    strcpy(h->dataBlock.szDataID, "data");
    h->dataBlock.dwDataSize = 0;

    return 0;
}

int read_riff_header(int fd, struct RIFF_HEADER *h)
{
    read(fd, &h->dwRiffSize, 4);
    read(fd, h->szRiffFormat, 4);

    return 0;
}

int read_fmt_block(int fd, struct FMT_BLOCK *h)
{
    read(fd, &h->dwFmtSize, 4);
    read(fd, &h->wavFormat, h->dwFmtSize);

    return 0;
}

int read_fact_block(int fd, struct FACT_BLOCK *h)
{
    unsigned int dummy;
    int cnt;

    read(fd, &h->dwFactSize, 4);

    cnt = h->dwFactSize / 4;
    while (cnt--)
        read(fd, &dummy, 1);

    return 0;
}

int read_data_header(int fd, struct DATA_BLOCK *h)
{
    read(fd, &h->dwDataSize, 4);

    return 0;
}

int get_wave_header(int fd, struct WAVE_HEADER *h)
{
    unsigned int id;
    int file_size;

    file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    int is_data_block = 0;

    while (!is_data_block)
    {
        if (0 > read(fd, &id, 4))
        {
            printf("file corrupted.\n");
            return -1;
        }

        switch (id)
        {
        case 0x46464952:  // RIFF
            *(unsigned int *)h->riffHeader.szRiffID = id;
            read_riff_header(fd, &h->riffHeader);

            if (file_size - 8 != h->riffHeader.dwRiffSize)
            {
                printf("WARNING: file size not match\n");
            }

            if (*(unsigned int *)h->riffHeader.szRiffFormat != 0x45564157)  // WAVE
            {
                printf("invaild wave file.\n");
                return -1;
            }
            break;
        case 0x20746d66:  // fmt
            *(unsigned int *)h->fmtBlock.szFmtID = id;
            read_fmt_block(fd, &h->fmtBlock);
            break;
        case 0x74636166:  // fact
            *(unsigned int *)h->factBlock.szFactID = id;
            read_fact_block(fd, &h->factBlock);
            break;
        case 0x61746164:  // data
            *(unsigned int *)h->dataBlock.szDataID = id;
            read_data_header(fd, &h->dataBlock);
            is_data_block = 1;
            break;
        default:
            printf("block type unknown.\n");
            return -1;
        }
    }

    return lseek(fd, 0, SEEK_CUR);
}
