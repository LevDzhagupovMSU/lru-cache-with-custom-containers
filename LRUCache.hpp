#pragma once

#include "list.hpp"
#include "hash_map.hpp"

template<typename K, typename T>
class LRUCache{
    list<std::pair<K, T>> order_;
    hash_map<K, typename list<std::pair<K, T>>::iterator> cache_;
    size_t capacity_;
    public:

    LRUCache(size_t capacity = 10) : capacity_(capacity) {}

    T& get(const K& key){
        auto it = cache_.find(key);
        if(it == cache_.end())
            throw std::out_of_range("Key not found");
        
        order_.move_to_front(it->second);

        return it->second->second;
    };

    const T& get(const K& key) const {
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            throw std::out_of_range("Key not found");
        }
        
        return it->second->second;
    }

    void put(const K& key, const T& value){
        if(capacity_ == 0) return;

        auto it = cache_.find(key);
        if(it != cache_.end()){
            it->second->second = value;  
            order_.move_to_front(it->second);
            
            return;
        }

        if(order_.size() >= capacity_){
            auto tmp = typename decltype(order_)::iterator(order_.back());
            auto it_cache = cache_.find(tmp->first);
            cache_.erase(it_cache);
            order_.pop_back();
        }

        order_.push_front({key, value});
        cache_[key] = order_.begin();
    }; 

    void clear(){
        order_.clear();
        cache_.clear();
    }
    
    bool contains(const K& key) const{
        return cache_.find(key) != cache_.end(); 
    };

    size_t capacity() const{
        return capacity_;
    };

    size_t size() const{
        return order_.size();
    };

    void print_cache() const{
        for(const auto& elem : order_)
            std::cout << elem.first << ": " << elem.second << "     ";
        std::cout << std::endl;
    }
};