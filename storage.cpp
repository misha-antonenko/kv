#include <cassert>
#include <fstream>

#include "kv.pb.h"
#include "storage.h"

using namespace std;

TStorage::TStorage(const std::string& index_file_name)
    : index_file_name(index_file_name) {
  index_file.open(index_file_name, std::ios::in | std::ios::out |
                                       std::ios::app | std::ios::binary);
  assert(index_file.is_open());
  cout<<"initialised!"<<endl;
}

std::optional<uint64_t> TStorage::get(const std::string &key) {
  auto it = index.find(key);
  if (it == index.end())
    return std::nullopt;
  return it->second;
}

void TStorage::set(const std::string &key, uint64_t offset) {
  index[key] = offset;

  assert(index_file.is_open());

  int l = key.size();
  index_file.write(reinterpret_cast<char *>(&l), 4);
  index_file.write(key.data(), l);
  index_file.write(reinterpret_cast<char *>(&offset), 8);
  index_file.flush();
}

void TStorage::load() {
  while (1) {
    int l = -17;
    index_file.read(reinterpret_cast<char *>(&l), 4);

    if (!index_file) {
      index_file.clear();
      index_file.seekg(std::ios::end);
      break;
    }

    char k[l + 1];
    k[l] = 0;
    uint64_t v;

    index_file.read(k, l);
    index_file.read(reinterpret_cast<char *>(&v), 8);

    index[k] = v;
  }
}

void TStorage::dump() {
  index_file.close();
  index_file.open(index_file_name,
                  std::ios::out | std::ios::binary | std::ios::trunc);
  for (auto [k, v] : index) {
    uint32_t l = k.size();
    index_file.write(reinterpret_cast<char *>(&l), 4);
    index_file.write(k.data(), l);
    index_file.write(reinterpret_cast<char *>(&v), 8);
  }
  index_file.close();
  index_file.open(index_file_name, std::ios::in | std::ios::out |
                                       std::ios::app | std::ios::binary);
}