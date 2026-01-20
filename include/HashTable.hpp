#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <vector>

template<typename K, typename V>

class HashTable {
private:
    std::vector<list<pair<K, V>>> table; 
    int (*hashFunc)(K); 
    // insert, search, delete with collision handling 
};

#endif // HASHTABLE_HPP