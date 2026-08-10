#include <list>
#include <unordered_map>
#include <utility>

class LRUCache {
  int capacity{};
  using list_pair = std::list<std::pair<int, int>>;
  list_pair cache_list;
  std::unordered_map<int, list_pair::iterator> cache_map;

public:
  LRUCache(int capacity) : capacity(capacity) {}

  int get(int key) {
    auto it = cache_map.find(key);
    if (it == cache_map.end())
      return -1;
    cache_list.splice(cache_list.begin(), cache_list, it->second);
    return it->second->second;
  }

  void put(int key, int value) {
    auto it = cache_map.find(key);
    if (it != cache_map.end()) {
      it->second->second = value;
      cache_list.splice(cache_list.begin(), cache_list, it->second);
      return;
    }
    if (cache_list.size() == capacity) {
      int lru_key = cache_list.back().first;
      cache_list.pop_back();
      cache_map.erase(lru_key);
    }
    cache_list.emplace_front(key, value);
    cache_map[key] = cache_list.begin();
  }
};
