#pragma once

#include <iostream>
#include "list.hpp"
#include "hash.hpp"

template<typename K, typename T>
class hash_map{
    list<std::pair<const K, T>>* buckets_ = nullptr;
    
    std::equal_to<K> key_equal;
    
    size_t bucket_count_;
    size_t size_ = 0;

    double max_load_factor = 1.0;

    bool need_rehash() const{
        return size_ > bucket_count_ * max_load_factor;
    }

    public:
    class const_iterator;
    class iterator{ // очевидный инвариант надо согласовывать indx_cur_bucket_ и begin() иначе не сможем обойти мапу
        friend class const_iterator;

        typename list<std::pair<const K, T>>::node* ptr_;
        hash_map* parent_;
        size_t indx_cur_bucket_;

    public:
        iterator(typename list<std::pair<const K, T>>::node* ptr = nullptr, hash_map* parent = nullptr, size_t indx_cur_bucket = 0) : 
                        ptr_{ptr}, parent_{parent}, indx_cur_bucket_{indx_cur_bucket} {};

        iterator& operator++(){
            if(!parent_ || !ptr_) return *this;

            if(ptr_->prev_){
                ptr_ = ptr_->prev_;

                return *this;
            }
            
            for(int i = indx_cur_bucket_ + 1; i < parent_->bucket_count_; ++i){
                if(parent_->buckets_[i].back()){
                    indx_cur_bucket_ = i;
                    ptr_ = parent_->buckets_[i].back();

                    return *this;
                }
            }

            ptr_ = nullptr;
            indx_cur_bucket_ = parent_->bucket_count_;

            return *this;
        }
        
        iterator operator++(int){
            if(!parent_ || !ptr_) return *this;

            auto tmp = *this;
            if(ptr_->prev_){
                ptr_ = ptr_->prev_;

                return tmp;
            }
            
            for(int i = indx_cur_bucket_ + 1; i < parent_->bucket_count_; ++i){
                if(parent_->buckets_[i].back()){
                    indx_cur_bucket_ = i;
                    ptr_ = parent_->buckets_[i].back();

                    return tmp;
                }
            }

            ptr_ = nullptr;
            indx_cur_bucket_ = parent_->bucket_count_;

            return tmp;
        }

        typename list<std::pair<const K, T>>::iterator to_list_iterator() const {
            return typename list<std::pair<const K, T>>::iterator(ptr_);
        };

        typename std::pair<const K, T>& operator*() const{
            return ptr_->val_;
        }

        typename std::pair<const K, T>* operator->() const{
            return &(ptr_->val_);
        }

        bool operator!=(const iterator& other) const{
            return ptr_ != other.ptr_;
        }

        bool operator==(const iterator& other) const{
            return ptr_ == other.ptr_;
        }

        bool operator!=(const const_iterator& other) const{
            return ptr_ != other.ptr_;
        }

        bool operator==(const const_iterator& other) const{
            return ptr_ == other.ptr_;
        }

        typename list<std::pair<const K, T>>::node* get_ptr() const{
            return ptr_;
        }

        const hash_map* get_parent() const{
            return parent_;
        }

        size_t get_index_bucket() const{
            return indx_cur_bucket_;
        }
    };
    
    class const_iterator{
        friend class hash_map;

        const typename list<std::pair<const K, T>>::node* ptr_;
        const hash_map* parent_;
        size_t indx_cur_bucket_;

    public:
        const_iterator(const typename list<std::pair<const K, T>>::node* ptr, const hash_map* parent, size_t indx_cur_bucket) :
            ptr_{ptr}, parent_{parent}, indx_cur_bucket_{indx_cur_bucket} {};

        const_iterator(const iterator& it) : 
            ptr_{it.ptr_}, parent_{it.parent_}, indx_cur_bucket_{it.indx_cur_bucket_} {};

        const_iterator& operator++(){
            if(!parent_ || !ptr_) return *this;

            if(ptr_->prev_){
                ptr_ = ptr_->prev_;

                return *this;
            }
            
            for(int i = indx_cur_bucket_ + 1; i < parent_->bucket_count_; ++i){
                if(parent_->buckets_[i].back()){
                    indx_cur_bucket_ = i;
                    ptr_ = parent_->buckets_[i].back();

                    return *this;
                }
            }

            ptr_ = nullptr;
            indx_cur_bucket_ = parent_->bucket_count_;
            return *this;
        }
        
        const iterator operator++(int){
            if(!parent_ || !ptr_) return *this;

            auto tmp = *this;
            if(ptr_->prev_){
                ptr_ = ptr_->prev_;

                return tmp;
            }
            
            for(int i = indx_cur_bucket_ + 1; i < parent_->bucket_count_; ++i){
                if(parent_->buckets_[i].back()){
                    indx_cur_bucket_ = i;
                    ptr_ = parent_->buckets_[i].back();

                    return tmp;
                }
            }

            ptr_ = nullptr;
            indx_cur_bucket_ = parent_->bucket_count_;

            return tmp;
        }

        typename list<std::pair<const K, T>>::const_iterator to_list_const_iterator() const {
            return typename list<std::pair<const K, T>>::const_iterator{ptr_};
        }

