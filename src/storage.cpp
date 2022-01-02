#include <cassert>
#include <chrono>
#include <cstdio>
#include <experimental/filesystem>
#include <fstream>
#include <mutex>
#include <thread>

#include "kv.pb.h"
#include "storage.h"

using namespace std;
using namespace experimental::filesystem;

TStorage::TStorage(const string &index_file_name)
    : index_file_name(index_file_name) {

  load();
  
  index_file = fstream(index_file_name, ios::out | ios::app | ios::binary);

  cerr << "initialised!" << endl;

  dumper = thread([&]() { this->dump(); });
}

optional<uint64_t> TStorage::get(const string &key) {
  lock_guard<mutex> index_guard(index_mutex);
  auto i = index.find(key);
  if (i != index.end())
    return i->second;
  return nullopt;
}

void TStorage::set(const string &key, uint64_t value) {
  lock_guard<mutex> index_guard(index_mutex);

  index[key] = value;

  assert(index_file.is_open());
  int l = key.size();
  index_file.write(reinterpret_cast<char *>(&l), 4);
  index_file.write(key.data(), l);
  index_file.write(reinterpret_cast<char *>(&value), 8);
  index_file.flush();
  log_size++;
}

bool TStorage::load_from(string file_name) {
  fstream file(file_name);

  if (not file.good())
    return false;

  while (1) {
    int l;
    file.read(reinterpret_cast<char *>(&l), 4);

    if (!file) {
      file.clear();
      file.seekg(ios::end);
      break;
    }

    char k[l + 1];
    k[l] = 0;
    uint64_t v;

    file.read(k, l);
    file.read(reinterpret_cast<char *>(&v), 8);

    index[k] = v;
  }

  return true;
}

void TStorage::load() {
  load_from(index_file_name);
  auto swap_exists = load_from(index_file_name + ".swap");
  if (swap_exists)
    remove((index_file_name + ".swap").data());
}

void TStorage::dump() {
  while (1) {
    this_thread::sleep_for(chrono::seconds(dump_interval));

    if (log_size < dump_redundancy * index.size())
      continue;

    unordered_map<string, uint64_t> index_copy;

    {
      lock_guard<mutex> guard(index_mutex);
      index_copy = index;
    }

    fstream swap_index_file(index_file_name + ".swap", ios::out | ios::binary | ios::trunc);

    size_t new_log_size = 0;

    for (auto [k, v] : index_copy) {
      uint32_t l = k.size();
      swap_index_file.write(reinterpret_cast<char *>(&l), 4);
      swap_index_file.write(k.data(), l);
      swap_index_file.write(reinterpret_cast<char *>(&v), 8);
      new_log_size++;
    }

    swap_index_file.close();

    {
      lock_guard<mutex> guard(index_mutex);
      index_file.close();
      rename(index_file_name + ".swap", index_file_name);
      index_file.open(index_file_name, ios::out | ios::app | ios::binary);
      assert(index_file.is_open());
      log_size = new_log_size;
    }
    
    cerr << "Dumped" << endl;
  }
}
