/* =======================================================================
Implement Custom hash tables for: 
o Location Database: Hash table mapping location IDs to coordinates and details 
o Vehicle Registry: Quick vehicle lookup by ID 
o Package Tracking: Track packages by tracking number 
o Customer Database: Store and retrieve customer information
======================================================================= */

#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include "Utils.hpp"
#include <vector>
#include <list>
#include <utility>
#include <cmath>
#include <stdexcept>
#include <string>
#include <iostream>
#include <iomanip>

// ==================== Hash Table Template ====================

template<typename K, typename V>
class HashTable {
private:
    std::vector<std::list<std::pair<K, V>>> table;
    int tableSize;
    int loadCount;
    static const int DEFAULT_SIZE = 101;
    static const float LOAD_FACTOR;

    // Helper to check if resize is needed
    bool needsResize() const {
        return (float)loadCount / tableSize > LOAD_FACTOR;
    }
    
    // Resize hash table when load factor exceeds threshold
    void resize() {
        int newSize = tableSize * 2;
        std::vector<std::list<std::pair<K, V>>> oldTable = table;
        
        table.clear();
        table.resize(newSize);
        tableSize = newSize;
        loadCount = 0;
        
        for (const auto& bucket : oldTable) {
            for (const auto& pair : bucket) {
                insert(pair.first, pair.second);
            }
        }
    }
    
    // Generic hash function
    int hash(const K& key) const {
        return std::hash<K>()(key) % tableSize;
    }

public:
    // Constructor
    HashTable(int size = DEFAULT_SIZE) 
        : tableSize(size), loadCount(0) {
        table.resize(tableSize);
    }
    
    // Destructor
    ~HashTable() = default;
    
    // Insert a key-value pair
    void insert(const K& key, const V& value) {
        if (needsResize()) {
            resize();
        }
        
        int index = hash(key);
        
        // Check if key already exists
        for (auto& pair : table[index]) {
            if (pair.first == key) {
                pair.second = value;  // Update existing value
                return;
            }
        }
        
        // Insert new key-value pair
        table[index].push_back({key, value});
        loadCount++;
    }
    
    // Search for a value by key
    bool find(const K& key, V& value) const {
        int index = hash(key);
        
        for (const auto& pair : table[index]) {
            if (pair.first == key) {
                value = pair.second;
                return true;
            }
        }
        
        return false;
    }
    
    // Delete a key-value pair
    bool remove(const K& key) {
        int index = hash(key);
        
        auto it = table[index].begin();
        while (it != table[index].end()) {
            if (it->first == key) {
                table[index].erase(it);
                loadCount--;
                return true;
            }
            ++it;
        }
        
        return false;
    }
    
    // Check if a key exists
    bool contains(const K& key) const {
        int index = hash(key);
        
        for (const auto& pair : table[index]) {
            if (pair.first == key) {
                return true;
            }
        }
        
        return false;
    }
    
    // Get the current size
    int size() const {
        return loadCount;
    }
    
    // Check if hash table is empty
    bool isEmpty() const {
        return loadCount == 0;
    }
    
    // Clear all entries
    void clear() {
        for (auto& bucket : table) {
            bucket.clear();
        }
        loadCount = 0;
    }
    
    // Get all keys
    std::vector<K> getAllKeys() const {
        std::vector<K> keys;
        for (const auto& bucket : table) {
            for (const auto& pair : bucket) {
                keys.push_back(pair.first);
            }
        }
        return keys;
    }
    
    // Get all values
    std::vector<V> getAllValues() const {
        std::vector<V> values;
        for (const auto& bucket : table) {
            for (const auto& pair : bucket) {
                values.push_back(pair.second);
            }
        }
        return values;
    }
    
    // Get statistics
    void printStatistics() const {
        std::cout << "Hash Table Statistics:\n";
        std::cout << "  Table Size: " << tableSize << "\n";
        std::cout << "  Load Count: " << loadCount << "\n";
        std::cout << "  Load Factor: " << (float)loadCount / tableSize << "\n";
    }
};

// Define static constant
template<typename K, typename V>
const float HashTable<K, V>::LOAD_FACTOR = 0.75f;


#endif // HASHTABLE_HPP