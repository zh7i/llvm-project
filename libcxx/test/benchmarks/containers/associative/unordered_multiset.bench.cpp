//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

#include <unordered_set>

#include "associative_container_benchmarks.h"
#include "benchmark/benchmark.h"

template <class K>
struct support::adapt_operations<std::unordered_multiset<K>> {
  using ValueType = typename std::unordered_multiset<K>::value_type;
  using KeyType   = typename std::unordered_multiset<K>::key_type;
  static ValueType value_from_key(KeyType const& k) { return k; }
  static KeyType key_from_value(ValueType const& value) { return value; }

  using InsertionResult = typename std::unordered_multiset<K>::iterator;
  static auto get_iterator(InsertionResult const& result) { return result; }
};

int main(int argc, char** argv) {
  support::associative_container_benchmarks<std::unordered_multiset<int>>("std::unordered_multiset<int>");

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
