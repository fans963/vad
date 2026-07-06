#include "WaveReader.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <algorithm>

WaveReader::WaveReader() : sampleRate(0), channels(0) {}

const std::vector<double>& WaveReader::getAudioData() const {
    return audioData;
}

uint32_t WaveReader::getSampleRate() const {
    return sampleRate;
}

std::vector<double> WaveReader::getCroppedData(size_t startSample, size_t endSample) const {
    if (audioData.empty() || startSample >= audioData.size()) {
        return {};
    }
    endSample = std::min(endSample, audioData.size());
    if (startSample >= endSample) {
        return {};
    }
    return std::vector<double>(audioData.begin() + startSample, audioData.begin() + endSample);
}

bool WaveReader::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return false;
    }

    WaveHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(WaveHeader));
    
    if (std::strncmp(header.riff, "RIFF", 4) != 0 || std::strncmp(header.wave, "WAVE", 4) != 0) {
        std::cerr << "Not a valid WAV file." << std::endl;
        return false;
    }

    // Sometimes there are extra chunks before 'data', so we need to search for 'data'
    // but for simplicity, assuming standard 44-byte header if data_chunk_header matches 'data'
    uint32_t data_size = header.data_size;
    if (std::strncmp(header.data_chunk_header, "data", 4) != 0) {
        // Fallback: search for 'data' chunk
        char chunkId[4];
        uint32_t chunkSize;
        bool foundData = false;
        
        file.seekg(20 + header.length_of_fmt, std::ios::beg); // skip format chunk
        while (file.read(chunkId, 4) && file.read(reinterpret_cast<char*>(&chunkSize), 4)) {
            if (std::strncmp(chunkId, "data", 4) == 0) {
                foundData = true;
                data_size = chunkSize;
                break;
            }
            file.seekg(chunkSize, std::ios::cur); // skip other chunks
        }
        
        if (!foundData) {
            std::cerr << "Could not find 'data' chunk." << std::endl;
            return false;
        }
    }

    sampleRate = header.sample_rate;
    channels = header.channels;
    
    int bytesPerSample = header.bits_per_sample / 8;
    int numSamples = data_size / bytesPerSample;
    
    audioData.clear();
    audioData.reserve(numSamples);
    
    for (int i = 0; i < numSamples; ++i) {
        if (bytesPerSample == 2) {
            int16_t sample;
            file.read(reinterpret_cast<char*>(&sample), sizeof(int16_t));
            audioData.push_back(static_cast<double>(sample) / 32768.0);
        } else if (bytesPerSample == 1) {
            uint8_t sample;
            file.read(reinterpret_cast<char*>(&sample), sizeof(uint8_t));
            audioData.push_back((static_cast<double>(sample) - 128.0) / 128.0);
        }
    }
    
    return true;
}
