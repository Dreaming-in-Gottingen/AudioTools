/*
 *******************************************************
 * Author       : jingzhou.zhang
 * Last modified: 2019-08-24 17:28
 * Email        : zjz1988314@gmail.com
 * Filename     : wav_header.h
 * Description  : 
 ********************************************************
 */

#ifndef _WAV_HEADER_H_
#define _WAV_HEADER_H_

#ifdef __cplusplus
namespace audiotools {
#endif

    enum WavFormat {
        kWavFormatPcm   = 1,  // PCM, each sample of size bytes_per_sample
        kWavFormatALaw  = 6,  // 8-bit ITU-T G.711 A-law
        kWavFormatMuLaw = 7,  // 8-bit ITU-T G.711 mu-law
        kWavFormatADPCM = 0x11,  // 8-bit
        kWavFormatG726 = 0x45,  // 2-5-bit ITU-T G.726
    };

    const uint32_t WAV_ID_RIFF = 0x46464952; /* "RIFF" */
    const uint32_t WAV_ID_WAVE = 0x45564157; /* "WAVE" */
    const uint32_t WAV_ID_FMT = 0x20746d66; /* "fmt " */
    const uint32_t WAV_ID_DATA = 0x61746164; /* "data" */
    const uint32_t WAV_ID_PCM = 0x0001; /* S16_LE */
    const uint32_t WAV_ID_FLOAT_PCM = 0x0003;

	typedef struct WavHeader
	{
        uint32_t RiffHeader;
        uint32_t FileSize;
        uint32_t WaveHeader;
        uint32_t FormatHeader;
        uint32_t FormatSize;
        uint16_t FormatCode;
        uint16_t Channels;
        uint32_t SampleRate;
        uint32_t BytesPerSecond;
        uint16_t BlockAlign;
        uint16_t BitsPerSample;
        uint32_t DataHeader;
        uint32_t DataSize;
	} WavHeader;

#ifdef __cplusplus
}  // namespace audiotools
#endif

#endif // _WAV_HEADER_H_
