#pragma once

#include <fstream>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>

using namespace std;

class TStorage {
  size_t log_size = 0;
  size_t dump_interval = 1;
  double dump_redundancy = 2;

  unordered_map<string, uint64_t> index;
  fstream index_file;
  string index_file_name;
  mutex index_mutex;
  thread dumper;

public:
  TStorage(const string &index_file_name);

  optional<uint64_t> get(const string &key);
  void set(const string &key, uint64_t value);

  void dump();
  void load();
  bool load_from(string file_name);
};