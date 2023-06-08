#ifndef INTERACTIVE_PROCESS_HPP
#define INTERACTIVE_PROCESS_HPP

#include "vec2.hpp"
#include <string>
#include <thread>
#include <vector>

enum class ProcessState {
    Inactive,
    Active,
    Error,
};

class InteractiveProcess
{
    int _master;
    ProcessState *childState;
    std::thread _reader;
    std::function<void(char *, size_t)> _readCallback;
    pid_t _pid;
    Vec2<unsigned int> _size;

public:
    InteractiveProcess();
    ProcessState getState();
    void setSize(Vec2<unsigned int> size);
    void setReadCallback(std::function<void(char *, size_t)> callback);
    void start(std::string command);
    void end();
    void sigint();
    void input(std::string str);
    void input(char c);

private:
    void _childProcessStart();
    void _readerThread();
};

#endif
