#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <utility>

using namespace std;

class CacheEntry {
public:
    int index;
    int set;
    int tag;
    int counter;

    CacheEntry(int i, int s, int t, int c) : index(i), set(s), tag(t), counter(c) {}
};

void simulateDirectMapped(vector<CacheEntry>& cache, unsigned int address, int blockSize, int numBlocks) {
    int indexSize = log2(numBlocks);
    int blockOffsetSize = log2(blockSize);
    int tagSize = 32 - indexSize - blockOffsetSize;

    unsigned int index = (address >> blockOffsetSize) & ((1 << indexSize) - 1);
    unsigned int tag = (address >> (indexSize + blockOffsetSize)) & ((1 << tagSize) - 1);

    CacheEntry& entry = cache[index];
    if (entry.tag == tag) {
        // Hit
        entry.counter++;
    } else {
        // Miss
        entry.tag = tag;
        entry.counter = 1;
    }
}

int main() {
    int cacheSize = 512; // cache size in bytes
    int blockSize = 8; // cache block size in bytes
    int associativity = 1; // direct-mapped cache

    int numBlocks = cacheSize / (blockSize * associativity);
    vector<CacheEntry> cache(numBlocks, CacheEntry(0, 0, 0, 0)); // numBlocks blocks, initially empty

    int hits = 0;
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

        // Ignore the operation and size, and simulate the access to the cache
        simulateDirectMapped(cache, address, blockSize, numBlocks);
        accesses++;
        if (cache.back().counter > 1) {
            hits++;
        }
    }

    // Output the hit rate and access count
    double hitRate = (double)hits / accesses;
    cout << "Hit rate: " << hitRate << endl;
    cout << "Access count: " << accesses << endl;

    return 0;
}