#include "../include/dterm_screen_printer.hpp"

void DTermScreenPrinter::setScreen(DTermScreen *screen) {
    _screen = screen;
}

void DTermScreenPrinter::print() {
    _screen->repaint();
    Vec2<unsigned int> size = _screen->getSize();
    for (int i = 0; i < size.x; i++) {
        printf("%s\n", _screen->getRow(i).c_str());
    }
}

void DTermScreenPrinter::printBordered() {
    _screen->repaint();
    Vec2<unsigned int> size = _screen->getSize();

    printf("┌");
    for (int i = 0; i < size.y; i++) {
        printf("─");
    }
    printf("┐\n");

    for (int i = 0; i < size.x; i++) {
        printf("│%s│\n", _screen->getRow(i).c_str());
    }

    printf("└");
    for (int i = 0; i < size.y; i++) {
        printf("─");
    }
    printf("┘\n");
}
