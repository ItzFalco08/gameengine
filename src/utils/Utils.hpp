#pragma once
#include <cstdint>
#include <random>
#include <cinttypes>
#include <cstdio>
#include <string>
#include <json/json.hpp>

struct UUID {
    uint64_t low{ 0 };
    uint64_t high{ 0 };

    UUID() = default;
    UUID(uint64_t other) : low(other), high(0) {}
    UUID(uint64_t low, uint64_t high) : low(low), high(high) {}

    static UUID fromString(const std::string& str) {
        UUID uuid;
        sscanf(
            str.c_str(),
            "%" SCNx64 "%" SCNx64,
            &uuid.high,
            &uuid.low
        );
        return uuid;
    }

    static std::string toString(const UUID& uuid) {
        char buffer[33];
        snprintf(
            buffer,
            sizeof(buffer),
            "%016" PRIx64 "%016" PRIx64,
            uuid.high,
            uuid.low
        );
        return std::string(buffer);
    }

	UUID& operator=(uint64_t value) {
		high = 0;
		low = value;
		return *this;
	}

    UUID& operator=(const UUID& value) {
        high = value.high;
        low = value.low;
        return *this;
    }


	bool operator==(const UUID& other) const {
		return low == other.low && high == other.high;
	}
};

struct UUIDHasher {
	std::size_t operator()(const UUID& uuid) const noexcept {
		std::size_t h1 = std::hash<uint64_t>{}(uuid.high);
		std::size_t h2 = std::hash<uint64_t>{}(uuid.low);

		return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
	}
};

UUID generateUUID() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());

    return {
        gen(),
        gen()
    };
}