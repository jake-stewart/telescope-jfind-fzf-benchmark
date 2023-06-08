#include "../include/dterm_screen.hpp"

void DTermScreen::clear() {
    vterm_screen_reset(_vtermScreen, true);
}

void DTermScreen::setSize(Vec2<unsigned int> size) {
    _size = size;
}

const std::string& DTermScreen::getRow(int row) {
    return _rows[row];
}

Vec2<unsigned int> DTermScreen::getSize() {
    return _size;
}

bool DTermScreen::isDirty() {
    return _dirty;
}

void DTermScreen::setDirty() {
    _dirty = true;
}

void DTermScreen::SetVTermScreen(VTermScreen *vtermScreen) {
    _vtermScreen = vtermScreen;
    vterm_screen_enable_altscreen(_vtermScreen, true);
}

void DTermScreen::reset() {
    vterm_screen_reset(_vtermScreen, true);
}

void DTermScreen::repaintRow(int row) {
    int j = 0;
    while (j < _size.y) {
        VTermScreenCell cell;
        vterm_screen_get_cell(_vtermScreen, {row, j}, &cell);
        if (cell.chars[0] == 0) {
            _rows[row].push_back(' ');
            j++;
        }
        else {
            _rows[row] += convertUnicodeToUTF8(cell.chars[0]);
            j += cell.width;
        }
    }
}

void DTermScreen::repaint() {
    if (!_dirty) {
        return;
    }
    _dirty = false;
    _rows.clear();
    for (int i = 0; i < _size.x; i++) {
        _rows.push_back("");
        repaintRow(i);
    }
}

bool DTermScreen::contains(int row, std::string& string) {
    repaintRow(row);
    if (_rows[row].find(string) != std::string::npos) {
        return true;
    }
    return false;
}
