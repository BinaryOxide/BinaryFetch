#ifndef ASCIIART_H
#define ASCIIART_H

#include <string>
#include <vector>

// ---------------- Helper functions for AsciiArt ----------------
std::string stripAnsiSequences(const std::string& s);
std::wstring utf8_to_wstring(const std::string& s);
int char_display_width(wchar_t wc);
size_t visible_width(const std::string& s);
void sanitizeLeadingInvisible(std::string& s);

// ---------------- AsciiArt class ----------------
class AsciiArt {
public:
    AsciiArt();
    bool loadFromFile(const std::string& filename);
    bool isEnabled() const;
    void setEnabled(bool enable);
    void clear();

    // getters for real-time printing
    int getHeight() const { return height; }
    int getMaxWidth() const { return maxWidth; }
    int getSpacing() const { return spacing; }
    const std::string& getLine(int i) const { return artLines[i]; }
    int getLineWidth(int i) const { return (i >= 0 && i < (int)artWidths.size()) ? artWidths[i] : 0; }

private:
    std::vector<std::string> artLines;
    std::vector<int> artWidths;
    int maxWidth;
    int height;
    bool enabled;
    int spacing;
};

// ---------------- LivePrinter (incremental printing) ----------------
class LivePrinter {
public:
    LivePrinter(const AsciiArt& artRef);

    // push one info line; prints the art line at current index (or padding) + spacing + info + newline
    void push(const std::string& infoLine);

    // same as push but for blank info (just prints art line)
    void pushBlank();

    // When no more info lines remain, print leftover art lines
    void finish();

private:
    const AsciiArt& art;
    int index;

    void printArtAndPad();
};

// ---------------- Helper function to push multi-line formatted strings ----------------
void pushFormattedLines(LivePrinter& lp, const std::string& s);

#endif // ASCIIART_H