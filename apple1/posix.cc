#include <termio.h>
#include <unistd.h>
#include <6502.h>

extern CPU cpu;
std::string commandKeyBanner = "^\ is reset, SIGUSR1 enters debugger";

void signalHandler(int signum) {
	switch (signum) {
	case SIGQUIT:		// Reset computer
		fmt::print("\n");
		cpu.setPendingReset();
		break;

	case SIGUSR1:		// Toggle the 6502 emulator's debugger
		cpu.setDebug(true);
		break;

	case SIGINT:
		fmt::print("\nExiting emulator\n");
		exit(0);
	}
}

void setupSignals() {
	// register signal SIGINT and signal handler  
	signal(SIGQUIT, signalHandler); // Reset
	signal(SIGUSR1, signalHandler); // force into debugger
	signal(SIGINT, signalHandler);  // Quit
}

// Make the terminal non-blocking
void enable_raw_mode()
{
	termios term;
	fflush(stdout);
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	tcflush(0, TCIFLUSH);
}

void disable_raw_mode()
{
	termios term;
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag |= ICANON | ECHO;
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

