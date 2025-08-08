
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>


constexpr int TOPK = 3;


struct BuildNode {
    std::string edge;                     
    std::vector<int> topk;                
    std::unordered_map<char, BuildNode*> children;
    int wordID = -1;                      
    BuildNode(const std::string& e="") : edge(e), wordID(-1) { topk.reserve(TOPK); }
};

struct WordEntry {
    uint32_t offset;   
    uint32_t length;
    uint64_t freq;
};

class RadixTrie {
public:
    RadixTrie();
    ~RadixTrie();

    
    void buildFromFreqMap(const std::vector<std::pair<std::string, uint64_t>>& freqList);

    
    void insertWord(const std::string& word, uint32_t wordID, uint64_t freq);

    
    void serialize(const std::string& outFile);

private:
    BuildNode* root;
    std::vector<std::string> words;      
    std::vector<WordEntry> wordEntries;  
    void freeNode(BuildNode* n);

    
    void updateTopK(BuildNode* node, uint32_t wordID, uint64_t freq);
};
