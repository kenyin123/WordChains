// WordList.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "cxxopts.hpp"
#include "WordGraph.h"

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

int main(int argc, char *argv[])
{
    WordListOptions wlOption;
    if (!getOptions(argc, argv, wlOption))
    {
        std::cout << "Invalid options. Command line syntax:" << std::endl;
        std::cout << "WordList [-w] [-c] [-h <headchar>] [-t <tailchar>] [-r] absolute_path_of_word_list" << std::endl;
        return -1;
    }

    std::cout << "---------------------------- Options ----------------------------" << std::endl;
    wlOption.dump();
    std::cout << "-----------------------------------------------------------------" << std::endl;
    WordGraph wg;
    wg.go(wlOption);
}