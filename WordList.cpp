// WordList.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "cxxopts.hpp"

struct WordListOptions
{
    bool countWord;
    bool allowLoop;
    char headChar;
    char tailChar;
    std::string fileName;

    WordListOptions()
    {
        countWord = true;
        allowLoop = false;
        headChar = '*';
        tailChar = '*';
        fileName = "";
    }

    void dump()
    {
        std::cout << "Count by " << (countWord ? "words" : "characters") << std::endl;
        if (headChar != '*')
        {
            std::cout << "Head char: " << headChar << std::endl;
        }
        if (tailChar != '*')
        {
            std::cout << "Tail char: " << tailChar << std::endl;
        }
        std::cout << "File name: " << fileName << std::endl;
    }
};

bool getOptions(int argc, char *argv[], WordListOptions& wlOption)
{
    cxxopts::Options options("WordList", "Count the length of connected word list");
    options.allow_unrecognised_options().add_options()
        ("w,word", "Count the list by words")
        ("c,character", "Count the list by characters")
        ("r,round", "Allow loops in the word list")
        ("h,head", "Head character", cxxopts::value<char>())
        ("t,tail", "Tail character", cxxopts::value<char>())
        ("file", "File name", cxxopts::value<std::string>())
        ("positional",
            "Positional arguments: these are the arguments that are entered "
            "without an option", cxxopts::value<std::vector<std::string>>());
    options.parse_positional({ "file", "positional" });

    auto result = options.parse(argc, argv);
    if (result.count("w") && result.count("c"))
    {
        std::cout << "ERROR: -w conflicts with -c" << std::endl;
        return false;
    }

    if (!result.count("file"))
    {
        std::cout << "ERROR: Missing filename" << std::endl;
        return false;
    }

    wlOption.countWord = !result.count("c");
    wlOption.allowLoop = result.count("r");
    wlOption.headChar = result.count("h") ? result["h"].as<char>() : '*';
    wlOption.tailChar = result.count("t") ? result["t"].as<char>() : '*';
    wlOption.fileName = result["file"].as<std::string>();
    return true;
}

void go(const WordListOptions& option)
{
    std::ifstream filein(option.fileName);
    std::vector<std::string> words;
    if (!filein.fail())
    {
        for (std::string line; std::getline(filein, line); )
        {
            int pos = 0;
            bool expectBegin = true;
            int beginPos;
            while (pos < line.length())
            {
                if (std::isalpha(line[pos]))
                {
                    if (expectBegin)
                    {
                        beginPos = pos;
                        expectBegin = false;
                    }
                }
                else
                {
                    if (!expectBegin)
                    {
                        std::string word = line.substr(beginPos, pos - beginPos);
                        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                        words.push_back(word);
                        expectBegin = true;
                    }
                }
                pos++;
            }
            if (!expectBegin)
            {
                std::string word = line.substr(beginPos, pos - beginPos);
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                words.push_back(word);
            }
            std::cout << line << std::endl;
        }
        for (auto word: words)
        {
            std::cout << word << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to open " << option.fileName << std::endl;
    }
}

int main(int argc, char *argv[])
{
    WordListOptions wlOption;
    if (!getOptions(argc, argv, wlOption))
    {
        std::cout << "Invalid options. Command line syntax:" << std::endl;
        std::cout << "WordList [-w] [-c] [-h <headchar>] [-t <tailchar>] [-r] absolute_path_of_word_list" << std::endl;
        return -1;
    }

    wlOption.dump();
    go(wlOption);
}