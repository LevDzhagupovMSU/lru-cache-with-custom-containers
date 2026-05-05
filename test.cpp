#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <numeric>
#include <cmath>

#include "LRUCache.hpp"

using namespace std::chrono;

// Вспомогательная функция для форматирования чисел
std::string format_number(size_t n) {
    if (n < 1000) return std::to_string(n);
    if (n < 1000000) return std::to_string(n / 1000) + "K";
    return std::to_string(n / 1000000) + "M";
}

template<typename K>
K random_key(std::mt19937& rng, const std::vector<K>& keys) {
    std::uniform_int_distribution<size_t> dist(0, keys.size() - 1);
    return keys[dist(rng)];
}

// ===================== Тест 1: Только вставки (put) =====================
void benchmark_put(size_t capacity, size_t num_ops) {
    LRUCache<int, int> cache(capacity);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> key_dist(0, num_ops * 2);

    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < num_ops; ++i) {
        int key = key_dist(rng);
        cache.put(key, key);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(end - start).count();
    
    double ops_per_sec = (num_ops * 1e9) / duration;

    std::cout << std::left << std::setw(18) << "Put only"
              << " | cap=" << std::setw(6) << capacity
              << " | ops=" << std::setw(8) << format_number(num_ops)
              << " | time=" << std::setw(8) << duration / 1000 << " μs"
              << " | avg=" << std::setw(6) << duration / num_ops << " ns"
              << " | " << std::setw(10) << (ops_per_sec / 1e6) << " M ops/s"
              << std::endl;
}

// ===================== Тест 2: Только чтения =====================
void benchmark_get_existing(size_t capacity, size_t num_ops) {
    LRUCache<int, int> cache(capacity);
    std::mt19937 rng(42);

    for (size_t i = 0; i < capacity; ++i) {
        cache.put(i, i * 10);
    }

    std::vector<int> existing_keys(capacity);
    std::iota(existing_keys.begin(), existing_keys.end(), 0);

    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < num_ops; ++i) {
        int key = random_key(rng, existing_keys);
        volatile int val = cache.get(key);
        (void)val;
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(end - start).count();
    
    double ops_per_sec = (num_ops * 1e9) / duration;

    std::cout << std::left << std::setw(18) << "Get only"
              << " | cap=" << std::setw(6) << capacity
              << " | ops=" << std::setw(8) << format_number(num_ops)
              << " | time=" << std::setw(8) << duration / 1000 << " μs"
              << " | avg=" << std::setw(6) << duration / num_ops << " ns"
              << " | " << std::setw(10) << (ops_per_sec / 1e6) << " M ops/s"
              << std::endl;
}

// ===================== Тест 3: Hit rate в зависимости от размера working set =====================
void benchmark_hit_rate(size_t capacity, size_t working_set_size, size_t num_ops) {
    LRUCache<int, int> cache(capacity);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> key_dist(0, working_set_size - 1);
    
    size_t hits = 0;
    
    for (size_t i = 0; i < num_ops; ++i) {
        int key = key_dist(rng);
        if (cache.contains(key)) {
            cache.get(key);
            hits++;
        } else {
            cache.put(key, key);
        }
    }
    
    double hit_rate = (hits * 100.0) / num_ops;
    
    std::cout << std::left << std::setw(18) << "Hit rate"
              << " | cap=" << std::setw(6) << capacity
              << " | ws=" << std::setw(6) << working_set_size
              << " | hits=" << std::setw(5) << (int)hit_rate << "%"
              << " | ops=" << std::setw(8) << format_number(num_ops)
              << std::endl;
}

// ===================== Тест 4: Сравнение с различными размерами =====================
void benchmark_scale_comparison() {
    std::cout << "\n--- Scalability comparison (1M operations) ---\n";
    std::vector<size_t> capacities = {10, 100, 1000, 10000};
    const size_t num_ops = 1000000;
    
    std::cout << std::string(80, '=') << std::endl;
    for (size_t cap : capacities) {
        benchmark_put(cap, num_ops);
        benchmark_get_existing(cap, num_ops);
        std::cout << std::string(80, '-') << std::endl;
    }
}

