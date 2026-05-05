#pragma once

#include <initializer_list>

template<typename Key, typename Value>
class hash_map; 


template <typename T>
class list{    
    struct node{
        T val_;
        node* next_ = nullptr, *prev_ = nullptr;

        node() : val_{} {};

        node(const T& val) : val_{val} {};

        T& get() & {
            return val_;
        };

        node* operator++(){
            return next_;
        };
    };
    
    node* top_ = nullptr;
    node* tail_ = nullptr;
    std::size_t size_;

    template<typename Key,typename Val>
    friend class hash_map;

    public:
    class const_iterator;

    class iterator{
        friend class const_iterator;

        list<T>::node* ptr_;
    public:
        iterator(list<T>::node* ptr = nullptr) : ptr_{ptr} {};

        iterator& operator++(){
            ptr_ = ptr_->next_;
            return *this;
        }

        iterator operator++(int){
            auto tmp = *this;
            ptr_ = ptr_->next_;
            return tmp;
        }

        T& operator*() const{
            return ptr_->val_;
        }

        T* operator->() const{
            return &(ptr_->val_);
        }

        bool operator!=(const iterator& other) const{
            return ptr_ != other.ptr_;
        }

        bool operator==(const iterator& other) const {
            return ptr_ == other.ptr_;
        }

        bool operator==(const const_iterator& other) const {
            return ptr_ == other.ptr_;
        }

        bool operator!=(const const_iterator& other) const {
            return ptr_ != other.ptr_;
        }

        list<T>::node* get_ptr() const{
            return ptr_;
        }
    };

    class const_iterator{
        friend class iterator;

        const list<T>::node* ptr_;

    public:
        const_iterator(const list<T>::node* ptr = nullptr) : ptr_{ptr} {};
        const_iterator(const iterator& it) : ptr_{it.ptr_} {};

        const_iterator& operator++(){
            ptr_ = ptr_->next_;
            return *this;
        }

        const_iterator operator++(int){
            auto tmp = *this;
            ptr_ = ptr_->next_;
            return tmp;
        }

        const T& operator*() const{
            return ptr_->val_;
        }

        const T* operator->() const{
            return &(ptr_->val_);
        }

        bool operator!=(const const_iterator& other) const{
            return ptr_ != other.ptr_;
        }

        bool operator==(const const_iterator& other) const {
            return ptr_ == other.ptr_;
        }

        bool operator==(const iterator& other) const {
            return ptr_ == other.ptr_;
        }

        bool operator!=(const iterator& other) const {
            return ptr_ != other.ptr_;
        }

        const list<T>::node* get_ptr() const{
            return ptr_;
        }
    };

    list() : size_{} {};
    
    list(std::initializer_list<T> l) : size_(l.size()) {
        if(l.size() == 0) return;

        top_ = new node{*l.begin()};
        node* cur = top_;

        bool flag = true;
        for(const auto& elem : l){
            if(flag){
                flag = false;
                continue;
            }

            cur->next_ = new node{elem};
            cur->next_->prev_ = cur;
            cur = cur->next_;
        }

        tail_ = cur;
    };

    ~list() noexcept{
        node* cur = top_;
        while(cur){ 
            node* tmp = cur;
            cur = cur->next_;
            delete tmp;
        }
    };

    list(const list& other) : size_(other.size_){
        if(size_ == 0) return;

        top_ = new node{other.top_->val_};

        node* cur = other.top_->next_;
        node* cur_this = top_;
        while(cur){
            cur_this->next_ = new node{cur->val_};
            cur_this->next_->prev_ = cur_this;
            
            cur = cur->next_;
            cur_this = cur_this->next_;
        }

        tail_ = cur_this;
    };

    list(list&& other) noexcept : top_{other.top_}, tail_{other.tail_}, size_{other.size_} {                
        other.top_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    };
    
    list& operator=(const list& other) & {
        if(this == &other) return *this;

        node* cur_this = top_;
        while(cur_this){
            node* tmp = cur_this;
            cur_this = cur_this->next_;
            delete tmp;
        }

        size_ = other.size_;
        if(size_ == 0){
            top_ = nullptr;
            return *this;
        }
        top_ = new node{other.top_->val_};

        node* cur = other.top_->next_;
        cur_this = top_;

        while(cur){
            cur_this->next_ = new node{cur->val_};
            cur_this->next_->prev_ = cur_this;

            cur = cur->next_;
            cur_this = cur_this->next_;
        }

        tail_ = cur_this;
    
        return *this;
    };
    
    list& operator=(list&& other) noexcept {
        if(this == &other) return *this;

        node* cur_this = top_;
        while(cur_this){
            node* tmp = cur_this;
            cur_this = cur_this->next_;
            delete tmp;
        }

        size_ = other.size_;
        top_ = other.top_;
        tail_ = other.tail_;

        other.size_ = 0;
        other.top_ = nullptr;
        other.tail_ = nullptr;

        return *this;
    };

    node* front() const{
        return top_;
    };

    node* back() const{
        return tail_;
    };

    const node* cfront() const{
        return top_;
    }

    const node* cback() const{
        return tail_;
    }

    void push_back(const T& elem){
        if(top_ == tail_ && !top_){
            top_ = new node{elem};
            tail_ = top_;
            size_++;

            return;
        }
        tail_->next_ = new node{elem};
        tail_->next_->prev_ = tail_;
        tail_ = tail_->next_;
        size_++;
    };

    void push_front(const T& elem){
        if(top_ == tail_ && !top_){
            top_ = new node{elem};
            tail_ = top_;
            size_++;

            return;
        }
        top_->prev_ = new node{elem};
        top_->prev_->next_ = top_;
        top_ = top_->prev_;
        size_++;
    };

    std::size_t size() const{
        return size_;
    }

    void pop_back(){
        erase(tail_);
    }

    void pop_front(){
        erase(top_);
    }

    void erase(const iterator& target){
        if(!target.get_ptr()) return;

        if(size_ == 1){
            top_ = tail_ = nullptr;
            size_ = 0;
            delete target.get_ptr();
            return;
        }

        if(target.get_ptr()->next_ && target.get_ptr()->prev_){
            node* tmp = target.get_ptr()->prev_;
            tmp->next_ = target.get_ptr()->next_;
            target.get_ptr()->next_->prev_ = tmp;
        }
        else if(target.get_ptr() == tail_){
            tail_->prev_->next_ = nullptr;
            tail_ = tail_->prev_;
        }else if(target.get_ptr() == top_){
            top_->next_->prev_ = nullptr;
            top_ = top_->next_;
        }
        
        delete target.get_ptr();
        size_--;
    }

    bool empty() const{
        return tail_ == nullptr;
    }

    iterator begin(){
        return top_;
    }

    const_iterator begin() const{
        return top_;
    }

    iterator end(){
        return iterator{};
    }

    const_iterator end() const{
        return const_iterator{};
    }

    void clear(){
        node* it = top_;
        while(it){
            node* tmp = it->next_;
            delete it;
            it = tmp;
        }
        
        top_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    void move_to_front(const iterator& target){
        if(!target.get_ptr() || target.get_ptr() == top_) return;

        node* current = target.get_ptr();

        if(current == top_) return;

        if(current->prev_) 
            current->prev_->next_ = current->next_;   
        if(current->next_) 
            current->next_->prev_ = current->prev_;

        if(current == tail_) 
            tail_ = current->prev_;

        current->prev_ = nullptr;
        current->next_ = top_;
        if(top_) 
            top_->prev_ = current;  
        top_ = current;

        if(!tail_) 
            tail_ = current;
    };
};
