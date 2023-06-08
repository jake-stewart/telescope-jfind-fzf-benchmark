#include "../include/util.hpp"

namespace fs = std::filesystem;

std::string convertUnicodeToUTF8(uint32_t glyph) {
    std::string result;

    if (glyph <= 0x7F) {
        result += static_cast<char>(glyph);
    }
    else if (glyph <= 0x7FF) {
        result += static_cast<char>(0xC0 | ((glyph >> 6) & 0x1F));
        result += static_cast<char>(0x80 | (glyph & 0x3F));
    }
    else if (glyph <= 0xFFFF) {
        result += static_cast<char>(0xE0 | ((glyph >> 12) & 0x0F));
        result += static_cast<char>(0x80 | ((glyph >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (glyph & 0x3F));
    }
    else if (glyph <= 0x10FFFF) {
        result += static_cast<char>(0xF0 | ((glyph >> 18) & 0x07));
        result += static_cast<char>(0x80 | ((glyph >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((glyph >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (glyph & 0x3F));
    }
    else {
        result = "□";
    }

    return result;
}

std::string parseControlEscapes(const std::string input) {
    std::string output;
    bool escape;
    for (int i = 0; i < input.size(); i++) {
        if (escape) {
            escape = false;
            switch (input[i]) {
                case 'e':
                    output.push_back(27);
                    break;
                case 'n':
                    output.push_back(10);
                    break;
                case 'c':
                    output.push_back(3);
                    break;
                case 'd':
                    output.push_back(4);
                    break;
                case 'b':
                    output.push_back(8);
                    break;
                default:
                    output.push_back(input[i]);
                    break;
            }
        }
        else if (input[i] == '\\') {
            escape = true;
        }
        else {
            output.push_back(input[i]);
        }
    }
    return output;
}


std::vector<std::string> split(std::string str, char delim) {
    std::vector<std::string> words;
    std::string word = "";

    for(char c : str) {
        if(c == delim){
           words.push_back(word);
           word = "";
        }
        else {
           word += c;
        }
    }
    if (word.size() > 0) {
        words.push_back(word);
    }
    return words;
}

std::string toLower(std::string s) {
    for (char &c : s) {
        c = tolower(c);
    }
    return s;
}

fs::path expandUserPath(std::string userPath) {
    if (userPath == "~") {
        return getenv("HOME");
    }
    if (userPath.starts_with("~/")) {
        return getenv("HOME") / fs::path(userPath.substr(2));
    }
    return userPath;
}

int mod(int n, int m) {
    return ((n % m) + m) % m;
}

bool isVowel(char c) {
    switch (tolower(c)) {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
            return true;
        default:
            return false;
    }
}

bool isInteger(const char *str) {
    if (!str || !*str) {
        return false;
    }
    if (*str == '-') {
        str++;
    }
    while (*str) {
        if (!isdigit(*str)) {
            return false;
        }
        str++;
    }
    return true;
}
