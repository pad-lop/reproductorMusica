#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>

double getAudioFileDuration(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        return -1.0; // Indicates an error
    }

    // Read the chunk ID
    char chunkID[4];
    file.read(chunkID, 4);

    if (std::string(chunkID, 4) != "RIFF") {
        std::cerr << "Not a valid RIFF file" << std::endl;
        return -1.0; // Indicates an error
    }

    // Read the file size
    uint32_t fileSize;
    file.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));

    // Read the format (should be "WAVE")
    char format[4];
    file.read(format, 4);

    if (std::string(format, 4) != "WAVE") {
        std::cerr << "Not a valid WAVE file" << std::endl;
        return -1.0; // Indicates an error
    }

    // Search for the "fmt " subchunk
    while (true) {
        char subchunkID[4];
        file.read(subchunkID, 4);

        if (std::string(subchunkID, 4) == "fmt ") {
            // Skip the next 4 bytes (subchunk size)
            file.seekg(4, std::ios::cur);

            // Read the audio format (should be 1 for PCM)
            uint16_t audioFormat;
            file.read(reinterpret_cast<char*>(&audioFormat), sizeof(audioFormat));

            if (audioFormat != 1) {
                std::cerr << "Unsupported audio format" << std::endl;
                return -1.0; // Indicates an error
            }

            // Read the number of channels (1 for mono, 2 for stereo)
            uint16_t numChannels;
            file.read(reinterpret_cast<char*>(&numChannels), sizeof(numChannels));

            // Read the sample rate
            uint32_t sampleRate;
            file.read(reinterpret_cast<char*>(&sampleRate), sizeof(sampleRate));

            // Skip the next 6 bytes
            file.seekg(6, std::ios::cur);

            // Read the bits per sample
            uint16_t bitsPerSample;
            file.read(reinterpret_cast<char*>(&bitsPerSample), sizeof(bitsPerSample));

            // Search for the "data" subchunk
            while (true) {
                char subchunkID[4];
                file.read(subchunkID, 4);

                if (std::string(subchunkID, 4) == "data") {
                    // Skip the next 4 bytes (subchunk size)
                    file.seekg(4, std::ios::cur);

                    // Calculate the duration based on file size
                    double duration = static_cast<double>(fileSize - 44) / (numChannels * bitsPerSample / 8.0) / sampleRate;

                    // Close the file and return the duration
                    file.close();
                    return duration;
                } else {
                    // Skip the subchunk size
                    uint32_t subchunkSize;
                    file.read(reinterpret_cast<char*>(&subchunkSize), sizeof(subchunkSize));
                    file.seekg(subchunkSize, std::ios::cur);
                }
            }
        } else {
            // Skip the subchunk size
            uint32_t subchunkSize;
            file.read(reinterpret_cast<char*>(&subchunkSize), sizeof(subchunkSize));
            file.seekg(subchunkSize, std::ios::cur);
        }
    }
}

int main() {
    const std::string filename = "C:/Users/Luisf/Desktop/Estructuras de Datos/Proyecto Modulo 3/Musica/Cage The Elephant - Cold Cold Cold.wav";

    double duration = getAudioFileDuration(filename);

    if (duration >= 0.0) {
        std::cout << "Duration of " << filename << ": " << duration << " seconds" << std::endl;
    }

    return 0;
}
