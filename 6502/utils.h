//
// Various helper function prototypes
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

#pragma once

#include <string>

std::string stripSpaces(const std::string&);
std::string stripLeadingSpaces(const std::string&);
std::string stripTrailingSpaces(const std::string&);
std::string split(std::string&, const std::string&);
std::string removeDuplicateSpaces(const std::string&);
std::string wrapText(const std::string&, int, int);
bool containsChar(const std::string&, const char);
void upCaseString(std::string&);
bool isHexDigit(const char);
bool isHexNumber(const std::string&);