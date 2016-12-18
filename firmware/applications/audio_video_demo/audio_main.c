#include <string.h>
#include <stdio.h>
#include <rtthread.h>
#include <dfs_posix.h>
#include "isp_api.h"
#include "FHAdv_audio_cap.h"
#include "FHAdv_audio_play.h"
#include "wave.h"

#define SEC_PER_TICK (1 / RT_TICK_PER_SECOND)

static int g_cap_wav_fd                 = -1;

extern void sdk_mem_init(void);

void print_help(void)
{
    printf("Available function:\n");
    printf("   audio_demo(\"help\")                                            print help info\n");
    printf("   audio_demo(\"cap\", seconds, sample_rate, sample_bits, volume)  capture audio and save .wav file\n");
    printf("   audio_demo(\"play\", \"filename\", volume)                      play a .wav file\n");
    printf("   audio_demo(\"loop\", sample_rate, sample_bits, volume)          capture and then play\n");
    printf("   audio_demo(\"vol-cap\", new_volume)                             set capture volume\n");
    printf("   audio_demo(\"vol-play\", new_volume)                            set play volume\n");
    printf("   audio_demo(\"exit\")                                            close audio engine\n");
}

void audio_demo_exit(void)
{
    FHAdv_Aucap_Close();
    FHAdv_Auply_Close();
}

void capture_callback(const FHADV_AUCAP_FRM_INFO_t *info, const FH_ADDR data, const FH_SINT32 len)
{
    if (g_cap_wav_fd < 0)
        return;
    write(g_cap_wav_fd, data, len);
}

void capture_loop_callback(const FHADV_AUCAP_FRM_INFO_t *info, const FH_ADDR data, const FH_SINT32 len)
{
    FHAdv_Auply_Play(data, len);
}

void audio_cap(int duration, int sample_rate, int sample_bits, int vol)
{
    FHADV_AUCAP_CONFIG_t config;
    struct WAVE_HEADER wav_hdr;
    char filename[32];
    int total_size;
    int ret;

    config.sample_rate = sample_rate;
    config.bit_depth   = sample_bits;
    config.volume      = vol;
    config.cb          = capture_callback;
    config.input_if    = FHADV_AUCAP_MIC_IN;

    snprintf(filename, 32, "cap_%d_%d_%d.wav", sample_rate, sample_bits, vol);
    g_cap_wav_fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0);
    if (g_cap_wav_fd < 0)
    {
        printf("Error: failed to crate .wav file\n");
        return;
    }

    total_size = duration * sample_rate * sample_bits / 8;
    init_wave_header(&wav_hdr);
    wav_hdr.riffHeader.dwRiffSize               = sizeof(struct WAVE_HEADER) + total_size - 8;
    wav_hdr.fmtBlock.wavFormat.dwSamplesPerSec  = sample_rate;
    wav_hdr.fmtBlock.wavFormat.wBitsPerSample   = sample_bits;
    wav_hdr.fmtBlock.wavFormat.dwAvgBytesPerSec = sample_bits * sample_rate / 8;
    wav_hdr.fmtBlock.wavFormat.wBlockAlign      = sample_bits / 8;
    wav_hdr.dataBlock.dwDataSize                = total_size;
    write(g_cap_wav_fd, &wav_hdr, sizeof(struct WAVE_HEADER));

    sdk_mem_init();
    ret = FHAdv_Aucap_Open(&config);
    if (FH_SUCCESS != ret)
        return;
    FHAdv_Aucap_Start();
    rt_thread_delay(duration * RT_TICK_PER_SECOND);
    FHAdv_Aucap_Stop();
    FHAdv_Aucap_Close();
    close(g_cap_wav_fd);
}

