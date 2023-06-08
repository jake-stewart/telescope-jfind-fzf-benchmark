#ifndef DTERM_SCREEN_PRINTER_HPP
#define DTERM_SCREEN_PRINTER_HPP

#include "dterm_screen.hpp"
#include "vec2.hpp"
#include <vterm.h>

class DTermScreenPrinter {
    DTermScreen *_screen;

public:
    void setScreen(DTermScreen *screen);
    void print();
    void printBordered();
};

#endif
