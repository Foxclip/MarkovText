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
    double probability;
};

struct CharEntry {
    unsigned char c;
    std::vector<CharAfter*> charsAfter;
};

CharAfter *findCharAfter(unsigned char c, std::vector<CharAfter*> chars) {
    for(CharAfter *after: chars) {
        if(after->c == c) {
            return after;
        }
    }
    return nullptr;
}

CharEntry *findCharEntry(unsigned char c, std::vector<CharEntry*> chars) {
    for(CharEntry *entry: chars) {
        if(entry->c == c) {
            return entry;
        }
    }
    return nullptr;
}

std::vector<CharEntry*> readChars(std::string inputFilename) {
    std::vector<CharEntry*> chars;
    std::fstream inputStream(inputFilename);
    if(!inputStream.is_open()) {
        std::cout << "Error" << std::endl;
    }
    CharEntry *previousEntry = nullptr;
    unsigned char c;
    while(inputStream >> std::noskipws >> c) {
        if(chars.size() > 0) {
            CharAfter *thisChar = findCharAfter(c, previousEntry->charsAfter);
            if(thisChar) {
                thisChar->times++;
            } else {
                CharAfter *after = new CharAfter();
                *after = {c, 1};
                previousEntry->charsAfter.push_back(after);
            }
        }
        CharEntry *currentEntry = findCharEntry(c, chars);
        if(!currentEntry) {
            CharEntry *newEntry = new CharEntry();
            *newEntry = {c, {}};
            chars.push_back(newEntry);
            currentEntry = newEntry;
        }
        previousEntry = currentEntry;
    }
    return chars;
}

void calculateProbabilities(std::vector<CharEntry*> chars) {
    for(CharEntry *entry: chars) {
        long sum = 0;
        for(CharAfter *after: entry->charsAfter) {
            sum += after->times;
        }
        for(CharAfter *after: entry->charsAfter) {
            after->probability = (double)after->times / sum;
        }
    }
}

void generateText(std::string outputFilename, std::vector<CharEntry*> chars, int length) {
    std::ofstream outStream(outputFilename);
    CharEntry *currentChar = chars[0];
    for(int i = 0; i < length; i++) {
        std::vector<double> weights;
        for(CharAfter *after: currentChar->charsAfter) {
            weights.push_back(after->probability);
        }
        int nextCharIndex = utils::weightedRandom(weights);
        unsigned char c = currentChar->charsAfter[nextCharIndex]->c;
        outStream << c;
        currentChar = findCharEntry(c, chars);
    }
}

int main(int argc, char *argv[]) {

    std::vector<CharEntry*> chars = readChars(argv[1]);
    calculateProbabilities(chars);
    std::ofstream outStream("output.txt");

    for(CharEntry *entry: chars) {
        std::string outputChar1;
        entry->c == 10 ? outputChar1 = "\n" : outputChar1 += entry->c;
        outStream << "symbol: (" << std::to_string((int)entry->c) << "): " << outputChar1 << "\n";
        for(CharAfter *after: entry->charsAfter) {
            std::string outputChar2;
            after->c == 10 ? outputChar2 = "\n" : outputChar2 += after->c;
            outStream << "    (" << std::to_string((int)after->c) << ") " << outputChar2 << ": " << after->probability*100 << "%" << "\n";
        }
    }

    generateText("text.txt", chars, 10000);

    return 0;

}