#ifndef UTIL_HPP
#define UTIL_HPP

#include <sys/mman.h>
#include <filesystem>
#include <vector>
#include <string>

template <typename T>
T *create_shared_memory(int size) {
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return (T *)mmap(nullptr, sizeof(T), protection, visibility, -1, 0);
}

std::string convertUnicodeToUTF8(uint32_t glyph);
std::string parseControlEscapes(const std::string input);

std::vector<std::string> split(std::string str, char delim);
std::string toLower(std::string s);
std::string& strip(std::string &str);
std::filesystem::path expandUserPath(std::string user_path);
int mod(int n, int m);
bool isVowel(char c);
bool isInteger(const char *str);

#endif
