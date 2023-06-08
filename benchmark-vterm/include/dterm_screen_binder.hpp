#ifndef DTERM_SCREEN_BINDER_HPP
#define DTERM_SCREEN_BINDER_HPP

#include <vterm.h>
#include "dterm_screen.hpp"

class DTermScreenBinder
{
    static VTermScreenCallbacks _callbacks;

public:
    static void bind(VTermScreen *vtermScreen, DTermScreen *screen);

private:
    static int damage(VTermRect rect, void *dts);
    static int moverect(VTermRect dest, VTermRect src, void *dts);
    static int settermprop(VTermProp prop, VTermValue *val, void *dts);
    static int bell(void *dts);
    static int resize(int rows, int cols, void *dts);
    static int sb_pushline(int cols, const VTermScreenCell *cells, void *dts);
    static int sb_popline(int cols, VTermScreenCell *cells, void *dts);
    static int sb_clear(void *dts);
    static int movecursor(
        VTermPos pos, VTermPos oldpos, int visible, void *dts
    );
};

#endif
