#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdint>   


int main() {
    std::unordered_map<std::string, uint64_t> freq;
    std::ifstream infile("data/corpus.txt");
    std::string word;
    while (infile >> word) {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        freq[word]++;
    }

    std::vector<std::pair<std::string, uint64_t>> entries(freq.begin(), freq.end());

    std::ofstream os("data/words_blob.dat", std::ios::binary);
    uint64_t W = entries.size();
    os.write((char*)&W, sizeof(W));
    for (auto &e : entries) {
        uint32_t L = e.first.size();
        os.write((char*)&L, sizeof(L));
        os.write(e.first.data(), L);
        os.write((char*)&e.second, sizeof(uint64_t));
    }

    std::cout << "Wrote " << W << " words to data/words_blob.dat\n";
}
