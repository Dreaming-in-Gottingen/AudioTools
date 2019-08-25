/*
 *******************************************************
 * Author       : jingzhou.zhang
 * Last modified: 2019-08-24 17:24
 * Email        : zjz1988314@gmail.com
 * Filename     : WavFileOpsTest.cpp
 * Description  : test libwav_ops.so to read/write wav files.
 ********************************************************
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>

#include "wav_ops.h"

//using namespace audiotools;

int main(int argc, char* argv[])
{
    puts("test WavReader/Writer operations, and read from argv[1] to write to argv[2]");
    if (argc != 3)
    {
        puts("error input! ./xx input_file output_file");
        return -1;
    }

//    WavReader file(argv[1]);
//    printf("Info => sr:%d, chn:%d, dura:%d ms\n", file.sample_rate(), file.channel_cnt(), file.duration_ms());
//
//    WavWriter wf(argv[2], file.sample_rate(), file.channel_cnt());
//    int16_t buf[file.sample_rate()/100 * file.channel_cnt()];
//    int cnt = file.sample_rate()/100;
//    int ret;
//    while ((ret = file.ReadSamples(buf, cnt)) != 0)
//    {
//        wf.WriteSamples(buf, ret);
//    }

    CWavReader *pwr = CWavReaderOpen(argv[1]);
    int wr_sr = CWavReader_GetSampleRate(pwr);
    int wr_chn = CWavReader_GetChannelCnt(pwr);
    int wr_dura = CWavReader_GetDurationMs(pwr);
    printf("input_file(%s) info: sr-%d, chn-%d, dura-%d ms\n", argv[1], wr_sr, wr_chn, wr_dura);

    CWavWriter *pww = CWavWriterOpen(argv[2], wr_sr, wr_chn);

    int smp_sz = wr_sr/100*wr_chn;  //10ms size
    int16_t *buf = (int16_t *)malloc(smp_sz*2);
    int wr_cnt;
    while ((wr_cnt = CWavReader_GetSamples(pwr, buf, wr_sr/100)) != 0)
    {
        CWavWriter_AddSamples(pww, buf, wr_cnt);
    }
    CWavWriterClose(pww);
    printf("copy %s to %s finish!\n", argv[1], argv[2]);

    free(buf);

    return 0;
}
