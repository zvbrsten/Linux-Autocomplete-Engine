// MMapTrieLoader.cpp
#include "MMapTrieLoader.h"
#include <fstream>
#include <iostream>
#include <cstdint>  

#include <algorithm>

bool MMapTrieLoader::loadWordsOnly(const std::string& wordsFile) {
    std::ifstream is(wordsFile, std::ios::binary);
    if (!is) return false;
    uint64_t W;
    is.read((char*)&W, sizeof(W));
    words.resize(W);
    freqs.resize(W);
    for (uint64_t i=0;i<W;++i) {
        uint32_t L;
        is.read((char*)&L, sizeof(L));
        words[i].resize(L);
        is.read(&words[i][0], L);
        uint64_t f;
        is.read((char*)&f, sizeof(f));
        freqs[i] = f;
    }
    std::cerr << "Loaded " << W << " words (demo loader)\n";
    return true;
}

std::vector<std::pair<std::string, uint64_t>> MMapTrieLoader::queryPrefix(const std::string& prefix, int k) {
    std::vector<std::pair<uint64_t, size_t>> candidates; // (freq, idx)
    for (size_t i=0;i<words.size(); ++i) {
        if (words[i].size() >= prefix.size() && words[i].compare(0, prefix.size(), prefix) == 0) {
            candidates.push_back({freqs[i], i});
        }
    }
    // partial sort top k
    if (candidates.size() > (size_t)k) {
        std::nth_element(candidates.begin(), candidates.begin()+k, candidates.end(), [](auto &a, auto &b){ return a.first > b.first; });
        candidates.resize(k);
    }
    sort(candidates.begin(), candidates.end(), [](auto &a, auto &b){ return a.first > b.first; });
    std::vector<std::pair<std::string, uint64_t>> out;
    for (auto &c : candidates) out.push_back({words[c.second], c.first});
    return out;
}
