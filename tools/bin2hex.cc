//
// Convert a binary file to a hexadecimal representation suitable for Wozmon
//
// Copyright (C) 2023 Walt Drummond
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#include <cstdint>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <fmt/format.h>

int main(int argc, char* argv[]) {
    std::istream *input;
    std::ostream *output;
    std::ifstream inFile;
    std::ofstream outFile;
    unsigned int startAddress;

    auto openInput  = [&inFile](char *name) {
        inFile.open(name);
        if (!inFile) {
            std::cerr << "Error: Couldn't open input file." << std::endl;
            exit (1);
        }
    };
    auto openOutput = [&outFile](char *name) {
        outFile.open(name);
        if (!outFile) {
            std::cout << "Error: Couldn't open output file '" << name << "'." << std::endl;
            exit(1);
        }
    };

    switch (argc) {
    case 4: // input is file, output is file
        openInput(argv[1]);
        input = &inFile;
       
        openOutput(argv[2]);
        output = &outFile;
       
        sscanf(argv[3], "%x", &startAddress);
        break;
    
    case 3: // input is stdin, output is file
        input = &std::cin;
       
        openOutput(argv[1]);
        output = &outFile;
       
        sscanf(argv[2], "%x", &startAddress);
        break;
    
    case 2: // input is stdin, output is stdout
        input = &std::cin;
       
        output = &std::cout;
       
        sscanf(argv[1], "%x", &startAddress);
        break;
    
    default:
        std::cerr << "Usage: " << argv[0] << " [input_file] [output_file] <start_address>" << std::endl;
        std::cerr << "\tInput may be stdin, outout may be stdout" << std::endl;
        return 1;
    }

    std::vector<uint8_t> buffer(16);
    while (true) {
        input->read(reinterpret_cast<char *>(buffer.data()), buffer.size());
        std::streamsize bytesRead = input->gcount();

        if (bytesRead == 0) {
            break; // Finished reading
        }

        *output << fmt::format("{:04x}: ", startAddress);

        for (std::streamsize i = 0; i < bytesRead; ++i) {
            *output << fmt::format("{:02x} ", static_cast<unsigned char>(buffer[i]));
        }
        *output << "\n";

        startAddress += bytesRead;
    }

    if (inFile.is_open()) 
        inFile.close();
    if (outFile.is_open())
        outFile.close();

    return 0;
}
