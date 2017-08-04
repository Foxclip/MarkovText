#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <string>
#include <memory>

enum State {
    ST_SPACE,
    ST_ALPHANUM,
    ST_CHAR
};

int iscyr(int c) {
    return (c > 191 || c == 168 || c == 184);
}

std::vector<std::string> readWords(std::string inputFilename) {
    std::vector<std::string> words;
    std::fstream inputStream(inputFilename);
    if(!inputStream.is_open()) {
        std::cout << "Error" << std::endl;
    }
    std::string wordBuffer;
    unsigned char c;
    State currentState = ST_SPACE;
    while(inputStream >> std::noskipws >> c) {
        if(std::isspace(c)) {
            if(currentState == ST_ALPHANUM) {
                words.push_back(wordBuffer);
                currentState = ST_SPACE;
            }
            else
            if(currentState == ST_CHAR) {
                currentState = ST_SPACE;
            }
        }
        else
        if(std::isalnum(c) || iscyr(c) || c == '\'') {
            if(currentState == ST_SPACE || currentState == ST_CHAR) {
                wordBuffer.clear();
                wordBuffer += c;
                currentState = ST_ALPHANUM;
            }
            else
            if(currentState == ST_ALPHANUM) {
                wordBuffer += c;
            }
        }
    }
    if(currentState == ST_ALPHANUM) {
        words.push_back(wordBuffer);
        currentState = ST_SPACE;
    }
    return words;
}

struct CharAfter {
    unsigned char c;
    int times;
};

struct CharEntry {
    unsigned char c;
    std::vector<CharAfter> charsAfter;
};

bool charEntryExists(char c, std::vector<CharEntry> chars) {
    for(CharEntry entry: chars) {
        if(entry.c == c) {
            return true;
        }
    }
    return false;
}

std::unique_ptr<CharAfter> findCharEntry(char c, std::vector<CharAfter> chars) {
    for(CharAfter after: chars) {
        if(after.c == c) {
            return std::unique_ptr<CharAfter>(&after);
        }
    }
    return nullptr;
}

std::vector<CharEntry> readChars(std::string inputFilename) {
    std::vector<CharEntry> chars;
    std::fstream inputStream("input.txt");
    if(!inputStream.is_open()) {
        std::cout << "Error" << std::endl;
    }
    unsigned char c;
    while(inputStream >> std::noskipws >> c) {
        if(chars.size() > 0) {
            std::unique_ptr<CharEntry> previousEntry = std::unique_ptr<CharEntry>(&chars[chars.size() - 1]);
            std::unique_ptr<CharAfter> thisChar = findCharEntry(c, previousEntry->charsAfter);
            if(thisChar) {
                thisChar->times++;
            } else {
                previousEntry->charsAfter.push_back({c, 1});
            }
        }
        if(!charEntryExists(c, chars)) {
            chars.push_back({c, {}});
        }
    }
    return chars;
}

int main(int argc, char *argv[]) {

    std::vector<CharEntry> chars = readChars(argv[1]);
    std::ofstream outStream("output.txt");
    for(CharEntry entry: chars) {
        outStream << entry.c << "\n";
        for(CharAfter after: entry.charsAfter) {
            outStream << "    " << after.c << ": " << after.times << "\n";
        }
    }

    return 0;

}