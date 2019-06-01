#include "WordGraph.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cctype>
#include <stdexcept>

void WordListOptions::dump()
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


void WordGraph::go(const WordListOptions& op)
{
    _options = op;
    std::ifstream filein(_options.fileName);

    if (!filein.fail())
    {
        _words.clear();
        _connections.clear();
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
                        addWord(word);
                        expectBegin = true;
                    }
                }
                pos++;
            }
            if (!expectBegin)
            {
                std::string word = line.substr(beginPos, pos - beginPos);
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                addWord(word);
            }
            //std::cout << line << std::endl;
        }

        std::vector<std::vector<std::string>> result;
        if (search(result))
        {
            for (auto v : result)
            {
                std::cout << "Result: " << v << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Failed to open " << _options.fileName << std::endl;
    }
}

bool WordGraph::search(std::vector<std::vector<std::string>>& result)
{
    _sources.clear();
    _maxLen = 0;
    _maxPaths.clear();
    for (auto w : _words)
    {
        if (_options.headChar == '*' || _options.headChar == w[0])
        {
            _sources.insert(w);
        }
    }

    try
    {
        while (!_sources.empty())
        {
            std::vector<std::string> path = { *_sources.begin() };
            _sources.erase(path[0]);
            searchWorker(path, 1, path[0].length());
        }
    }
    catch (const std::logic_error& le)
    {
        std::cout << le.what() << std::endl;
        return false;
    }

    if (_maxLen > 0)
    {
        result = _maxPaths;
    }
    return (_maxLen > 0);
}

void WordGraph::searchWorker(std::vector<std::string> &path, int c_word, int c_char)
{
    if (path.size() > c_word)
    {
        path.resize(c_word);
    }
    const std::string& tailWord = path.back();
    const char tailChar = tailWord.back();
    //Check if there's a loop
    if (c_word > 1)
    {
        auto it = std::find_if(path.begin(), path.end(), [&](const std::string &w) {
            return (tailChar == w[0] && tailWord != w);
            });
        if (it != path.end() && !_options.allowLoop)
        {
            throw std::logic_error("find a loop.");
        }
    }

    //Check if this is a valid result
    if (c_word > 1 && (_options.tailChar == '*' || _options.tailChar == tailChar))
    {
        std::cout << "----> " << path << std::endl;
        
        // Found a valid list
        int len = _options.countWord ? c_word : c_char;
        if (len > _maxLen)
        {
            _maxLen = len;
            _maxPaths.clear();
            _maxPaths.push_back(path);
        }
        else if (len == _maxLen)
        {
            _maxPaths.push_back(path);
        }
    }

    if (_connections.count(tailChar))
    {
        const std::vector<std::string>& nextWords = _connections[tailChar];
        for (auto word : nextWords)
        {
            if (std::find(path.begin(), path.end(), word) != path.end())
            {
                continue;
            }

            if (_sources.count(word))
            {
                _sources.erase(word);
            }

            if (path.size() > c_word)
            {
                path.resize(c_word);
            }
            path.push_back(word);
            searchWorker(path, c_word + 1, c_char + word.length());
        }
    }
}

void WordGraph::addWord(const std::string& word)
{
    _words.push_back(word);
    if (_connections.count(word[0]))
    {
        _connections[word[0]].push_back(word);
    }
    else
    {
        _connections[word[0]] = { word };
    }
}

void WordGraph::dumpWords()
{
    std::cout << "Dump the list of words" << std::endl;
    for (auto word : _words)
    {
        std::cout << word << std::endl;
    }

    std::cout << "Dump the connections" << std::endl;
    for (const auto& pair : _connections)
    {
        std::cout << "connection[" << pair.first << "]=" << pair.second << std::endl;
    }
}

std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& val)
{
    out << "[";
    for (int i = 0; i < val.size(); i++)
    {
        out << val[i];
        if (i != val.size() - 1)
        {
            out << ", ";
        }
    }
    out << "]";
    return out;
}
