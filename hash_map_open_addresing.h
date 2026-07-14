#pragma once

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>
#include "equable_and_hashable.h"

enum class NodeState {
  FREE,
  IN_USE,
  ERASED
};

template <EquableAndHashable K, typename V> struct Node {
public:
  K key;
  V value;
  NodeState state {NodeState::FREE};
};

template <EquableAndHashable K, typename V>
class HashMapOpenAddressing {
private:
  constexpr static size_t DEFAULT_CAPACITY = 19, MIN_CAPACITY = 5;
  size_t capacity, size{0}, used_slots{0};
  float load_factor{0.90};
  std::vector<Node<K, V>> table;

  size_t hash(const K &key, size_t capacity, size_t i) const;
  size_t nextPrime(size_t x);
  bool shouldReziseTable();
  void resizeTable(const size_t new_capacity);

public:
  HashMapOpenAddressing();
  HashMapOpenAddressing(size_t capacity);

  std::optional<V> get(const K &key) const;
  void put(K key, V value);
  void remove(const K &key);
  size_t getSize() const {return size;}
  size_t getCapacity() const { return capacity; }

  std::optional<V> operator[](const K &key) const;
};

template <EquableAndHashable K, typename V>
HashMapOpenAddressing<K, V>::HashMapOpenAddressing()
  : HashMapOpenAddressing{DEFAULT_CAPACITY} {}

template <EquableAndHashable K, typename V>
HashMapOpenAddressing<K, V>::HashMapOpenAddressing(size_t capacity)
  : capacity{nextPrime(capacity)}, table{capacity} {
  if(capacity < MIN_CAPACITY) throw std::invalid_argument{"Capacity is too small"};
}

template <EquableAndHashable K, typename V>
size_t HashMapOpenAddressing<K, V>::hash(const K &key, size_t capacity,
                                  size_t i) const {
  size_t h = std::hash<K>{}(key);
  size_t h1 = h % capacity;

  if (i == 0) return h1;

  size_t h2 = 1 + (h % (capacity - 1));
  return (h1 + i * h2) % capacity;
}

template <EquableAndHashable K, typename V>
bool HashMapOpenAddressing<K, V>::shouldReziseTable() {
  return static_cast<float>(used_slots) / capacity >= load_factor;
}

template <EquableAndHashable K, typename V>
void HashMapOpenAddressing<K, V>::resizeTable(const size_t new_capacity) {
  std::vector<Node<K, V>> new_table{new_capacity};

  for (size_t i = 0; i < capacity; i++) {
    if (table[i].state == NodeState::ERASED) {
      continue;
    }

    Node<K, V> &node = table[i];

    for (size_t j = 0; j < new_capacity; j++) {
      size_t bucket = hash(node.key, new_capacity, j);

      if (new_table[bucket].state == NodeState::FREE) {
        new_table[bucket] = node;
        break;
      }
    }
  }

  used_slots = size;
  capacity = new_capacity;
  table = std::move(new_table);
}

template <EquableAndHashable K, typename V>
std::optional<V> HashMapOpenAddressing<K, V>::get(const K &key) const {

  for (size_t i = 0; i < capacity; i++) {
    size_t bucket = hash(key, capacity, i);

    if (table[bucket].state == NodeState::FREE) {
      return std::nullopt;
    }

    if (table[bucket].key == key && table[bucket].state == NodeState::IN_USE) {
      return table[bucket].value;
    }
  }

  return std::nullopt;
}

template <EquableAndHashable K, typename V>
void HashMapOpenAddressing<K, V>::put(K key, V value) {
  if (shouldReziseTable()) {
    resizeTable(nextPrime(capacity * 2));
  }

  for (size_t i = 0; i < capacity; i++) {
    size_t bucket = hash(key, capacity, i);

    Node<K, V> &node = table[bucket];

    if (node.state == NodeState::IN_USE && node.key == key) {
      node.value = value;
      return;
    }

    if (node.state == NodeState::ERASED) {
      node.key = key;
      node.value = value;
      node.state = NodeState::IN_USE;

      size++;
      return;
    }

    if (node.state == NodeState::FREE) {
      node.key = key;
      node.value = value;
      node.state = NodeState::IN_USE;

      size++;
      used_slots++;

      return;
    }
  }

  throw std::runtime_error{"Hash Map is full"};
}

template <EquableAndHashable K, typename V>
void HashMapOpenAddressing<K, V>::remove(const K &key) {
  for (size_t i = 0; i < capacity; i++) {
    size_t bucket = hash(key, capacity, i);

    if (table[bucket].state == NodeState::FREE) {
      return;
    }

    if (table[bucket].key == key) {
      table[bucket].state = NodeState::ERASED;
      size--;
      return;
    }
  }
}

template <EquableAndHashable K, typename V>
std::optional<V> HashMapOpenAddressing<K, V>::operator[](const K &key) const {
  return get(key);
}

template <EquableAndHashable K, typename V>
size_t HashMapOpenAddressing<K, V>::nextPrime(size_t n) {
  auto isPrime = [](size_t x) {
    if (x < 2) return false;
    for (size_t d = 2; d * d <= x; d++) {
      if (x % d == 0) return false;
    }
    return true;
  };
  while (!isPrime(n)) n++;
  return n;
}
