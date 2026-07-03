#include "include/hash_map.h"
#include <string>

int main() {
  HashMap<std::string, int> map{};

  map.put("Hello", 0);

  return 0;
}
