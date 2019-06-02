#include "pch.h"
#include "WordGraph.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cctype>
#include <stdexcept>

//static function
int Core::gen_chain_word(char* words[], int len, char* result[], char head, char tail, bool enable_loop)
{
    Core core;
    std::vector<std::string> vWords;
    for (int i = 0; i < len; i++)
    {
		std::string temp = words[i];
		std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        vWords.push_back(temp);
    }
    
    WordListOptions options;
    options.headChar = (head == 0) ? '*' : head;
    options.tailChar = (tail == 0) ? '*' : tail;
    options.allowLoop = enable_loop;

    auto res = core.go(vWords, options);
    for (int i = 0; i < (int)res.size(); i++)
    {
        result[i] = const_cast<char *>(res[i].c_str());
    }
    return res.size();
}
int Core::gen_chain_char(char* words[], int len, char* result[], char head, char tail, bool enable_loop)
{
	Core core;
	std::vector<std::string> vWords;
	for (int i = 0; i < len; i++)
	{

		vWords.push_back(words[i]);
	}

	WordListOptions options;
	options.headChar = (head == 0) ? '*' : head;
	options.tailChar = (tail == 0) ? '*' : tail;
	options.allowLoop = enable_loop;
	options.countWord = false;

	auto res = core.go(vWords, options);
	for (int i = 0; i < (int)res.size(); i++)
	{
		result[i] = const_cast<char *>(res[i].c_str());
	}
	return res.size();
}

std::vector<std::string> Core::read_file(const std::string& file_name)
{
    std::ifstream filein(file_name);

    std::vector<std::string> res;
    if (!filein.fail())
    {
        for (std::string line; std::getline(filein, line); )
        {
            int pos = 0;
            bool expectBegin = true;
            int beginPos;
            while (pos < (int)line.length())
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
                        res.push_back(word);
                        expectBegin = true;
                    }
                }
                pos++;
            }
            if (!expectBegin)
            {
                std::string word = line.substr(beginPos, pos - beginPos);
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                res.push_back(word);
            }
        }
    }
    else
    {
        std::cout << "Failed to open " << file_name << std::endl;
    }
    return res;
}


std::vector<std::string> Core::go(const std::vector<std::string> &words, const WordListOptions& op)
{
    _options = op;
    for (auto word : words)
    {
        addWord(word);
    }

    std::vector<std::string> result;
    if (search(result))
    {
        std::cout << "Result: " << result << std::endl;
    }
    return result;
}

bool Core::search(std::vector<std::string>& result)
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
        result = _maxPath;
    }
    return (_maxLen > 0);
}

void Core::searchWorker(std::vector<std::string> &path, int c_word, int c_char)
{
    if ((int)path.size() > c_word)
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
            _maxPath = path;
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

            if ((int)path.size() > c_word)
            {
                path.resize(c_word);
            }
            path.push_back(word);
            searchWorker(path, c_word + 1, c_char + word.length());
        }
    }
}

void Core::addWord(const std::string& word)
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

void Core::dumpWords()
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
    for (int i = 0; i < (int)val.size(); i++)
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
