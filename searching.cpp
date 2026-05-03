#include "searching.hpp"
#include "analysis.hpp"
#include <chrono>
#include <iostream>

// --- array: linear search by age group ---
// O(n) time, O(1) space

PerfMetrics searchByAgeGroupArray(const ResidentArray& arr, const std::string& ageGroupCode) {
    auto start = std::chrono::high_resolution_clock::now();
    
    int count = 0;
    for (int i = 0; i < arr.count; i++) {
        if (getAgeGroupCode(arr.data[i].age) == ageGroupCode) {
            count++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Resident) * arr.count;
    metrics.itemsProcessed = count;
    
    return metrics;
}

// --- array: linear search by mode ---
// O(n) time, O(1) space

PerfMetrics searchByModeArray(const ResidentArray& arr, const std::string& mode) {
    auto start = std::chrono::high_resolution_clock::now();
    
    int count = 0;
    for (int i = 0; i < arr.count; i++) {
        if (arr.data[i].modeOfTransport == mode) {
            count++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Resident) * arr.count;
    metrics.itemsProcessed = count;
    
    return metrics;
}

// --- array: linear search by distance threshold ---
// O(n) time, O(1) space, just dailyDistance > threshold

PerfMetrics searchByDistanceThresholdArray(const ResidentArray& arr, double threshold) {
    auto start = std::chrono::high_resolution_clock::now();
    
    int count = 0;
    for (int i = 0; i < arr.count; i++) {
        if (arr.data[i].dailyDistance > threshold) {
            count++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Resident) * arr.count;
    metrics.itemsProcessed = count;
    
    return metrics;
}

// --- array: binary search by exact age ---
// O(log n) time, O(1) space (when sorted, obviously)
//
// quick notes:
// - array MUST be pre-sorted by age (ascending)
// - each compare drops half the range
// - 500 elems -> ~9 compares vs 500 for linear
// - compare target with middle:
//   * equal: found
//   * target < middle: go left
//   * target > middle: go right

PerfMetrics binarySearchByAgeArray(const ResidentArray& arr, int targetAge) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // binary search assumes array is sorted by age (not sure who'd forget, but still)
    int left = 0;
    int right = arr.count - 1;
    int count = 0;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (arr.data[mid].age == targetAge) {
            // found a match, count all residents with this age
            count = 1;
            
            // scan left for first occurrence
            int leftIdx = mid - 1;
            while (leftIdx >= 0 && arr.data[leftIdx].age == targetAge) {
                count++;
                leftIdx--;
            }
            
            // scan right for last occurrence
            int rightIdx = mid + 1;
            while (rightIdx < arr.count && arr.data[rightIdx].age == targetAge) {
                count++;
                rightIdx++;
            }
            
            break;
        } else if (arr.data[mid].age < targetAge) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Resident) * arr.count;
    metrics.itemsProcessed = count;
    
    return metrics;
}

// --- list: linear search by age group ---
// O(n) time, O(1) space, pointer walk only

PerfMetrics searchByAgeGroupList(const LinkedList& list, const std::string& ageGroupCode) {
    auto start = std::chrono::high_resolution_clock::now();
    
    int count = 0;
    Node* current = list.head;
    
    // walk the list node by node
    while (current != nullptr) {
        if (getAgeGroupCode(current->resident.age) == ageGroupCode) {
            count++;
        }
        current = current->next;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Node) * list.size;
    metrics.itemsProcessed = count;
    
    return metrics;
}

// --- list: linear search by transport mode ---
// O(n) time, O(1) space

PerfMetrics searchByModeList(const LinkedList& list, const std::string& mode) {
    auto start = std::chrono::high_resolution_clock::now();
    
    int count = 0;
    Node* current = list.head;
    
    // walk the list node by node
    while (current != nullptr) {
        if (current->resident.modeOfTransport == mode) {
            count++;
        }
        current = current->next;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Node) * list.size;
    metrics.itemsProcessed = count;
    
    return metrics;
}

// --- list: linear search by distance threshold ---
// O(n) time, O(1) space, dailyDistance > threshold

PerfMetrics searchByDistanceThresholdList(const LinkedList& list, double threshold) {
    auto start = std::chrono::high_resolution_clock::now();
    
    int count = 0;
    Node* current = list.head;
    
    // walk the list node by node
    while (current != nullptr) {
        if (current->resident.dailyDistance > threshold) {
            count++;
        }
        current = current->next;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Node) * list.size;
    metrics.itemsProcessed = count;
    
    return metrics;
}
