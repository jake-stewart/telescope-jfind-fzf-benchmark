#ifndef DTERM_SCREEN_HPP
#define DTERM_SCREEN_HPP

#include <stdio.h>
#include <vector>
#include <string>
#include <vterm.h>
#include "util.hpp"
#include "vec2.hpp"

class DTermScreen
{
private:
    VTermScreen *_vtermScreen;
    std::vector<std::string> _rows;
    bool _dirty;
    Vec2<unsigned int> _size;

public:
    void clear();
    void setSize(Vec2<unsigned int> size);
    const std::string& getRow(int row);
    Vec2<unsigned int> getSize();
    bool isDirty();
    void setDirty();
    void SetVTermScreen(VTermScreen *vtermScreen);
    void reset();
    void repaint();
    void repaintRow(int row);
    bool contains(int line, std::string& string);
};

#endif
