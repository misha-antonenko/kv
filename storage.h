#pragma once

#include <optional>
#include <unordered_map>
#include <string>
#include <fstream>
#include <thread>
#include <mutex>

class TStorage {
    std::unordered_map<std::string, uint64_t> index;
    std::mutex index_mutex;
    std::string index_file_name;
    std::fstream index_file;
    std::thread dump_thread;
    
public:
    TStorage(const std::string& index_file_name);
    
    std::optional<uint64_t> get(const std::string& key);
    void set(const std::string& key, uint64_t value);
    
    void dump();
    void load();
};