#include "AsciiArt.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <locale>
#include <codecvt>
#include <cwchar>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

// ---------------- Helper functions for AsciiArt ----------------

// Strip ANSI escape sequences (like "\x1b[31m") from string
std::string stripAnsiSequences(const std::string& s) {
    static const std::regex ansi_re("\x1B\\[[0-9;]*[A-Za-z]");
    return std::regex_replace(s, ansi_re, "");
}

// Convert UTF-8 string to wstring
std::wstring utf8_to_wstring(const std::string& s) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        return conv.from_bytes(s);
    }
    catch (...) {
        std::wstring w;
        w.reserve(s.size());
        for (unsigned char c : s) w.push_back(static_cast<wchar_t>(c));
        return w;
    }
}

// Return displayed width of a wide character
int char_display_width(wchar_t wc) {
#if !defined(_WIN32)
    int w = wcwidth(wc);
    return (w < 0) ? 0 : w;
#else
    if (wc == 0) return 0;
    if (wc < 0x1100) return 1;
    if ((wc >= 0x1100 && wc <= 0x115F) ||
        (wc >= 0x2E80 && wc <= 0xA4CF) ||
        (wc >= 0xAC00 && wc <= 0xD7A3) ||
        (wc >= 0xF900 && wc <= 0xFAFF) ||
        (wc >= 0xFE10 && wc <= 0xFE19) ||
        (wc >= 0xFE30 && wc <= 0xFE6F) ||
        (wc >= 0xFF00 && wc <= 0xFF60) ||
        (wc >= 0x20000 && wc <= 0x2FFFD) ||
        (wc >= 0x30000 && wc <= 0x3FFFD))
        return 2;
    return 1;
#endif
}

// Return visible width of UTF-8 string
size_t visible_width(const std::string& s) {
    const std::string cleaned = stripAnsiSequences(s);
    const std::wstring w = utf8_to_wstring(cleaned);
    size_t width = 0;
    for (size_t i = 0; i < w.size(); ++i) width += static_cast<size_t>(char_display_width(w[i]));
    return width;
}

// ---------------- Sanitize leading invisible characters ----------------
void sanitizeLeadingInvisible(std::string& s) {
    // Remove UTF-8 BOM (EF BB BF)
    if (s.size() >= 3 &&
        (unsigned char)s[0] == 0xEF &&
        (unsigned char)s[1] == 0xBB &&
        (unsigned char)s[2] == 0xBF) {
        s.erase(0, 3);
    }

    // Remove leading zero-width spaces (U+200B -> E2 80 8B)
    while (s.size() >= 3 &&
        (unsigned char)s[0] == 0xE2 &&
        (unsigned char)s[1] == 0x80 &&
        (unsigned char)s[2] == 0x8B) {
        s.erase(0, 3);
    }
}

// ---------------- AsciiArt class implementation ----------------

AsciiArt::AsciiArt() : maxWidth(0), height(0), enabled(true), spacing(2) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

bool AsciiArt::loadFromFile(const std::string& filename) {
    artLines.clear();
    artWidths.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        enabled = false;
        maxWidth = 0;
        height = 0;
        return false;
    }

    std::string line;
    maxWidth = 0;
    bool isFirstLine = true;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        // Only sanitize the first line for BOM / zero-width characters
        if (isFirstLine) {
            sanitizeLeadingInvisible(line);
            isFirstLine = false;
        }

        artLines.push_back(line);
        size_t vlen = visible_width(line);
        artWidths.push_back((int)vlen);
        if (static_cast<int>(vlen) > maxWidth) maxWidth = static_cast<int>(vlen);
    }

    height = static_cast<int>(artLines.size());
    enabled = !artLines.empty();
    return enabled;
}

bool AsciiArt::isEnabled() const {
    return enabled;
}

void AsciiArt::setEnabled(bool enable) {
    enabled = enable;
}

void AsciiArt::clear() {
    artLines.clear();
    artWidths.clear();
    maxWidth = 0;
    height = 0;
}

// ---------------- LivePrinter implementation ----------------

LivePrinter::LivePrinter(const AsciiArt& artRef) : art(artRef), index(0) {}

void LivePrinter::push(const std::string& infoLine) {
    printArtAndPad();
    if (!infoLine.empty()) std::cout << infoLine;
    std::cout << '\n';
    std::cout.flush();
    ++index;
}

void LivePrinter::pushBlank() {
    printArtAndPad();
    std::cout << '\n';
    std::cout.flush();
    ++index;
}

void LivePrinter::finish() {
    while (index < art.getHeight()) {
        printArtAndPad();
        std::cout << '\n';
        ++index;
    }
}

void LivePrinter::printArtAndPad() {
    int artH = art.getHeight();
    int maxW = art.getMaxWidth();
    int spacing = art.getSpacing();

    if (index < artH) {
        const std::string& a = art.getLine(index);
        std::cout << a;
        int curW = art.getLineWidth(index);
        if (curW < maxW) std::cout << std::string(maxW - curW, ' ');
    }
    else {
        // no art line here, print blank area
        if (maxW > 0) std::cout << std::string(maxW, ' ');
    }

    if (spacing > 0) std::cout << std::string(spacing, ' ');
}

// ---------------- Helper function to push multi-line formatted strings ----------------
void pushFormattedLines(LivePrinter& lp, const std::string& s) {
    std::istringstream iss(s);
    std::string line;
    while (std::getline(iss, line)) {
        // remove trailing '\r' if present
        if (!line.empty() && line.back() == '\r') line.pop_back();
        lp.push(line);
    }
}

