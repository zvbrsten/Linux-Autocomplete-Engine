
#include "RadixTrie.h"
#include <algorithm>
#include <fstream>
#include <cstring>
#include <iostream>
#include <cstdint>  


RadixTrie::RadixTrie() { root = new BuildNode(""); }
RadixTrie::~RadixTrie() { freeNode(root); }

void RadixTrie::freeNode(BuildNode* n) {
    if (!n) return;
    for (auto &p: n->children) freeNode(p.second);
    delete n;
}

void RadixTrie::updateTopK(BuildNode* node, uint32_t wordID, uint64_t freq) {
    
    auto &top = node->topk;
    
    auto it = std::find(top.begin(), top.end(), (int)wordID);
    if (it != top.end()) top.erase(it);

    
    bool placed = false;
    for (auto itr = top.begin(); itr != top.end(); ++itr) {
        uint32_t existing = *itr;
        if (freq > wordEntries[existing].freq ||
            (freq == wordEntries[existing].freq && words[wordID] < words[existing])) {
            top.insert(itr, wordID);
            placed = true;
            break;
        }
    }
    if (!placed) top.push_back(wordID);
    if ((int)top.size() > TOPK) top.resize(TOPK);
}

void RadixTrie::insertWord(const std::string& word, uint32_t wordID, uint64_t freq) {
    BuildNode* node = root;
    size_t pos = 0;

    while (pos < word.size()) {
        char c = word[pos];
        if (node->children.find(c) == node->children.end()) {
            
            BuildNode* child = new BuildNode(word.substr(pos));
            node->children[c] = child;
            node = child;
            
            updateTopK(node, wordID, freq);
            break;
        }
        BuildNode* child = node->children[c];
        const std::string &label = child->edge;
        size_t i = 0;
        
        while (pos + i < word.size() && i < label.size() && word[pos + i] == label[i]) i++;
        if (i == label.size()) {
            
            node = child;
            pos += i;
            updateTopK(node, wordID, freq);
        } else {
            
            BuildNode* split = new BuildNode(label.substr(0, i));
            
            child->edge = label.substr(i);
            split->children[child->edge[0]] = child;
            
            node->children[c] = split;
            
            if (pos + i < word.size()) {
                BuildNode* newChild = new BuildNode(word.substr(pos + i));
                split->children[word[pos + i]] = newChild;
                updateTopK(newChild, wordID, freq);
            } else {
                
                split->wordID = wordID;
            }
            
            updateTopK(split, wordID, freq);
            break;
        }
    }
    if (pos == word.size()) {
        
        node->wordID = wordID;
    }
}

void RadixTrie::buildFromFreqMap(const std::vector<std::pair<std::string, uint64_t>>& freqList) {
    
    words.reserve(freqList.size());
    wordEntries.reserve(freqList.size());
    for (size_t i=0;i<freqList.size();++i) {
        words.push_back(freqList[i].first);
        WordEntry we{};
        we.offset = 0; we.length = (uint32_t)freqList[i].first.size(); we.freq = freqList[i].second;
        wordEntries.push_back(we);
    }
    for (uint32_t id=0; id<words.size(); ++id) {
        insertWord(words[id], id, wordEntries[id].freq);
    }
}



void RadixTrie::serialize(const std::string& outFile) {
    
    
    
    std::ofstream os(outFile, std::ios::binary);
    
    uint64_t W = words.size();
    os.write((char*)&W, sizeof(W));
    
    for (uint64_t i=0;i<W;++i) {
        uint32_t L = (uint32_t)words[i].size();
        os.write((char*)&L, sizeof(L));
        os.write(words[i].data(), L);
        uint64_t f = wordEntries[i].freq;
        os.write((char*)&f, sizeof(f));
    }
    
    os.close();
    std::cerr << "Serialized (words-only demo) to " << outFile << "\n";
}
