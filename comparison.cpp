#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>  

using namespace std;

// Cache parameters
 int CACHE_SIZE = 1024; // 1KB cache
 int BLOCK_SIZE = 64; // 32-byte blocks
 int NUM_SETS = CACHE_SIZE / (BLOCK_SIZE); // direct-mapped cache, so 1 set per block

// Cache entry structure
struct CacheEntry {
    int index;
    int tag;
    int counter;
};

// Cache statistics
int num_hits = 0;
int num_accesses = 0;

// Convert hex address to binary and extract tag, set, and offset
void parse_address(string address, int& tag, int& set, int& offset) {
    // Convert hex address to binary
    bitset<32> bits(stoul(address, nullptr, 16));

    // Extract tag, set, and offset
    tag = bits.to_ulong() / (BLOCK_SIZE * NUM_SETS);
    set = (bits.to_ulong() / BLOCK_SIZE) % NUM_SETS;
    offset = bits.to_ulong() % BLOCK_SIZE;
}

// Direct-mapped cache simulation
void direct_mapped_cache_sim() {
    // Initialize cache with empty entries
    vector<CacheEntry> cache(NUM_SETS);

    // Open trace file
    ifstream trace_file("gcc.trace");

    // Parse each line in trace file
    string line;
    while (getline(trace_file, line)) {
        // Ignore first character (load vs store)
        line = line.substr(1);

        // Parse address
        int tag, set, offset;
        parse_address(line.substr(0, 8), tag, set, offset);

        // Check cache for hit
        if (cache[set].tag == tag) {
            num_hits++;
            cache[set].counter++;
        }
        else {
            cache[set].tag = tag;
            cache[set].counter = 1;
        }

        num_accesses++;
    }

    // Calculate hit rate
    float hit_rate = (float)num_hits / num_accesses;

    // Print cache statistics
    cout << "Direct-mapped cache statistics:" << endl;
    cout << "Cache size: " << CACHE_SIZE << endl;
    cout << "Block size: " << BLOCK_SIZE << endl;
    cout << "Number of sets: " << NUM_SETS << endl;
    cout << "Number of accesses: " << num_accesses << endl;
    cout << "Number of hits: " << num_hits << endl;
    cout << "Hit rate: " << hit_rate << endl;
}

// Fully associative cache simulation
void fully_associative_cache_sim() {
    // Initialize cache with empty entries
    vector<CacheEntry> cache(CACHE_SIZE);

    // Open trace file
    ifstream trace_file("gcc.trace");

    // Parse each line in trace file
    string line;
    while (getline(trace_file, line)) {
        // Ignore first character (load vs store)
        line = line.substr(1);

        // Parse address
        int tag, set, offset;
        parse_address(line.substr(0, 8), tag, set, offset);

        // Check cache for hit
        bool hit = false;
        int lru_index = 0;
        for (int i = 0; i < CACHE_SIZE; i++) {
            if (cache[i].tag == tag) {
                hit = true;
                cache[i].counter++;
                break;
            }
            if (cache[i].counter < cache[lru_index].counter) {
                lru_index = i;
            }
        }

        if (!hit) {
            cache[lru_index].tag = tag;
            cache[lru_index].counter = num_accesses;
        }

        num_accesses++;
        if (hit) {
            num_hits++;
        }
    }

    // Calculate hit rate
    float hit_rate = (float)num_hits / num_accesses;

    // Print cache statistics
    cout << "Fully associative cache statistics:" << endl;
    cout << "Cache size: " << CACHE_SIZE << endl;
    cout << "Block size: " << BLOCK_SIZE << endl;
    cout << "Number of accesses: " << num_accesses << endl;
    cout << "Number of hits: " << num_hits << endl;
    cout << "Hit rate: " << hit_rate << endl;
}







int main() {
    direct_mapped_cache_sim();
    
    // Reset cache statistics
    num_hits = 0;
    num_accesses = 0;

    // Fully associative cache parameters
    CACHE_SIZE = 1024; // 1KB cache
    BLOCK_SIZE = 64; // 32-byte blocks
    NUM_SETS = 1; // fully associative cache, so only 1 set

    fully_associative_cache_sim();

    return 0;
}