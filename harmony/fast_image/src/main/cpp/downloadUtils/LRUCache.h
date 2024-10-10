#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <unordered_map>
#include <list>
#include <mutex>

template <typename KeyType, typename ValueType, int defaultCapacity = 10>
class LRUCache {
 private:
  std::unordered_map<KeyType, std::pair<ValueType, typename std::list<KeyType>::iterator>> cache;
  std::list<KeyType> lruList;
  std::mutex mtx;  // 用于同步的互斥锁
  std::function<bool(KeyType, ValueType)> deleteCallback;
  int cacheCapacity;  // 成员变量的容量

 public:
  // 构造函数，允许设置自定义容量和删除回调函数
  LRUCache(int customCapacity = defaultCapacity, std::function<bool(KeyType, ValueType)> callback = nullptr)
      : cacheCapacity(customCapacity), deleteCallback(callback) {}

  ValueType get(const KeyType& key) {
    std::lock_guard<std::mutex> lock(mtx);  // 线程安全
    if (cache.find(key) != cache.end()) {
      lruList.erase(cache[key].second);
      lruList.push_front(key);
      cache[key].second = lruList.begin();
      return cache[key].first;
    }
    return ValueType();  // 如果没找到，返回默认构造的值
  }

  void put(const KeyType& key, const ValueType& value) {
    std::lock_guard<std::mutex> lock(mtx);  // 线程安全
    if (cache.find(key) != cache.end()) {
      cache[key].first = value;
      lruList.erase(cache[key].second);
      lruList.push_front(key);
      cache[key].second = lruList.begin();
    } else {
      if (cache.size() >= cacheCapacity) {
        KeyType lruKey = lruList.back();
        
        // 调用删除回调函数
        if (deleteCallback) {
          deleteCallback(lruKey, cache[lruKey].first);
        }
        
        cache.erase(lruKey);
        lruList.pop_back();
      }
      lruList.push_front(key);
      cache[key] = std::make_pair(value, lruList.begin());
    }
  }
    
  bool remove(const KeyType& key) {
    std::lock_guard<std::mutex> lock(mtx);  // 线程安全
    bool status = false;
    if (cache.find(key) != cache.end()) {
      ValueType value = cache[key].first;
      lruList.erase(cache[key].second);
      if (deleteCallback) {
        status = deleteCallback(key, value);
      }
      
      cache.erase(key);
    }
    return status;
  }
bool clearAll() {
    std::lock_guard<std::mutex> lock(mtx);  // 线程安全
    // 从后向前遍历 lruList
    for (auto it = lruList.end(); it != lruList.begin();) {
        --it;  // 先向前移动迭代器
        KeyType key = *it;  // 获取当前键
        if (deleteCallback) {
            // 调用删除回调并检查返回值
            if (!deleteCallback(key, cache[key].first)) {
                return false;  // 如果删除失败，返回 false
            }
        }
        cache.erase(key);  // 从 cache 中删除该键
        it = lruList.erase(it);  // 删除当前节点并更新迭代器
    }
    return true;  // 成功删除所有元素，返回 true
}
size_t size(){
        return cache.size();
}
    
};

#endif //LRUCACHE_H