#ifndef WAVEREADER_H
#define WAVEREADER_H

#include <string>
#include <vector>
#include <cstdint>

struct WaveHeader {
    char riff[4];
    uint32_t overall_size;
    char wave[4];
    char fmt_chunk_marker[4];
    uint32_t length_of_fmt;
    uint16_t format_type;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byterate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data_chunk_header[4];
    uint32_t data_size;
};

class WaveReader {
public:
    WaveReader();
    bool load(const std::string& filename);

    const std::vector<double>& getAudioData() const;
    uint32_t getSampleRate() const;

    // Retrieve a subset of the audio data based on sample indices
    std::vector<double> getCroppedData(size_t startSample, size_t endSample) const;

    uint16_t getChannels() const { return channels; }

private:
    std::vector<double> audioData;
    uint32_t sampleRate;
    uint16_t channels;
};

#endif // WAVEREADER_H
