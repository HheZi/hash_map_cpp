#pragma once

#include <concepts>
#include <functional>

template <typename T>
concept Hashable = requires(T a) {
  { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept EqualityOperator = requires(T &lhs, T &rhs) {
  {lhs == rhs} -> std::convertible_to<bool>;
};

template <typename T>
concept EquableAndHashable = EqualityOperator<T> && Hashable<T>;
