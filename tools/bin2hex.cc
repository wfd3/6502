#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <fmt/core.h>
#include <fmt/format.h>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> <start_address>" << std::endl;
        return 1;
    }

    std::ifstream inFile(argv[1], std::ios::binary);
    std::ofstream outFile(argv[2]);
    unsigned int startAddress;
    sscanf(argv[3], "%x", &startAddress);

    if (!inFile) {
        std::cerr << "Error: Couldn't open input file." << std::endl;
        return 1;
    }

    if (!outFile) {
        std::cerr << "Error: Couldn't open or create output file." << std::endl;
        return 1;
    }

    std::vector<char> buffer(16);
    while (true) {
        inFile.read(buffer.data(), buffer.size());
        std::streamsize bytesRead = inFile.gcount();

        if (bytesRead == 0) {
            break; // Finished reading
        }

        outFile << fmt::format("{:04X}: ", startAddress);
        
        for (std::streamsize i = 0; i < bytesRead; ++i) {
            outFile << fmt::format("{:02X} ", static_cast<unsigned char>(buffer[i]));
        }
        outFile << "\n";

        startAddress += bytesRead;
    }

    inFile.close();
    outFile.close();

    return 0;
}
