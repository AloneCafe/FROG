#ifndef __TS_MAP_WARPPER_HEADER__
#define __TS_MAP_WARPPER_HEADER__

#include <mutex>
#include <unordered_map>

#if 0

template <typename K, typename T, typename H = std::hash<K>>
using TSMapWrapper = std::unordered_map<K, T, H>;

#else

template <typename K, typename T, typename H = std::hash<K>>
class TSMapWrapper {
private:
    static std::mutex _mtx;
    std::unordered_map<K, T, H> _map;

public:
    typename std::unordered_map<K, T, H>::const_iterator
    cbegin() const;
    
    typename std::unordered_map<K, T, H>::const_iterator
    cend() const;
    
    typename std::unordered_map<K, T, H>::const_iterator
    begin() const;
    
    typename std::unordered_map<K, T, H>::const_iterator
    end() const;
    
    typename std::unordered_map<K, T, H>::iterator
    begin();
    
    typename std::unordered_map<K, T, H>::iterator
    end();
    
    typename std::unordered_map<K, T, H>::iterator
    erase(typename std::unordered_map<K, T, H>::const_iterator pos);
    
    typename std::unordered_map<K, T, H>::iterator
    erase(typename std::unordered_map<K, T, H>::iterator pos);
    
    std::pair<
            typename std::unordered_map<K, T, H>::iterator, bool
    >   insert(const std::pair<K, T> & e);
    
    typename std::unordered_map<K, T, H>::const_iterator
    find(const K & e) const;
    
    typename std::unordered_map<K, T, H>::iterator
    find(const K & e);
    
    void clear();
};


template <typename K, typename T, typename H>
std::pair<typename std::unordered_map<K, T, H>::iterator, bool>
TSMapWrapper<K, T, H>::insert(const std::pair<K, T> & e) {
    std::lock_guard<std::mutex> lock(_mtx);
    return _map.insert(e);
}

template <typename K, typename T, typename H>
void TSMapWrapper<K, T, H>::clear() {
    std::lock_guard<std::mutex> lock(_mtx);
    _map.clear();
}

template <typename K, typename T, typename H>
typename std::unordered_map<K, T, H>::const_iterator
TSMapWrapper<K, T, H>::find(const K & e) const {
    std::lock_guard<std::mutex> lock(_mtx);
    return _map.find(e);
}

template <typename K, typename T, typename H>
typename std::unordered_map<K, T, H>::iterator
TSMapWrapper<K, T, H>::find(const K & e) {
    std::lock_guard<std::mutex> lock(_mtx);
    return _map.find(e);
}

template <typename K, typename T, typename H>
typename std::unordered_map<K, T, H>::const_iterator
TSMapWrapper<K, T, H>::cbegin() const {
    std::lock_guard<std::mutex> lock(_mtx);
    return _map.cbegin();
}

template <typename K, typename T, typename H>
typename std::unordered_map<K, T, H>::const_iterator
TSMapWrapper<K, T, H>::cend() const {
    std::lock_guard<std::mutex> lock(_mtx);
    return _map.cend();
}

template <typename K, typename T, typename H>
typename std::unordered_map<K, T, H>::iterator
TSMapWrapper<K, T, H>::begin() {
    std::lock_guard<std::mutex> lock(_mtx);
    return _map.begin();
}

template <typename K, typename T, typename H>
typename std::unordered_map<K, T, H>::iterator
TSMapWrapper<K, T, H>::end() {
    std::lock_guard<std::mutex> lock(_mtx);
    return _map.end();
}

template <typename K, typename T, typename H>
typename std::unordered_map<K, T, H>::const_iterator
TSMapWrapper<K, T, H>::begin() const {
    std::lock_guard<std::mutex> lock(_mtx);
    return _map.begin();
}

template <typename K, typename T, typename H>
typename std::unordered_map<K, T, H>::const_iterator
TSMapWrapper<K, T, H>::end() const {
    std::lock_guard<std::mutex> lock(_mtx);
    return _map.end();
}

template <typename K, typename T, typename H>
typename std::unordered_map<K, T, H>::iterator
TSMapWrapper<K, T, H>::erase(typename std::unordered_map<K, T, H>::iterator pos) {
    std::lock_guard<std::mutex> lock(_mtx);
    return _map.erase(pos);
}

template <typename K, typename T, typename H>
typename std::unordered_map<K, T, H>::iterator
TSMapWrapper<K, T, H>::erase(typename std::unordered_map<K, T, H>::const_iterator pos) {
    return _map.erase(pos);
}

template <typename K, typename T, typename H>
std::mutex TSMapWrapper<K, T, H>::_mtx;

#endif

#endif