void audio_play(const char *filename, int vol)
{
    int fd = -1;
    struct WAVE_HEADER wav_hdr;
    FHADV_AUPLY_CONFIG_t config;
    int left_size, read_size, real_bytes;
    FH_ADDR buf;
    int ret;

    fd = open(filename, O_RDONLY, 0);
    if (get_wave_header(fd, &wav_hdr) < 0)
    {
        printf("Error: the given file is no wav file\n");
        close(fd);
        return;
    }
    printf("******** Wave file info ********\n");
    printf("Format Tag:            %d\n", wav_hdr.fmtBlock.wavFormat.wFormatTag);
    printf("Channels:              %d\n", wav_hdr.fmtBlock.wavFormat.wChannels);
    printf("Samples Rate:          %d Hz\n", wav_hdr.fmtBlock.wavFormat.dwSamplesPerSec);
    printf("Bits per Sample:       %d\n", wav_hdr.fmtBlock.wavFormat.wBitsPerSample);
    printf("total bytes:           %d\n", wav_hdr.dataBlock.dwDataSize);
    printf("Average Bps:           %d\n", wav_hdr.fmtBlock.wavFormat.dwAvgBytesPerSec);
    printf("Duration:              %d ms\n",
           1000 * wav_hdr.dataBlock.dwDataSize / wav_hdr.fmtBlock.wavFormat.dwAvgBytesPerSec);
    printf("********************************\n");

    left_size = wav_hdr.dataBlock.dwDataSize;
    if (wav_hdr.fmtBlock.wavFormat.wBitsPerSample != 24)
        read_size = 1024 * wav_hdr.fmtBlock.wavFormat.wBitsPerSample / 8;
    else
        read_size = 1024 * 4;

    buf = rt_malloc(read_size);
    if (buf == RT_NULL)
    {
        printf("Error: failed to allocate read buffer\n");
        return;
    }

    config.sample_rate = wav_hdr.fmtBlock.wavFormat.dwSamplesPerSec;
    config.bit_depth   = wav_hdr.fmtBlock.wavFormat.wBitsPerSample;
    config.volume      = vol;
    config.output_if   = FHADV_AUPLY_LINE_OUT;
    ret                = FHAdv_Auply_Open(&config);
    if (FH_SUCCESS != ret)
        return;

    while (left_size > 0)
    {
        real_bytes = read(fd, buf, read_size);
        if (real_bytes != read_size)
            break;
        FHAdv_Auply_Play(buf, real_bytes);
        left_size -= real_bytes;
    }
    FHAdv_Auply_Close();
    rt_free(buf);
    close(fd);
}

void audio_loop(int sample_rate, int sample_bits, int vol)
{
    FHADV_AUCAP_CONFIG_t cap_config;
    FHADV_AUPLY_CONFIG_t play_config;
    char *buf;
    int len;

    cap_config.sample_rate = sample_rate;
    cap_config.bit_depth   = sample_bits;
    cap_config.volume      = vol;
    cap_config.cb          = capture_loop_callback;
    cap_config.input_if    = FHADV_AUCAP_MIC_IN;

    play_config.sample_rate = sample_rate;
    play_config.bit_depth   = sample_bits;
    play_config.volume      = vol;
    play_config.output_if   = FHADV_AUPLY_LINE_OUT;

    len = 4 * 1024;
    buf = rt_malloc(len);

    if (RT_NULL == buf)
    {
        printf("Error: failed to allocate %d bytes buffer\n", len);
        return;
    }
    rt_memset(buf, 0, len);

    sdk_mem_init();
    FHAdv_Auply_Open(&play_config);
    FHAdv_Aucap_Open(&cap_config);

    FHAdv_Aucap_Start();
}

void audio_set_cap_vol(int new_vol)
{
    FHAdv_Aucap_SetVolume(new_vol);
}

void audio_set_play_vol(int new_vol)
{
    FHAdv_Auply_SetVolume(new_vol);
}

void audio_demo(char *cmd, ...)
{
    va_list args;

    if (RT_NULL == cmd)
    {
        print_help();
    }

    va_start(args, cmd);

    if (strcmp(cmd, "help") == 0)
    {
        print_help();
    }
    else if (strcmp(cmd, "exit") == 0)
    {
        audio_demo_exit();
    }
    else if (strcmp(cmd, "cap") == 0)
    {
        int duration, sample_rate, sample_bits, vol;

        duration    = va_arg(args, int);
        sample_rate = va_arg(args, int);
        sample_bits = va_arg(args, int);
        vol         = va_arg(args, int);
        audio_cap(duration, sample_rate, sample_bits, vol);
    }
    else if (strcmp(cmd, "play") == 0)
    {
        char *filename;
        int vol;

        filename = va_arg(args, char *);
        vol      = va_arg(args, int);
        audio_play(filename, vol);
    }
    else if (strcmp(cmd, "loop") == 0)
    {
        int sample_rate, sample_bits, vol;

        sample_rate = va_arg(args, int);
        sample_bits = va_arg(args, int);
        vol         = va_arg(args, int);
        audio_loop(sample_rate, sample_bits, vol);
    }
    else if (strcmp(cmd, "vol-cap") == 0)
    {
        int new_vol;

        new_vol = va_arg(args, int);
        audio_set_cap_vol(new_vol);
    }
    else if (strcmp(cmd, "vol-play") == 0)
    {
        int new_vol;

        new_vol = va_arg(args, int);
        audio_set_play_vol(new_vol);
    }
    else
    {
        printf("Invalid function cmd !\n");
        print_help();
    }
    va_end(args);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(audio_demo, use audio_demo("help") for more info);
#endif