/*



// AsciiArt.cpp
#include "AsciiArt.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <regex>
#include <locale>
#include <codecvt>
#include <cwchar>

#ifdef _WIN32
#include <windows.h>
#endif

// ---------------- Helper functions ----------------

// Strip ANSI escape sequences (like "\x1b[31m") from string
static std::string stripAnsiSequences(const std::string& s) {
    static const std::regex ansi_re("\x1B\\[[0-9;]*[A-Za-z]");
    return std::regex_replace(s, ansi_re, "");
}

// Convert UTF-8 string to wstring
static std::wstring utf8_to_wstring(const std::string& s) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        return conv.from_bytes(s);
    }
    catch (...) {
        std::wstring w;
        w.reserve(s.size());
        for (unsigned char c : s) w.push_back(static_cast<wchar_t>(c));
        return w;
    }
}

// Return displayed width of a wide character
static int char_display_width(wchar_t wc) {
#if !defined(_WIN32)
    int w = wcwidth(wc);
    return (w < 0) ? 0 : w;
#else
    if (wc == 0) return 0;
    if (wc < 0x1100) return 1;
    if ((wc >= 0x1100 && wc <= 0x115F) ||
        (wc >= 0x2E80 && wc <= 0xA4CF) ||
        (wc >= 0xAC00 && wc <= 0xD7A3) ||
        (wc >= 0xF900 && wc <= 0xFAFF) ||
        (wc >= 0xFE10 && wc <= 0xFE19) ||
        (wc >= 0xFE30 && wc <= 0xFE6F) ||
        (wc >= 0xFF00 && wc <= 0xFF60) ||
        (wc >= 0x20000 && wc <= 0x2FFFD) ||
        (wc >= 0x30000 && wc <= 0x3FFFD))
        return 2;
    return 1;
#endif
}

// Return visible width of UTF-8 string
static size_t visible_width(const std::string& s) {
    const std::string cleaned = stripAnsiSequences(s);
    const std::wstring w = utf8_to_wstring(cleaned);
    size_t width = 0;
    for (size_t i = 0; i < w.size(); ++i) width += static_cast<size_t>(char_display_width(w[i]));
    return width;
}

// ---------------- Sanitize leading invisible characters ----------------
static void sanitizeLeadingInvisible(std::string& s) {
    // Remove UTF-8 BOM (EF BB BF)
    if (s.size() >= 3 &&
        (unsigned char)s[0] == 0xEF &&
        (unsigned char)s[1] == 0xBB &&
        (unsigned char)s[2] == 0xBF) {
        s.erase(0, 3);
    }

    // Remove leading zero-width spaces (U+200B -> E2 80 8B)
    while (s.size() >= 3 &&
        (unsigned char)s[0] == 0xE2 &&
        (unsigned char)s[1] == 0x80 &&
        (unsigned char)s[2] == 0x8B) {
        s.erase(0, 3);
    }
}

// ---------------- AsciiArt ----------------

AsciiArt::AsciiArt() : maxWidth(0), height(0), enabled(true), spacing(2) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

bool AsciiArt::loadFromFile(const std::string& filename) {
    artLines.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        enabled = false;
        maxWidth = 0;
        height = 0;
        return false;
    }

    std::string line;
    maxWidth = 0;
    bool isFirstLine = true;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        // Only sanitize the first line for BOM / zero-width characters
        if (isFirstLine) {
            sanitizeLeadingInvisible(line);
            isFirstLine = false;
        }

        artLines.push_back(line);
        size_t vlen = visible_width(line);
        if (static_cast<int>(vlen) > maxWidth) maxWidth = static_cast<int>(vlen);
    }

    height = static_cast<int>(artLines.size());
    enabled = !artLines.empty();
    return enabled;
}

bool AsciiArt::isEnabled() const {
    return enabled;
}

void AsciiArt::setEnabled(bool enable) {
    enabled = enable;
}

void AsciiArt::clear() {
    artLines.clear();
    maxWidth = 0;
    height = 0;
}

void AsciiArt::printWithArt(std::function<void()> infoPrinter) {
    if (!enabled) {
        infoPrinter();
        return;
    }

    // Capture infoPrinter output
    std::ostringstream oss;
    std::streambuf* oldBuf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());
    infoPrinter();
    std::cout.rdbuf(oldBuf);

    std::istringstream iss(oss.str());
    std::vector<std::string> infoLines;
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        infoLines.push_back(line);
    }

    int artHeight = height;
    int infoHeight = static_cast<int>(infoLines.size());
    int maxHeight = (artHeight > infoHeight) ? artHeight : infoHeight;

    // Compute max visible width for info
    int maxInfoWidth = 0;
    for (size_t i = 0; i < infoLines.size(); ++i) {
        size_t w = visible_width(infoLines[i]);
        if (static_cast<int>(w) > maxInfoWidth) maxInfoWidth = static_cast<int>(w);
    }

    for (int i = 0; i < maxHeight; ++i) {
        // ASCII art
        if (i < artHeight) {
            std::cout << artLines[i];
            size_t curW = visible_width(artLines[i]);
            if (static_cast<int>(curW) < maxWidth)
                std::cout << std::string(maxWidth - curW, ' ');
        }
        else {
            std::cout << std::string(maxWidth, ' ');
        }

        // spacing
        if (spacing > 0) std::cout << std::string(spacing, ' ');

        // info
        if (i < infoHeight) {
            std::cout << infoLines[i];
            size_t curW = visible_width(infoLines[i]);
            if (static_cast<int>(curW) < maxInfoWidth)
                std::cout << std::string(maxInfoWidth - curW, ' ');
        }
        else {
            std::cout << std::string(maxInfoWidth, ' ');
        }

        std::cout << "\n";
    }
}





*/