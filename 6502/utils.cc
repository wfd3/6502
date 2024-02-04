//
// Various helper functions
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

#include <string>
#include <algorithm>


std::string stripSpaces(const std::string& input) {
	std::string result = input;
	
	result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
	return result;
}

std::string stripLeadingSpaces(const std::string& input) {
	size_t first = input.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) {
        return "";
    }
    return input.substr(first);
}

std::string stripTrailingSpaces(const std::string& input) {
	size_t last = input.find_last_not_of(" \t\n\r\f\v");
    if (last == std::string::npos) {
        return "";
    }
    return input.substr(0, last + 1);

}

std::string split(std::string& input, const std::string& delim) {
    size_t pos = input.find(delim);
    std::string part;

    if (pos == std::string::npos) {
	    part = input;
	    input = "";
    } else {
        part = input.substr(0, pos);
        input = input.substr(pos + delim.length());
    }
    
    return part;
}

std::string wrapText(const std::string& text, int width, int tabLength) {
    std::string result;
    std::string line;
    int lineLength = 0;

    for (const auto& c : text) {
        if (c == ' ' && lineLength >= width - tabLength) {
            result += line + '\n';
            line.clear();
            lineLength = 0;
	        // Add a tab on the new line
            line.append(tabLength, ' ');
            lineLength += tabLength;
        } else if (c == '\t') {
            int spacesToAdd = tabLength - (lineLength % tabLength);
            line.append(spacesToAdd, ' '); // Add spaces for the tab
            lineLength += spacesToAdd;
        } else {
            line += c;
            lineLength++;
        }
    }

    result += line; // Add any remaining text to the result
    return result;
}

bool isHexDigit(const char ch) {
	return std::isxdigit(ch) || ch == 'x' || ch == 'X';
}

bool isHexNumber(const std::string& str) {
    for(const auto& ch : str) {
        if(!isHexDigit(ch))
            return false;
    }
    return true;
}