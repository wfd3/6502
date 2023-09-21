//
// POSIX OS specific functions
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

#include <termio.h>
#include <unistd.h>
#include <cstdio>

void setupSignals() {}

// Make the terminal non-blocking
void enable_raw_mode()
{
	termios term;
	fflush(stdout);
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~(ICANON | ECHO | ISIG);        // Disable echo as well
	// term.c_cc[VMIN] = 1;
	// term.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
	setbuf(stdin, nullptr);
	setbuf(stdout, nullptr);
	tcflush(0, TCIFLUSH);
}

void disable_raw_mode()
{
	termios term;
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag |= ICANON | ECHO | ISIG;
	tcsetattr(0, TCSANOW, &term);
}

bool getch(char& kbdCh) {
	int byteswaiting;
	char ch;

	ioctl(STDIN_FILENO, FIONREAD, &byteswaiting);
	if (byteswaiting == 0)
		return false;

	read(STDIN_FILENO, &ch, 1);
	kbdCh = ch;
	return true;
}

