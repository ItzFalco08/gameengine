#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include "../../utils/UUID.hpp"
#include <iostream>

template<typename T>
struct AssetsPool {
	// expose iteration
	auto begin() const { return m_fromUUID.begin(); }
	auto end() const { return m_fromUUID.end(); }

	static constexpr uint32_t INVALID = ~0u;

	void set(const UUID& id, std::unique_ptr<T> item) {
		uint32_t idx = find(id);
		if (idx != INVALID) { // update existing asset
			m_assets[idx] = std::move(item);
			return;
		}

		if (!m_freeIndices.empty()) {                  // reuse a freed slot
			idx = m_freeIndices.back();
			m_freeIndices.pop_back();
			m_assets[idx] = std::move(item);
		}
		else {                                         // append a new slot
			idx = static_cast<uint32_t>(m_assets.size());
			m_assets.push_back(std::move(item));
		}
		
		m_fromUUID[id] = idx;
	}

	uint32_t find(const UUID& id) const {
		auto it = m_fromUUID.find(id);
		return it == m_fromUUID.end() ? INVALID : it->second;
	}

	T* get(const UUID& id) { return get(find(id)); }

	T* get(uint32_t idx) {
		return idx < m_assets.size() ? m_assets[idx].get() : nullptr;   // nullptr if empty slot
	}

	bool has(const UUID& id) const { return find(id) != INVALID; }

	void remove(const UUID& uuid) {
		uint32_t id = find(uuid);
		if (id == INVALID) return;
		
		m_fromUUID.erase(uuid);
		m_freeIndices.push_back(id);
		m_assets[id].reset();  // Reset the unique_ptr to free the memory
	}

	uint32_t size() {
		return m_fromUUID.size();
	}

	std::unordered_map<UUID, uint32_t, UUIDHasher> m_fromUUID;
private:
	std::vector<std::unique_ptr<T>> m_assets;
	std::vector<uint32_t> m_freeIndices;
};