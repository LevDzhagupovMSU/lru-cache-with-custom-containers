#include <iostream>
#include <cinttypes>

namespace cust_hash{
    template<typename K>
    struct hash{
        static size_t hash_func(const K& key) {
            if constexpr (std::is_same_v<K, std::string>) {
                size_t hash = 5381;
                for (char c : key) {
                    hash = ((hash << 5) + hash) + static_cast<size_t>(c);
                }
                return hash;
            } else {
                const std::uint8_t* bytes = reinterpret_cast<const std::uint8_t*>(&key);
                size_t hash = 5381;
                for (size_t i = 0; i < sizeof(K); ++i) {
                    hash = ((hash << 5) + hash) + bytes[i];
                }
                return hash;
            }
        }
    };
}