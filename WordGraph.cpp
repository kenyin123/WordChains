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

        std::vector<std::string> result;
        if (search(result))
        {
            std::cout << "Result: " << result << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to open " << _options.fileName << std::endl;
    }
}

bool WordGraph::search(std::vector<std::string>& result)
{
    _sources.clear();
    _maxLen = 0;
    for (auto w : _words)
    {
        if (_options.headChar == '*' || _options.headChar == w[0])
        {
            _sources.insert(w);
        }
    }

    while (!_sources.empty())
    {
        std::vector<std::string> path = { *_sources.begin() };
        _sources.erase(path[0]);
        searchWorker(path, 1, path[0].length());
    }
    if (_maxLen > 0)
    {
        result = _maxPath;
    }
    return (_maxLen > 0);
}

void WordGraph::searchWorker(std::vector<std::string> &path, int c_word, int c_char)
{
    char currentTail = path[c_word - 1].back();
    if (c_word > 1 && (_options.tailChar == '*' || _options.tailChar == currentTail))
    {
        std::vector<std::string> vt(path.begin(), path.begin() + c_word);
        std::cout << "Found a solution: " << vt << std::endl;
        // Found a valid list
        int len = _options.countWord ? c_word : c_char;
        if (len > _maxLen)
        {
            _maxLen = len;
            _maxPath = path;
        }
    }

    if (_connections.count(currentTail))
    {
        const std::vector<std::string>& nextWords = _connections[currentTail];
        for (auto word : nextWords)
        {
            bool foundLoop = false;
            for (int i = 0; i < c_word; i++)
            {
                if (path[i] == word)
                {
                    std::vector<std::string> vt(path.begin(), path.begin() + c_word);
                    std::cout << "find a loop: " << vt << " nextword = " << word << std::endl;
                    foundLoop = true;
                    break;
                }
            }
            if (foundLoop) continue;

            if (_sources.count(word))
            {
                _sources.erase(word);
            }

            if (c_word < path.size())
            {
                path[c_word] = word;
            }
            else
            {
                path.push_back(word);
            }
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
    for (auto s : val)
    {
        out << s << ", ";
    }
    return out;
}
