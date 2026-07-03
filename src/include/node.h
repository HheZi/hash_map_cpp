#pragma once

#include "assignable_and_hashable.h"
#include <memory>

template <EquableAndHashable K, typename V> class Node {
public:
  const K &getKey() { return key; }
  void setKey(K key) {this->key = key;};

  V &getValue() { return value; }
  void setValue(V value) {this->value = value;};

  void setNext(K key, V value) {
    next = std::make_unique<Node<K, V>>(key, value);
  }
  void setNext(std::unique_ptr<Node<K, V>> node) { next = std::move(node); }
  Node<K, V> *getNext() const { return next.get(); }
  std::unique_ptr<Node<K, V>> releaseNext() { return std::move(next); }

  Node() = delete;
  Node(const Node<K, V> &node) = delete;
  Node(Node<K, V> &&node)
      : key{std::move(node.key)}, value{std::move(node.value)},
        next{std::move(node.next)} {}
  Node(const K key, const V value) : key{key}, value{value} {}

  Node<K, V> &operator=(const Node<K, V> &node) = delete;
  Node<K, V> &operator=(Node<K, V> &&node) {
    key = std::move(node.key);
    value = std::move(node.value);
    next = std::move(node.next);

    return *this;
  }

private:
  K key;
  V value;
  std::unique_ptr<Node<K, V>> next;
};
