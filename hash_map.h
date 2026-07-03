#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>
#include "assignable_and_hashable.h"
#include "node.h"
#include "hash_map.h"

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

template <EquableAndHashable K, typename V>
HashMap<K, V>::HashMap()
  : HashMap{DEFAULT_CAPACITY} {}

template <EquableAndHashable K, typename V>
HashMap<K, V>::HashMap(size_t capacity)
    : table{capacity, nullptr}, capacity{capacity}, size{0} {}

template <EquableAndHashable K, typename V>
HashMap<K, V>::HashMap(float load_factor) : HashMap{DEFAULT_CAPACITY} {
    this->load_factor = load_factor;
    if (load_factor > 1) {
      throw std::invalid_argument("Load factor cannot be more than 1");
    }
}
template <EquableAndHashable K, typename V>
HashMap<K, V>::~HashMap() {
    for (int i = 0; i < table.size(); i++) {
      delete table[i];

      table[i] = nullptr;
    }
  }

template <EquableAndHashable K, typename V>
size_t HashMap<K, V>::getHashKey(const K &key, const size_t capacity) const {
  return std::hash<K>{}(key) % capacity;
}

template <EquableAndHashable K, typename V>
bool HashMap<K, V>::shouldReziseTable() {
  return static_cast<float>(size) / capacity >= load_factor;
}

template <EquableAndHashable K, typename V>
void HashMap<K, V>::resizeTable(const size_t new_capacity) {
  std::vector<Node<K, V> *> new_table{new_capacity, nullptr};

  for (Node<K, V> *node : table) {
    if (node) {
      size_t h = getHashKey(node->getKey(), new_capacity);

      new_table[h] = std::move(node);
    }
  }

  table = std::move(new_table);
  capacity = new_capacity;
}

template <EquableAndHashable K, typename V>
V *HashMap<K, V>::get(const K &key) const {
  size_t h = getHashKey(key, capacity);

  Node<K, V> *node = table[h];

  while (node && !(node->getKey() == key)) {
    node = node->getNext();
  }

  if (node == nullptr) {
    return nullptr;
  }

  return &node->getValue();
}

template <EquableAndHashable K, typename V>
void HashMap<K, V>::put(K key, V value) {
  if (shouldReziseTable()) {
    resizeTable(capacity * 2);
  }

  size_t h = getHashKey(key, capacity);

  Node<K, V> *node = table[h];

  if (!node) {
    table[h] = new Node<K, V>{key, value};
    size++;
    return;
  }

  Node<K, V> *prev = nullptr;
  while (node) {
    if (node->getKey() == key) {
      node->setValue(value);
      return;
    }

    prev = node;
    node = node->getNext();
  }

  prev->setNext(key, value);
  size++;
}

template <EquableAndHashable K, typename V>
void HashMap<K, V>::remove(const K &key) {
  size_t h = getHashKey(key, capacity);
  if (table[h] == nullptr) {
    return;
  }

  if (table[h]->getKey() == key) {
    Node<K, V> *old = table[h];
    table[h] = old->releaseNext().release();
    delete old;
    size--;
    return;
  }

  Node<K, V> *prev = nullptr, *curr = table[h];

  while (curr && !(curr->getKey() == key)) {
    Node<K, V> *next = curr->getNext();
    prev = curr;
    curr = next;
  }

  if (!curr) {
    return;
  }

  prev->setNext(curr->releaseNext());
  size--;
}

template <EquableAndHashable K, typename V>
V *HashMap<K, V>::operator[](const K &key) const {
  return get(key);
}

