#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

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

    void dump();
};

class WordGraph
{
private:
    std::vector<std::string> _words;
    std::unordered_map<char, std::vector<std::string> > _connections;
    std::unordered_set<std::string> _sources;
    WordListOptions _options;
    int _maxLen;
    std::vector<std::string> _maxPath;

    void addWord(const std::string& word);
    bool search(std::vector<std::string> &result);
    void searchWorker(std::vector<std::string> &path, int c_word, int c_char);

public:
    WordGraph() : _maxLen(0) {}
    ~WordGraph() {}

    void go(const WordListOptions& op);
    void dumpWords();
};

std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& val);

