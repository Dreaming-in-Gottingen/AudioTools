/*
 *******************************************************
 * Author       : jingzhou.zhang
 * Last modified: 2019-08-24 17:30
 * Email        : zjz1988314@gmail.com
 * Filename     : wav_ops.h
 * Description  : 
 ********************************************************
 */

#ifndef _AUDIO_TOOLS_WAV_OPS_H_
#define _AUDIO_TOOLS_WAV_OPS_H_

#ifdef __cplusplus

#include <stdint.h>
#include <cstddef>
#include <string>

namespace audiotools {

// writing 16-bit PCM WAV files
class WavWriter {
public:
    WavWriter(const std::string& file_name, int sample_rate, int channel_cnt);
    ~WavWriter();

    // Each sample is in the range [-32768,32767] for S16_LE, and interleaved channels.
    // return num_samples is ok
    ssize_t WriteSamples(const int16_t* samples, size_t num_samples);

    uint32_t sample_rate() const { return sample_rate_; }
    uint32_t channel_cnt() const { return channel_cnt_; }
    uint32_t num_samples() const { return num_samples_; }

private:
    void Close_file();
    const uint32_t sample_rate_;
    const uint32_t channel_cnt_;
    uint32_t num_samples_;  // Total number of samples written to file.
    FILE* file_handle_;
};

class WavReader {
public:
    explicit WavReader(const std::string& file_name);
    ~WavReader();

    // Returns the number of samples read. If this is less than requested,
    // verifies that the end of the file was reached.
    ssize_t ReadSamples(int16_t* sample, size_t num_samples);

    uint32_t sample_rate() const { return sample_rate_; }
    uint32_t channel_cnt() const { return channel_cnt_; }
    uint32_t num_samples() const { return num_samples_; }
    uint32_t num_samples_remaining() const { return num_samples_remaining_; }
    // unit: ms
    uint32_t duration_ms() const { return 1000*num_samples_/sample_rate_;}

private:
    void Close_file();
    uint32_t sample_rate_;
    uint32_t channel_cnt_;
    uint32_t num_samples_;  // Total number of samples in the file.
    uint32_t num_samples_remaining_;
    FILE* file_handle_;
};

}  // namespace audiotools

extern "C" {

#endif  // __cplusplus

// C wrappers for the WavWriter and WavReader class.
// WavWriter/Reader is implemented by c++ in libwavops.so
typedef struct CWavWriter CWavWriter;
CWavWriter* CWavWriterOpen(const char* file_name, int sample_rate, int channel_cnt);
void CWavWriterClose(CWavWriter* wf);
ssize_t CWavWriter_AddSamples(CWavWriter* wf, const int16_t* samples, size_t num_samples);
uint32_t CWavWriter_GetSampleRate(const CWavWriter* wf);
uint32_t CWavWriter_GetChannelCnt(const CWavWriter* wf);
uint32_t CWavWriter_GetNumSamples(const CWavWriter* wf);

typedef struct CWavReader CWavReader;
CWavReader* CWavReaderOpen(const char* file_name);
void CWavReaderClose(CWavReader* rf);
ssize_t CWavReader_GetSamples(CWavReader* rf, int16_t* samples, size_t num_samples);
uint32_t CWavReader_GetSampleRate(const CWavReader* rf);
uint32_t CWavReader_GetChannelCnt(const CWavReader* rf);
uint32_t CWavReader_GetNumSamples(const CWavReader* rf);
uint32_t CWavReader_GetNumSamplesRemaining(const CWavReader* rf);
uint32_t CWavReader_GetDurationMs(const CWavReader* rf);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // _AUDIO_TOOLS_WAV_OPS_H_
