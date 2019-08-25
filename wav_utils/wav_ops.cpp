/*
 *******************************************************
 * Author       : jingzhou.zhang
 * Last modified: 2019-08-24 17:29
 * Email        : zjz1988314@gmail.com
 * Filename     : wav_ops.cpp
 * Description  : implemetation for class WavReader/Writer
 ********************************************************
 */

//#include <algorithm>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <limits>

#include "include/wav_ops.h"
#include "include/wav_header.h"

namespace audiotools {

#define CHECK(expr)                                                                     \
    if (!(expr))                                                                        \
    {                                                                                   \
        fprintf(stderr, "Error at (%s, %i) with \"%s\" \n", __FILE__, __LINE__, #expr); \
        assert(0);                                                                      \
    }

// only for 16-bit PCM WAV files(S16_LE).
static const WavFormat kWavFormat = kWavFormatPcm;
static const int kBytesPerSample = 2;

// careful: wav file must be standard, cannot include other media info.
void GetWavInfo(FILE* fp, uint32_t* p_sample_rate, uint32_t* p_channel_cnt, uint32_t* p_num_samples)
{
    WavHeader header;
    int ret = fread(&header, sizeof(header), 1, fp);
    CHECK(ret == 1);
	CHECK(header.DataHeader == WAV_ID_DATA);

    *p_sample_rate = header.SampleRate;
    *p_channel_cnt = header.Channels;
    *p_num_samples = header.DataSize/header.Channels/2;
}

WavReader::WavReader(const std::string& file_name)
    : file_handle_(fopen(file_name.c_str(), "rb"))
{
    GetWavInfo(file_handle_, &sample_rate_, &channel_cnt_, &num_samples_);
    num_samples_remaining_ = num_samples_;
}

WavReader::~WavReader()
{
    Close_file();
}

ssize_t WavReader::ReadSamples(int16_t* samples, size_t num_samples)
{
    const ssize_t readSmpCnt = fread(samples, sizeof(*samples)*channel_cnt_, num_samples, file_handle_);
    num_samples_remaining_ -= readSmpCnt;
    return readSmpCnt;
}

void WavReader::Close_file()
{
    fclose(file_handle_);
    file_handle_ = NULL;
}


WavWriter::WavWriter(const std::string& file_name, int sample_rate, int channel_cnt)
    : sample_rate_(sample_rate),
      channel_cnt_(channel_cnt),
      num_samples_(0),
      file_handle_(fopen(file_name.c_str(), "wb"))
{
    CHECK(file_handle_ != NULL);
    WavHeader header;
    memset(&header, 0, sizeof(header));
    fwrite(&header, sizeof(header), 1, file_handle_);
}

WavWriter::~WavWriter()
{
    Close_file();
}

ssize_t WavWriter::WriteSamples(const int16_t* samples, size_t num_samples)
{
    const size_t written_cnt = fwrite(samples, sizeof(*samples)*channel_cnt_, num_samples, file_handle_);
    num_samples_ += static_cast<uint32_t>(written_cnt);
    return written_cnt;
}

void WavWriter::Close_file()
{
    int pcmSz  = num_samples_ * sizeof(int16_t) * channel_cnt_;
    int riffSz = pcmSz + sizeof(WavHeader) - 8;

    fseek(file_handle_, 0, SEEK_SET);
    WavHeader header;
	header.RiffHeader       =   WAV_ID_RIFF;
	header.FileSize         =   riffSz;
	header.WaveHeader       =   WAV_ID_WAVE;
	header.FormatHeader     =   WAV_ID_FMT;
	header.FormatSize       =   16;
	header.FormatCode       =   kWavFormat;
	header.Channels         =   channel_cnt_;
	header.SampleRate       =   sample_rate_;
	header.BytesPerSecond   =   sample_rate_ * sizeof(int16_t) * channel_cnt_;
	header.BlockAlign       =   sizeof(int16_t) * channel_cnt_;
	header.BitsPerSample    =   16;
	header.DataHeader       =   WAV_ID_DATA;
	header.DataSize         =   pcmSz;

    fwrite(&header, sizeof(WavHeader), 1, file_handle_);
    file_handle_ = NULL;
}

}  // namespace audiotools

/******************************************C API***********************************************/
// below are wrappers of WavReader to  get sample_rate/channel_cnt/duration/remaining_samples

// WavWriter
CWavWriter* CWavWriterOpen(const char* file_name, int sample_rate, int channel_cnt)
{
    return reinterpret_cast<CWavWriter*>(new audiotools::WavWriter(file_name, sample_rate, channel_cnt));
}

void CWavWriterClose(CWavWriter* wf)
{
    delete reinterpret_cast<audiotools::WavWriter*>(wf);
}

ssize_t CWavWriter_AddSamples(CWavWriter* wf, const int16_t* samples, size_t num_samples)
{
    return reinterpret_cast<audiotools::WavWriter*>(wf)->WriteSamples(samples, num_samples);
}

uint32_t CWavWriter_GetSampleRate(const CWavWriter* wf)
{
    return reinterpret_cast<const audiotools::WavWriter*>(wf)->sample_rate();
}

uint32_t CWavWriter_GetChannelCnt(const CWavWriter* wf)
{
    return reinterpret_cast<const audiotools::WavWriter*>(wf)->channel_cnt();
}

uint32_t CWavWriter_GetNumsamples(const CWavWriter* wf)
{
    return reinterpret_cast<const audiotools::WavWriter*>(wf)->num_samples();
}

// WavReader
CWavReader* CWavReaderOpen(const char* file_name)
{
    return reinterpret_cast<CWavReader*>(new audiotools::WavReader(file_name));
}

void CWavReaderClose(CWavReader* rf)
{
    delete reinterpret_cast<audiotools::WavReader*>(rf);
}

ssize_t CWavReader_GetSamples(CWavReader* rf, int16_t* samples, size_t num_samples)
{
    reinterpret_cast<audiotools::WavReader*>(rf)->ReadSamples(samples, num_samples);
}

uint32_t CWavReader_GetSampleRate(const CWavReader* rf)
{
    return reinterpret_cast<const audiotools::WavReader*>(rf)->sample_rate();
}

uint32_t CWavReader_GetChannelCnt(const CWavReader* rf)
{
    return reinterpret_cast<const audiotools::WavReader*>(rf)->channel_cnt();
}

uint32_t CWavReader_GetNumSamples(const CWavReader* rf)
{
    return reinterpret_cast<const audiotools::WavReader*>(rf)->num_samples();
}

uint32_t CWavReader_GetNumSamplesRemaining(const CWavReader* rf)
{
    return reinterpret_cast<const audiotools::WavReader*>(rf)->num_samples_remaining();
}

uint32_t CWavReader_GetDurationMs(const CWavReader* rf)
{
    return reinterpret_cast<const audiotools::WavReader*>(rf)->duration_ms();
}

