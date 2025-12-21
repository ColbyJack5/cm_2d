#pragma once
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <optional>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "misc.h"
#include "constants.h"


template <typename T, int MaxCapacity>
class ObjectPool {
private:
    struct Slot {
        std::optional<T> data;
        int generation = 0;
    };

    std::array<Slot, MaxCapacity> slots;
    std::vector<int> free_indices;
    int high_water_mark = 0; 
    int active_count = 0;

public:
    ObjectPool() {
        free_indices.reserve(MaxCapacity);
    }


    template <typename... Args>
    T* create(Args&&... args) {
        if (active_count >= MaxCapacity) {
            return nullptr; 
        }

        int index;
        if (!free_indices.empty()) {
            index = free_indices.back();
            free_indices.pop_back();
        } else {
            if (high_water_mark < MaxCapacity) {
                index = high_water_mark++;
            } else {
                std::cout << "Exceeded limit" << "\n"; 
                return nullptr;
            }
        }
        active_count++;

        // Emplace the object. Pass the Identifier to the constructor 
        Slot& slot = slots[index];
        slot.data.emplace(Identifier{index, slot.generation}, std::forward<Args>(args)...);
        
        return &slot.data.value();
    }

    
    void remove(Identifier identity) {
        if (!isValid(identity)) return;
        Slot& slot = slots[identity.id];
        slot.data.reset(); // Destroys object
        slot.generation++; // Increment generation so old Identifiers become invalid
        free_indices.push_back(identity.id);
        active_count--;
    }


    T* get(Identifier identity) {
        if(!isValid(identity)) return nullptr;
        return &slots[identity.id].data.value();
    }


    bool isValid(Identifier identity) const {
        if (identity.id < 0 || identity.id >= MaxCapacity) return false;
        const Slot& slot = slots[identity.id];
        if (!slot.data.has_value()) return false;
        if (identity.generation != slot.generation) return false;
        return true;
    }


    void clear() {
        for (int i = 0; i < high_water_mark; ++i) {
            if (slots[i].data.has_value()) {
                slots[i].data.reset();
                slots[i].generation++;
            }
        }
        free_indices.clear();
        high_water_mark = 0;
        active_count = 0;
    }


    // Iterator support for internal looping
    template<typename Func>
    void forEach(Func func) {
        for (int i = 0; i < high_water_mark; ++i) {
            if (slots[i].data.has_value()) {
                func(slots[i].data.value());
            }
        }
    }
};