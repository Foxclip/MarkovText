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

std::ostream& operator << (std::ostream& stream, const ustring& str) {
    if(const auto len = str.size()) {
        stream.write(reinterpret_cast<const char*>(&str[0]), len);
    }
    return stream;
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

struct EntryAfter {
    ustring c;
    int times;
    double probability;
};

struct TextEntry {
    ustring c;
    std::vector<EntryAfter*> listAfter;
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

std::vector<TextEntry*> readChars(std::string inputFilename, int span) {
    std::vector<TextEntry*> entries;
    std::fstream inputStream(inputFilename);
    if(!inputStream.is_open()) {
        std::cout << "Error" << std::endl;
    }
    TextEntry *previousEntry = nullptr;
    ustring charBuffer;
    unsigned char c;
    while(inputStream >> std::noskipws >> c) {
        ustring cstr = charToUstring(c);
        if(charBuffer.size() >= span) {
            TextEntry *baseEntry = findTextEntry(charBuffer, entries);
            if(!baseEntry) {
                TextEntry *newTextEntry = new TextEntry();
                *newTextEntry = {charBuffer, {}};
                entries.push_back(newTextEntry);
                baseEntry = newTextEntry;
            }
            EntryAfter *entryAfter = findEntryAfter(cstr, baseEntry->listAfter);
            if(entryAfter) {
                entryAfter->times++;
            } else {
                EntryAfter *newEntryAfter = new EntryAfter();
                *newEntryAfter = {cstr, 1};
                baseEntry->listAfter.push_back(newEntryAfter);
            }
        }
        charBuffer += c;
        if(charBuffer.size() > span) {
            charBuffer = charBuffer.substr(1, span);
        }
    }
    return entries;
}

void calculateProbabilities(std::vector<TextEntry*> chars) {
    for(TextEntry *entry: chars) {
        long sum = 0;
        for(EntryAfter *after: entry->listAfter) {
            sum += after->times;
        }
        for(EntryAfter *after: entry->listAfter) {
            after->probability = (double)after->times / sum;
        }
    }
}

void generateText(std::string outputFilename, std::vector<TextEntry*> entries, int span, int textLength) {
    TextEntry *currentEntry = entries[0];
    std::ofstream clearOutputFile(outputFilename);
    clearOutputFile.close();
    for(int i = 0; i < textLength; i++) {
        if(!currentEntry) {
            currentEntry = entries[utils::randomBetween(0, entries.size())];
        }
        std::vector<double> weights;
        for(EntryAfter *after: currentEntry->listAfter) {
            weights.push_back(after->probability);
        }
        int nextCharIndex = utils::weightedRandom(weights);
        ustring c = currentEntry->listAfter[nextCharIndex]->c;
        std::ofstream outStream(outputFilename, std::ios_base::app);
        outStream << c;
        outStream.close();
        currentEntry = findTextEntry((currentEntry->c + c).substr(1, span), entries);
    }
}

int main(int argc, char *argv[]) {

    setlocale(LC_CTYPE, "Russian");

    int span = 4;
    std::cout << "Reading chars..." << "\n";
    std::vector<TextEntry*> entries = readChars(argv[1], span);
    std::cout << "Calculating probabilities..." << "\n";
    calculateProbabilities(entries);
    std::cout << "Generating text..." << "\n";
    generateText("text.txt", entries, span, 10000);

    return 0;

}