// ===================== Тест 5: Зависимость от типа данных =====================
void benchmark_different_types() {
    std::cout << "\n--- Performance with different key/value types ---\n";
    const size_t capacity = 1000;
    const size_t num_ops = 500000;
    
    // Тест 1: int, int
    {
        LRUCache<int, int> cache(capacity);
        std::mt19937 rng(42);
        auto start = high_resolution_clock::now();
        for (size_t i = 0; i < num_ops; ++i) {
            int key = i % 2000;
            cache.put(key, i);
            if (i % 2 == 0 && cache.contains(key)) {
                volatile int v = cache.get(key);
                (void)v;
            }
        }
        auto end = high_resolution_clock::now();
        auto ns = duration_cast<nanoseconds>(end - start).count();
        std::cout << "int,int:     " << ns / 1000 << " μs total, " 
                  << ns / num_ops << " ns/op, "
                  << (num_ops * 1e9) / ns / 1e6 << " M ops/s\n";
    }
    
    // Тест 3: int, string
    {
        LRUCache<int, std::string> cache(capacity);
        std::mt19937 rng(42);
        auto start = high_resolution_clock::now();
        for (size_t i = 0; i < num_ops; ++i) {
            int key = i % 2000;
            std::string value = "value_" + std::to_string(i);
            cache.put(key, value);  // value копируется в кэш - OK
            if (i % 2 == 0 && cache.contains(key)) {
                volatile std::string v = cache.get(key);
                (void)v;
            }
        }
        auto end = high_resolution_clock::now();
        auto ns = duration_cast<nanoseconds>(end - start).count();
        std::cout << "int,string:  " << ns / 1000 << " μs total, " 
                  << ns / num_ops << " ns/op, "
                  << (num_ops * 1e9) / ns / 1e6 << " M ops/s\n";
    }
    
    {
        LRUCache<std::string, int> cache(capacity);
        std::mt19937 rng(42);
        auto start = high_resolution_clock::now();
        for (size_t i = 0; i < num_ops; ++i) {
            std::string key = "key_" + std::to_string(i % 2000);
            cache.put(key, i);  // key копируется в кэш - OK
            if (i % 2 == 0 && cache.contains(key)) {
                volatile int v = cache.get(key);
                (void)v;
            }
        }
        auto end = high_resolution_clock::now();
        auto ns = duration_cast<nanoseconds>(end - start).count();
        std::cout << "string,int:  " << ns / 1000 << " μs total, " 
                  << ns / num_ops << " ns/op, "
                  << (num_ops * 1e9) / ns / 1e6 << " M ops/s\n";
    }

    // Тест 4: string, string
    {
        LRUCache<std::string, std::string> cache(capacity);
        std::mt19937 rng(42);
        auto start = high_resolution_clock::now();
        for (size_t i = 0; i < num_ops; ++i) {
            std::string key = "key_" + std::to_string(i % 2000);
            std::string value = "value_" + std::to_string(i);
            cache.put(key, value);
            if (i % 2 == 0 && cache.contains(key)) {
                volatile std::string v = cache.get(key);
                (void)v;
            }
        }
        auto end = high_resolution_clock::now();
        auto ns = duration_cast<nanoseconds>(end - start).count();
        std::cout << "string,string: " << ns / 1000 << " μs total, " 
                  << ns / num_ops << " ns/op, "
                  << (num_ops * 1e9) / ns / 1e6 << " M ops/s\n";
    }
}


// ===================== Поиск точного места падения =====================
void find_exact_crash_point() {
    std::cout << "\n=== Finding exact crash point ===\n";
    
    LRUCache<std::string, int> cache(1000);
    
    for (int i = 0; i < 10000; ++i) {
        std::string key = "key_" + std::to_string(i % 2000);
        
        if (i >= 5000 && i <= 6000) {
            std::cout << "i=" << i << ": ";
            std::cout << "key=" << key << ", ";
            std::cout << "cache.size()=" << cache.size() << ", ";
            std::cout.flush();
        }
        
        cache.put(key, i);
        
        if (i >= 5000 && i <= 6000) {
            std::cout << "put OK, ";
            std::cout.flush();
        }
        
        if (i % 2 == 0 && cache.contains(key)) {
            if (i >= 5000 && i <= 6000) {
                std::cout << "contains OK, ";
                std::cout.flush();
            }
            
            volatile int val = cache.get(key);
            (void)val;
            
            if (i >= 5000 && i <= 6000) {
                std::cout << "get OK" << std::endl;
            }
        } else if (i >= 5000 && i <= 6000) {
            std::cout << "skip get" << std::endl;
        }
    }
}

// ===================== Тест 6: Распределение времени операций =====================
void benchmark_latency_distribution() {
    std::cout << "\n--- Latency distribution (capacity=1000, 100k ops) ---\n";
    LRUCache<int, int> cache(1000);
    std::mt19937 rng(42);
    const size_t num_ops = 100000;
    
    std::vector<long long> latencies;
    latencies.reserve(num_ops);
    
    for (int i = 0; i < 1000; ++i) {
        cache.put(i, i);
    }
    
    std::vector<int> keys(1000);
    std::iota(keys.begin(), keys.end(), 0);
    
    for (size_t i = 0; i < num_ops; ++i) {
        int key = random_key(rng, keys);
        auto start = high_resolution_clock::now();
        volatile int val = cache.get(key);
        auto end = high_resolution_clock::now();
        (void)val;
        latencies.push_back(duration_cast<nanoseconds>(end - start).count());
    }
    
    std::sort(latencies.begin(), latencies.end());
    
    auto p50 = latencies[num_ops * 50 / 100];
    auto p90 = latencies[num_ops * 90 / 100];
    auto p95 = latencies[num_ops * 95 / 100];
    auto p99 = latencies[num_ops * 99 / 100];
    auto p999 = latencies[num_ops * 999 / 1000];
    
    std::cout << "P50:  " << std::setw(6) << p50 << " ns\n";
    std::cout << "P90:  " << std::setw(6) << p90 << " ns\n";
    std::cout << "P95:  " << std::setw(6) << p95 << " ns\n";
    std::cout << "P99:  " << std::setw(6) << p99 << " ns\n";
    std::cout << "P999: " << std::setw(6) << p999 << " ns\n";
}

int main() {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "========== LRU Cache Performance Tests ==========\n\n";
    
    const size_t num_ops = 500000;
    
    // Базовые тесты
    std::cout << "--- Basic operations (500k ops) ---\n";
    benchmark_put(100, num_ops);
    benchmark_get_existing(100, num_ops);
    std::cout << "\n";
    
    // Анализ hit rate
    std::cout << "--- Hit rate analysis (100k ops) ---\n";
    benchmark_hit_rate(100, 50, 100000);
    benchmark_hit_rate(100, 200, 100000);
    benchmark_hit_rate(100, 500, 100000);
    benchmark_hit_rate(100, 1000, 100000);
    std::cout << "\n";

    // Другие бенчмарки
    benchmark_scale_comparison();
    
    benchmark_different_types();

    benchmark_latency_distribution();
    
    std::cout << "\n========== Tests finished ==========\n";
    return 0;
}