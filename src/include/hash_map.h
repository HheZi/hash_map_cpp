#pragma once

#include <vector>
#include "assignable_and_hashable.h"
#include "node.h"

template <EquableAndHashable K, typename V>
class HashMap {
private:
  constexpr static size_t DEFAULT_CAPACITY = 20;
  size_t capacity{}, size{};
  float load_factor{0.75};
  std::vector<Node<K, V> *> table;

  size_t getHashKey(const K &key, const size_t capacity) const;
  bool shouldReziseTable();
  void resizeTable(const size_t new_capacity);

public:
  HashMap();
  HashMap(size_t capacity);
  HashMap(float load_factor);
  ~HashMap();

  V *get(const K &key) const;
  void put(K key, V value);
  void remove(const K &key);
  size_t getSize() const {return size;}
  size_t getCapacity() const {return capacity;}

  V *operator[](const K &key) const;
};
