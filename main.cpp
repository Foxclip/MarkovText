#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <string>

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
    std::fstream inputStream("input.txt");
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

int main(int argc, char *argv[]) {

    std::vector<std::string> words = readWords(argv[1]);
    for(std::string word: words) {
        std::cout << word << "\n";
    }
    std::fstream writeStream("write.txt");
    writeStream << "test";
    std::cout << "Words read: " << words.size() << "\n";
    std::cout << "Writing words to file...\n";
    std::ofstream outStream("output.txt");
    for(std::string word: words) {
        outStream << word << "\n";
    }
    std::cout << "Done\n";

    return 0;

}