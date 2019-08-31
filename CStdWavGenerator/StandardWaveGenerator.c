/*
 *******************************************************
 * Author       : jingzhou.zhang
 * Last modified: 2019-08-31 12:04
 * Email        : zjz1988314@gmail.com
 * Filename     : StandardWaveGenerator.c
 * Description  : sin/square/triangle generator
 ********************************************************
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

struct WaveHeader{
    int riff_id;
    int riff_sz;
    int riff_fmt;
    int fmt_id;
    int fmt_sz;
    short audio_fmt;
    short num_chn;
    int sample_rate;
    int byte_rate;
    short block_align;
    short bits_per_sample;
    int data_id;
    int data_sz;
};

int audio_resample(int srcSr, int srcChn, short* pSrcBuf, int srcFrmLen, int dstSr, int dstChn, short* pDstBuf)
{
    int frmDuration = 1000LL*srcFrmLen/srcSr;
    printf("[f:%s, l:%d] (srcSr:%d, srcChn:%d) -> (dstSr:%d, dstChn:%d), duration:%d ms\n", __func__, __LINE__, srcSr, srcChn, dstSr, dstChn, frmDuration);

    int dstFrmCnt = 1LL * srcFrmLen * dstSr / srcSr;
    short *pTmpBuf = malloc(srcFrmLen * dstChn * 2);

    // chn uniform
    if (srcChn != dstChn)
    {
        printf("chn convert:\t %d -> %d\n", srcChn, dstChn);
        if (srcChn == 2)
        {
            for (int i=0; i<srcFrmLen; i++)
                pTmpBuf[i] = ((pSrcBuf[i*2]) + (pSrcBuf[i*2+1])) / 2;
        }
        else
        {
            for (int i=0; i<srcFrmLen; i++)
            {
                pTmpBuf[i*2] = pTmpBuf[i*2+1] = pSrcBuf[i];
            }
        }
    }
    else
    {
        memcpy(pTmpBuf, pSrcBuf, srcFrmLen*srcChn*2);
    }

    // sr convert
    if (srcSr != dstSr)
    {
        printf("sr convert:\t %d -> %d\n", srcSr, dstSr);
        // resample
        for (int i=0; i<dstFrmCnt; i++)
        {
            if (dstChn == 2)
            {
                // 1.be careful for idx overflow
                // 2.(...*2): extract pcm from each channel of src, and put them into the same dst channel, this can reduce noise
                pDstBuf[i*2] = pTmpBuf[1LL*i*(srcSr)/(dstSr) * 2];
                pDstBuf[i*2+1] = pTmpBuf[1LL*i*(srcSr)/(dstSr) * 2 + 1];
                //pDstBuf[i*2] = pTmpBuf[2LL*i*(srcSr-1)/(dstSr-1)];
                //pDstBuf[i*2+1] = pTmpBuf[2LL*i*(srcSr-1)/(dstSr-1) + 1];
                //pDstBuf[i*2] = 0x5c00;
                //pDstBuf[i*2+1] = 0x9000;
            }
            else
            {
                pDstBuf[i] = pTmpBuf[1LL*i*(srcSr)/(dstSr)];
                //pDstBuf[i] = pTmpBuf[1LL*i*(srcSr-1)/(dstSr-1)];
            }
        }
    }
    else
    {
        memcpy(pDstBuf, pTmpBuf, srcFrmLen*dstChn*2);
    }

    free(pTmpBuf);

    return 0;
}

int audio_mixer(short *pDstBuf, short *pBuf0, short *pBuf1, int mixLen)
{
    for (int i=0; i<mixLen; i++)
        pDstBuf[i] = (pBuf0[i] + pBuf1[i])/2;
    return 0;
}

void audio_fix_wavheader(struct WaveHeader *pHeader, int sampleRate, int chnCnt, int pcmSize)
{
    memcpy(&pHeader->riff_id, "RIFF", 4);
    pHeader->riff_sz = pcmSize + sizeof(struct WaveHeader) - 8;
    memcpy(&pHeader->riff_fmt, "WAVE", 4);
    memcpy(&pHeader->fmt_id, "fmt ", 4);
    pHeader->fmt_sz = 16;
    pHeader->audio_fmt = 1;       // s16le
    pHeader->num_chn = chnCnt;
    pHeader->sample_rate = sampleRate;
    pHeader->byte_rate = sampleRate * chnCnt * 2;
    pHeader->block_align = chnCnt * 2;
    pHeader->bits_per_sample = 16;
    memcpy(&pHeader->data_id, "data", 4);
    pHeader->data_sz = pcmSize;
}

void audio_generator(short *pPcmBuf, int pcmLen, int wavType, int wavSR, int wavChn, int wavPeriod, int pcmGain)
{
    float totalPeriodW;
    int semiPeriod;
    double gainFactor = pow(10, pcmGain/20.0);
    switch(wavType)
    {
        case 0:
            totalPeriodW = 2*M_PI/(wavSR*wavPeriod/1000);
            if (wavChn == 1)
            {
                for (int i=0; i<pcmLen/2; i++)
                {
                    pPcmBuf[i] = 32767*sin(i*totalPeriodW);
                    pPcmBuf[i] = (short)(pPcmBuf[i] * gainFactor);
                }
            }
            else
            {
                for (int i=0; i<pcmLen/4; i++)
                {
                    double val = 32767.0 * sin(i * totalPeriodW);
                    pPcmBuf[2*i] = pPcmBuf[2*i+1] = (short)(val * gainFactor);
                }
            }
            break;
        case 1:
            semiPeriod = wavSR*wavPeriod/1000L/2;
            short val = (short)(32767.0 * gainFactor);
            if (wavChn == 1)
            {
                for (int i=0; i<pcmLen/2; i++)
                {
                    if (i/semiPeriod%2==0)
                        pPcmBuf[i] = val;
                    else
                        pPcmBuf[i] = -val;
                }
            }
            else
            {
                for (int i=0; i<pcmLen/4; i++)
                {
                    if (i/semiPeriod%2==0)
                        pPcmBuf[2*i] = pPcmBuf[2*i+1] = val;
                    else
                        pPcmBuf[2*i] = pPcmBuf[2*i+1] = -val;
                }
            }
            break;
        case 2:
            semiPeriod = wavSR*wavPeriod/1000L/2;
            if (wavChn == 1)
            {
                for (int i=0; i<pcmLen/2; i++)
                {
                    int offsetCnt = i/semiPeriod;
                    if (offsetCnt%2 == 0)
                    {
                        //pPcmBuf[i] = -65535LL*(i-(offsetCnt*semiPeriod + semiPeriod/2))/semiPeriod;
                        short val = -65535LL*(i-(offsetCnt*semiPeriod + semiPeriod/2))/semiPeriod;
                        pPcmBuf[i] = (short)(gainFactor * val);
                    }
                    else
                    {
                        //pPcmBuf[i] = 65535LL*(i-(offsetCnt*semiPeriod + 3*semiPeriod/2))/semiPeriod;
                        short val = 65535LL*(i-(offsetCnt*semiPeriod + 3*semiPeriod/2))/semiPeriod;
                        pPcmBuf[i] = (short)(gainFactor * val);
                    }
                }
            }
            else
            {
                for (int i=0; i<pcmLen/4; i++)
                {
                    int offsetCnt = i/semiPeriod;
                    short val = -65535LL*(i-(offsetCnt*semiPeriod + semiPeriod/2))/semiPeriod;
                    if (offsetCnt%2 == 0)
                        pPcmBuf[2*i] = pPcmBuf[2*i+1] = (short)(gainFactor * val);
                    else
                        pPcmBuf[2*i] = pPcmBuf[2*i+1] = -(short)(gainFactor * val);
                }
            }
            break;
    }
}

int main(int argc, char **argv)
{
    int ret = 0;
    int wavType= 0;
    int wavDuration = 1000;
    int wavPeriod = 100;
    int wavSR = 8000;
    int wavChn = 1;
    int wavMaxGain = 0; // 20lg(pcm_val/32768) = gain

    if (argc != 7)
    {
        puts("error inputs! param must be this: StandardWavGenerator type(0-sin,1-square,2-triangle) duration(ms) period(ms) sample_rate channel gain");
        return -1;
    }
    else
    {
        wavType = atoi(argv[1]);
        wavDuration = atoi(argv[2]);
        wavPeriod = atoi(argv[3]);
        wavSR = atoi(argv[4]);
        wavChn = atoi(argv[5]);
        wavMaxGain = atoi(argv[6]);
    }

    if (wavMaxGain > 0)
    {
        printf("Input gain(%d) error! should be nagative value! use default(0dB)!\n", wavMaxGain);
        wavMaxGain = 0;
    }

    char wavName[64] = {0};
    switch (wavType)
    {
        case 0:
            strncpy(wavName, "sin.wav", 32);
            break;
        case 1:
            strncpy(wavName, "square.wav", 32);
            break;
        case 2:
            strncpy(wavName, "triangle.wav", 32);
            break;
        default:
            strncpy(wavName, "sin.wav", 32);
            break;
    }

    FILE *dstFp = fopen(wavName, "wb");
    if (!dstFp)
    {
        printf("open dst file(%s) failed!\n", wavName);
        return -1;
    }
    // set default chn = 1
    int pcmLen = wavDuration * wavSR * wavChn * 2LL / 1000;
    struct WaveHeader header;
    audio_fix_wavheader(&header, wavSR, wavChn, pcmLen);
    fwrite(&header, 1, sizeof(header), dstFp);

    short *pPcmBuf = malloc(pcmLen);

    audio_generator(pPcmBuf, pcmLen, wavType, wavSR, wavChn, wavPeriod, wavMaxGain);
    fwrite(pPcmBuf, 1, pcmLen, dstFp);
    printf("get %s with sample_rate=%d, channle=%d, duration=%d ms, period=%d ms, gain=%d dB, pcmLen:%d\n", wavName, wavSR, wavChn, wavDuration, wavPeriod, wavMaxGain, pcmLen);

    free(pPcmBuf);
    fclose(dstFp);

    return ret;
}
