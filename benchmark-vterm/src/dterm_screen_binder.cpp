#include "../include/dterm_screen_binder.hpp"

VTermScreenCallbacks DTermScreenBinder::_callbacks;

void DTermScreenBinder::bind(VTermScreen *vtermScreen, DTermScreen *screen) {
    _callbacks.damage = damage;
    _callbacks.moverect = moverect;
    _callbacks.settermprop = settermprop;
    _callbacks.bell = bell;
    _callbacks.resize = resize;
    _callbacks.sb_pushline = sb_pushline;
    _callbacks.sb_popline = sb_popline;
    _callbacks.sb_clear = sb_clear;
    _callbacks.movecursor = movecursor;
    vterm_screen_set_callbacks(vtermScreen, &_callbacks, screen);
}

int DTermScreenBinder::damage(VTermRect rect, void *dts) {
    ((DTermScreen*)dts)->setDirty();
    return 0;
}

int DTermScreenBinder::moverect(VTermRect dest, VTermRect src, void *dts) {
    ((DTermScreen*)dts)->setDirty();
    return 0;
}

int DTermScreenBinder::settermprop(VTermProp prop, VTermValue *val, void *dts) {
    ((DTermScreen*)dts)->setDirty();
    return 0;
}

int DTermScreenBinder::bell(void *dts) {
    ((DTermScreen*)dts)->setDirty();
    return 0;
}

int DTermScreenBinder::resize(int rows, int cols, void *dts) {
    ((DTermScreen*)dts)->setDirty();
    return 0;
}

int DTermScreenBinder::sb_pushline(
    int cols, const VTermScreenCell *cells, void *dts
) {
    ((DTermScreen*)dts)->setDirty();
    return 0;
}

int DTermScreenBinder::sb_popline(int cols, VTermScreenCell *cells, void *dts) {
    ((DTermScreen*)dts)->setDirty();
    return 0;
}

int DTermScreenBinder::sb_clear(void *dts) {
    ((DTermScreen*)dts)->setDirty();
    return 0;
}

int DTermScreenBinder::movecursor(
    VTermPos pos, VTermPos oldpos, int visible, void *dts
) {
    ((DTermScreen*)dts)->setDirty();
    return 0;
}

