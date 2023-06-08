#include "../include/interactive_process.hpp"
#include "../include/util.hpp"
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#if defined(__FreeBSD__)
#include <libutil.h>
#include <sys/ioctl.h>
#include <termios.h>
#elif defined(__OpenBSD__) || defined(__NetBSD__) || defined(__APPLE__)
#include <sys/ioctl.h>
#include <util.h>
#else
#include <pty.h>
#endif

InteractiveProcess::InteractiveProcess() {
    childState = create_shared_memory<ProcessState>(1);
    *childState = ProcessState::Inactive;
}

ProcessState InteractiveProcess::getState() {
    return *childState;
}

void InteractiveProcess::setSize(Vec2<unsigned int> size) {
    _size = size;
}

void InteractiveProcess::setReadCallback(std::function<void(char*, size_t)> callback) {
    _readCallback = callback;
}

void InteractiveProcess::start(std::string command) {
    *childState = ProcessState::Active;
    winsize size = {(unsigned short)_size.x, (unsigned short)_size.y, 0, 0};
    _pid = forkpty(&_master, nullptr, nullptr, &size);
    if (_pid) {
        _reader = std::thread(&InteractiveProcess::_readerThread, this);
    }
    else {
        setenv("TERM", "xterm", true);
        char *args[] = {(char*)command.c_str(), nullptr};
        execvp(command.c_str(), args);
        *childState = ProcessState::Error;
        printf("error starting child process\n");
        _exit(1);
    }
}

void InteractiveProcess::end() {
    int status;
    kill(_pid, SIGKILL);
    waitpid(_pid, &status, 0);
    _reader.join();
}

void InteractiveProcess::sigint() {
    kill(_pid, SIGINT);
}

void InteractiveProcess::input(std::string str) {
    write(_master, str.c_str(), str.size());
}

void InteractiveProcess::input(char c) {
    write(_master, &c, 1);
}

void InteractiveProcess::_childProcessStart() {
}

void InteractiveProcess::_readerThread() {
    // linux pty buffer size is 4096
    char buffer[4096];

    while (true) {
        ssize_t bytesRead = read(_master, buffer, sizeof buffer);
        if (bytesRead <= 0) {
            *childState = ProcessState::Error;
            break;
        }
        _readCallback(buffer, bytesRead);
    }
}
