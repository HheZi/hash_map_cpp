#pragma once

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>
#include "equable_and_hashable.h"
#include "node.h"

template <EquableAndHashable K, typename V>
class HashMapChanining {
private:
  constexpr static size_t DEFAULT_CAPACITY = 20;
  constexpr static float A = 0.618033;
  size_t capacity{}, size{};
  float load_factor{0.75};
  std::vector<Node<K, V> *> table;

  size_t getHashKey(const K &key, const size_t capacity) const;
  bool shouldReziseTable();
  void resizeTable(const size_t new_capacity);

public:
  HashMapChanining();
  HashMapChanining(size_t capacity);
  HashMapChanining(float load_factor);
  ~HashMapChanining();

  V *get(const K &key) const;
  void put(K key, V value);
  void remove(const K &key);
  size_t getSize() const {return size;}
  size_t getCapacity() const {return capacity;}

  V *operator[](const K &key) const;
};

template <EquableAndHashable K, typename V>
HashMapChanining<K, V>::HashMapChanining()
  : HashMapChanining{DEFAULT_CAPACITY} {}

template <EquableAndHashable K, typename V>
HashMapChanining<K, V>::HashMapChanining(size_t capacity)
    : table{capacity, nullptr}, capacity{capacity}, size{0} {}

template <EquableAndHashable K, typename V>
HashMapChanining<K, V>::HashMapChanining(float load_factor) : HashMapChanining{DEFAULT_CAPACITY} {
    this->load_factor = load_factor;
    if (load_factor > 1) {
      throw std::invalid_argument("Load factor cannot be more than 1");
    }
}
template <EquableAndHashable K, typename V>
HashMapChanining<K, V>::~HashMapChanining() {
    for (int i = 0; i < table.size(); i++) {
      delete table[i];

      table[i] = nullptr;
    }
  }

template <EquableAndHashable K, typename V>
size_t HashMapChanining<K, V>::getHashKey(const K &key, const size_t capacity) const {
  return std::floor(capacity * (static_cast<size_t>(std::hash<K>{}(key) * A) % 1));
}

template <EquableAndHashable K, typename V>
bool HashMapChanining<K, V>::shouldReziseTable() {
  return static_cast<float>(size) / capacity >= load_factor;
}

template <EquableAndHashable K, typename V>
void HashMapChanining<K, V>::resizeTable(const size_t new_capacity) {
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
V *HashMapChanining<K, V>::get(const K &key) const {
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
void HashMapChanining<K, V>::put(K key, V value) {
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
void HashMapChanining<K, V>::remove(const K &key) {
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
V *HashMapChanining<K, V>::operator[](const K &key) const {
  return get(key);
}

