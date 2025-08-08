
#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <cstdint>






class MMapTrieLoader {
public:
    bool loadWordsOnly(const std::string& wordsFile); 
    std::vector<std::pair<std::string, uint64_t>> queryPrefix(const std::string& prefix, int k);
private:
    std::vector<std::string> words;
    std::vector<uint64_t> freqs;
};
