#ifndef DTERM_HPP
#define DTERM_HPP

#include "dterm_screen.hpp"
#include "interactive_process.hpp"
#include "dterm_screen_binder.hpp"
#include <string>
#include <vterm.h>

class DTerm
{
    VTerm *_vterm;
    DTermScreen _screen;
    InteractiveProcess *_process;
    Vec2<unsigned int> _size;

public:
    DTerm(Vec2<unsigned int> size);
    ~DTerm();

    DTermScreen *getScreen();
    InteractiveProcess *getProcess();
    void startProcess(std::string command);
    void endProcess();
};

#endif
