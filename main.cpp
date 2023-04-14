#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <utility>
#include <algorithm>

using namespace std;

class CacheEntry {
public:
    int index;
    int tag;
    int counter;

    CacheEntry(int i, int t, int c) : index(i), tag(t), counter(c) {}
};

void simulateDirectMapped(vector<CacheEntry>& cache, unsigned int address, int blockSize, int numBlocks, int& hits) {
    int indexSize = log2(numBlocks);
    int blockOffsetSize = log2(blockSize);
    int tagSize = 32 - indexSize - blockOffsetSize;

    unsigned int index = (address >> blockOffsetSize) & ((1 << indexSize) - 1);
    unsigned int tag = address / blockSize; // using the decimal equivalent of the address as tag

    CacheEntry& entry = cache[index];
    if (entry.tag == tag) {
        // Hit
        entry.counter++;
        hits++;
    } else {
        // Miss
        entry.tag = tag;
        entry.counter = 1;
    }
}

void simulateSetAssociative(vector<vector<CacheEntry>>& cache, unsigned int address, int blockSize, int numBlocks, int associativity, int& hits) {
    int indexSize = log2(numBlocks / associativity);
    int blockOffsetSize = log2(blockSize);
    int tagSize = 32 - indexSize - blockOffsetSize;

    unsigned int index = (address >> blockOffsetSize) & ((1 << indexSize) - 1);
    unsigned int tag = address / blockSize; // using the decimal equivalent of the address as tag

    vector<CacheEntry>& set = cache[index];
    auto entry = find_if(set.begin(), set.end(), [tag](const CacheEntry& e) { return e.tag == tag; });
    if (entry != set.end()) {
        // Hit
        entry->counter++;
        hits++;
        for (auto it = set.begin(); it != set.end(); ++it) {
            if (it->counter < entry->counter) {
                entry->counter = it->counter + 1;
            }
        }
    } else {
        // Miss
        auto victim = min_element(set.begin(), set.end(), [](const CacheEntry& e1, const CacheEntry& e2) { return e1.counter < e2.counter; });
        victim->tag = tag;
        victim->counter = 1;
    }
}

int main() {
int cacheSize = 512; // cache size in bytes
int blockSize = 8; // cache block size in bytes
int directMappedNumBlocks = cacheSize / blockSize;
int setAssociativeNumBlocks = cacheSize / blockSize;
int associativity = 2; // set-associative cache with 2 ways
vector<CacheEntry> directMappedCache(directMappedNumBlocks, CacheEntry(0, 0, 0));
vector<vector<CacheEntry>> setAssociativeCache(setAssociativeNumBlocks / associativity, vector<CacheEntry>(associativity, CacheEntry(0, 0, 0)));

int directMappedHits = 0;
int setAssociativeHits = 0;
int accesses = 0;

// Open the trace file for reading
ifstream traceFile("swim.trace");
if (!traceFile.is_open()) {
    cerr << "Failed to open trace file." << endl;
    return 1;
}

string line;
while (getline(traceFile, line)) {
    // Parse the address from the trace file
    istringstream iss(line);
    char op;
    unsigned int address;
    int size;
    iss >> op >> hex >> address >> size;

    // Ignore the operation and size, and simulate the access to the caches
    simulateDirectMapped(directMappedCache, address, blockSize, directMappedNumBlocks, directMappedHits);
    simulateSetAssociative(setAssociativeCache, address, blockSize, setAssociativeNumBlocks, associativity, setAssociativeHits);
    accesses++;
}

// Output the hit rate and access count for each cache
double directMappedHitRate = (double)directMappedHits / accesses;
double setAssociativeHitRate = (double)setAssociativeHits / accesses;
cout << "Direct-mapped cache hit rate: " << directMappedHitRate << endl;
cout << "Set-associative cache hit rate: " << setAssociativeHitRate << endl;
cout << "Access count: " << accesses << endl;

return 0;
}
