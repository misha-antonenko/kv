#pragma once

#include <optional>
#include <unordered_map>
#include <string>
#include <fstream>

class TStorage {
    std::unordered_map<std::string, uint64_t> index;
    std::string index_file_name;
    std::fstream index_file;
    
public:
    TStorage(const std::string& index_file_name);
    
    std::optional<uint64_t> get(const std::string& key);
    void set(const std::string& key, uint64_t value);
    
    void dump();
    void load();
};