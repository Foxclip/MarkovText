#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <string>
#include <memory>
#include "utils.h"

enum State {
    ST_SPACE,
    ST_ALPHANUM,
    ST_CHAR
};

int iscyr(int c) {
    return (c > 191 || c == 168 || c == 184);
}

typedef std::basic_string<unsigned char> ustring;

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

struct EntryAfter {
    ustring c;
    int times;
    double probability;
};

struct TextEntry {
    ustring c;
    std::vector<EntryAfter*> charsAfter;
};

EntryAfter *findEntryAfter(ustring c, std::vector<EntryAfter*> list) {
    for(EntryAfter *after: list) {
        if(after->c == c) {
            return after;
        }
    }
    return nullptr;
}

TextEntry *findTextEntry(ustring c, std::vector<TextEntry*> list) {
    for(TextEntry *entry: list) {
        if(entry->c == c) {
            return entry;
        }
    }
    return nullptr;
}

ustring charToUstring(unsigned char c) {
    ustring str;
    str += c;
    return str;
}

std::vector<TextEntry*> readChars(std::string inputFilename) {
    std::vector<TextEntry*> chars;
    std::fstream inputStream(inputFilename);
    if(!inputStream.is_open()) {
        std::cout << "Error" << std::endl;
    }
    TextEntry *previousEntry = nullptr;
    unsigned char c;
    while(inputStream >> std::noskipws >> c) {
        ustring cstr = charToUstring(c);
        if(chars.size() > 0) {
            EntryAfter *thisChar = findEntryAfter(cstr, previousEntry->charsAfter);
            if(thisChar) {
                thisChar->times++;
            } else {
                EntryAfter *after = new EntryAfter();
                *after = {cstr, 1};
                previousEntry->charsAfter.push_back(after);
            }
        }
        TextEntry *currentEntry = findTextEntry(cstr, chars);
        if(!currentEntry) {
            TextEntry *newEntry = new TextEntry();
            *newEntry = {cstr, {}};
            chars.push_back(newEntry);
            currentEntry = newEntry;
        }
        previousEntry = currentEntry;
    }
    return chars;
}

void calculateProbabilities(std::vector<TextEntry*> chars) {
    for(TextEntry *entry: chars) {
        long sum = 0;
        for(EntryAfter *after: entry->charsAfter) {
            sum += after->times;
        }
        for(EntryAfter *after: entry->charsAfter) {
            after->probability = (double)after->times / sum;
        }
    }
}

void generateText(std::string outputFilename, std::vector<TextEntry*> chars, int length) {
    std::ofstream outStream(outputFilename);
    TextEntry *currentChar = chars[0];
    for(int i = 0; i < length; i++) {
        std::vector<double> weights;
        for(EntryAfter *after: currentChar->charsAfter) {
            weights.push_back(after->probability);
        }
        int nextCharIndex = utils::weightedRandom(weights);
        ustring c = currentChar->charsAfter[nextCharIndex]->c;
        outStream << c[0];
        currentChar = findTextEntry(c, chars);
    }
}

int main(int argc, char *argv[]) {

    std::vector<TextEntry*> chars = readChars(argv[1]);
    calculateProbabilities(chars);
    generateText("text.txt", chars, 10000);

    return 0;

}