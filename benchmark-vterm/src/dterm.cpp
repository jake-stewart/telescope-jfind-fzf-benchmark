#include "../include/dterm.hpp"

DTerm::DTerm(Vec2<unsigned int> size) {
    _size = size;
    _screen.setSize(_size);
    _vterm = vterm_new(_size.x, _size.y);
    vterm_set_utf8(_vterm, 1);
    VTermScreen *vtermScreen = vterm_obtain_screen(_vterm);
    _screen.SetVTermScreen(vtermScreen);
    DTermScreenBinder::bind(vtermScreen, &_screen);
    _screen.reset();
}

DTerm::~DTerm() {
    endProcess();
    vterm_free(_vterm);
}

DTermScreen *DTerm::getScreen() {
    return &_screen;
}

InteractiveProcess *DTerm::getProcess() {
    return _process;
}

void DTerm::startProcess(std::string command) {
    endProcess();
    _process = new InteractiveProcess();
    _process->setSize(_size);
    _process->setReadCallback([&](char *buffer, size_t size) {
        vterm_input_write(_vterm, buffer, size);
    });

    _process->start(command);
}

void DTerm::endProcess() {
    if (_process != nullptr) {
        _process->end();
        _process = nullptr;
    }
}