        const typename std::pair<const K, T>& operator*() const{
            return ptr_->val_;
        }

        const typename std::pair<const K, T>* operator->() const{
            return &(ptr_->val_);
        }

        bool operator!=(const const_iterator& other) const{
            return ptr_ != other.ptr_;
        }

        bool operator==(const const_iterator& other) const{
            return ptr_ == other.ptr_;
        }

        bool operator!=(const iterator& other) const{
            return ptr_ != other.ptr_;
        }

        bool operator==(const iterator& other) const{
            return ptr_ == other.ptr_;
        }

        const typename list<std::pair<const K, T>>::node* get_ptr() const{
            return ptr_;
        }

        const hash_map* get_parent() const{
            return parent_;
        }

        size_t get_index_bucket() const{
            return indx_cur_bucket_;
        }
    };

    hash_map(size_t bucket_size = 16) : bucket_count_(bucket_size) {
        buckets_ = new list<std::pair<const K, T>>[bucket_count_]();
    };

    ~hash_map() noexcept {
        delete[] buckets_;
    };

    hash_map(const hash_map<const K, T>&) = delete;
    hash_map& operator=(const hash_map<const K, T>&) = delete;
    
    hash_map(hash_map<const K, T>&& other) noexcept : buckets_(other.buckets_),
                                bucket_count_(other.bucket_count_), size_(other.size) {
        other.buckets_ = nullptr;
        other.bucket_count_ = 0;
        other.size_ = 0;
    };

    hash_map& operator=(hash_map<const K, T>&& other) noexcept {
        if(this == &other && !other.buckets_) return *this;

        std::swap(*this, other);
        return *this;
    };

    void erase(const iterator& elem) {
        auto list_it = elem.to_list_iterator(); 
        elem.get_parent()->buckets_[elem.get_index_bucket()].erase(list_it);
    };

    void insert(const K& key, const T& val){

        if(need_rehash()){
            rehash();
        }

        auto hash = cust_hash::hash<K>::hash_func(key) % bucket_count_;
        std::cout << bucket_count_ << " ";
        for(auto& node_pair : buckets_[hash]){
            auto this_hash = cust_hash::hash<K>::hash_func(node_pair.first) % bucket_count_;
             if(this_hash == hash && key_equal(key == node_pair.first)){
                node_pair.second = val;
                return;
            }
        }
        buckets_[hash].push_back({key, val});
        size_++;
    };

    iterator find(const K& key){
        auto hash = cust_hash::hash<K>::hash_func(key) % bucket_count_;

        if (buckets_[hash].empty()) {
            return end();
        }

        for(auto it = buckets_[hash].begin(); it != buckets_[hash].end(); ++it){
            if(key_equal((*it).first, key))
                return iterator{it.get_ptr(), this, hash};
        }

        return end();
    }

    const_iterator find(const K& key) const{
        auto hash = cust_hash::hash<K>::hash_func(key) % bucket_count_;

        if (buckets_[hash].empty()) {
            return end();
        }

        for(auto it = buckets_[hash].begin(); it != buckets_[hash].end(); ++it){
            if(key_equal((*it).first, key))
                return const_iterator{it.get_ptr(), this, hash};
        }

        return end();
    }

    T& operator[](const K& key){

        if(need_rehash()){
            rehash();
        }

        auto hash = cust_hash::hash<K>::hash_func(key) % bucket_count_;        
        for(auto& node_pair : buckets_[hash]){
            auto this_hash = cust_hash::hash<K>::hash_func(node_pair.first) % bucket_count_;
            if(this_hash == hash && key_equal(key, node_pair.first)){
                return node_pair.second;
            }
        }

        buckets_[hash].push_back({key, {}});
        size_++;

        return buckets_[hash].back()->val_.second;
    };

    void rehash() noexcept{
        hash_map<K, T> new_map{bucket_count_ << 1};

        for(size_t i = 0; i < bucket_count_; ++i){
            for(auto& pair : buckets_[i]){
                auto hash = cust_hash::hash<K>::hash_func(pair.first) % new_map.bucket_count_;
                new_map.buckets_[hash].push_back(std::move(pair));
            }
        }
        
        std::swap(buckets_, new_map.buckets_);
        std::swap(bucket_count_, new_map.bucket_count_);
    };

    size_t size() const{
        return size_;
    };

    void clear(){
        for(int i = 0; i < bucket_count_; ++i){
            if(buckets_[i].size() != 0){
                buckets_[i].clear();
            }
        }

        size_ = 0;
    }

    iterator begin() {
        for (size_t i = 0; i < bucket_count_; ++i) {
            if (!buckets_[i].empty()) {
                return iterator{buckets_[i].back(), this, i}; 
            }
        }
        return end();
    };

    const_iterator begin() const{
        for (size_t i = 0; i < bucket_count_; ++i) {
            if (!buckets_[i].empty()) {
                return const_iterator{buckets_[i].back(), this, i}; 
            }
        }
        return end();
    };

    iterator end() {
        return iterator{nullptr, this, bucket_count_};
    };

    const_iterator end() const{
        return const_iterator{nullptr, this, bucket_count_};
    };
};
