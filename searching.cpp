#include "searching.hpp"
#include "analysis.hpp"
#include <chrono>
#include <iostream>

// ============================================================
// SECTION 1: LINEAR SEARCH FOR ARRAY BY AGE GROUP
// ============================================================
// Time Complexity: O(n) - must check every element
// Space Complexity: O(1)

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

// ============================================================
// SECTION 2: LINEAR SEARCH FOR ARRAY BY TRANSPORT MODE
// ============================================================
// Time Complexity: O(n)
// Space Complexity: O(1)

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

// ============================================================
// SECTION 3: LINEAR SEARCH FOR ARRAY BY DISTANCE THRESHOLD
// ============================================================
// Time Complexity: O(n)
// Space Complexity: O(1)
// Finds residents with dailyDistance > threshold

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

// ============================================================
// SECTION 4: BINARY SEARCH FOR ARRAY BY AGE
// ============================================================
// Time Complexity: O(log n) - MUCH FASTER than linear search
// Space Complexity: O(1)
//
// IMPORTANT NOTES:
// - Array MUST be pre-sorted by age field (ascending)
// - Each comparison eliminates half of remaining elements
// - Example: 500 elements → ~9 comparisons max vs 500 for linear
// - Works by comparing target age to middle element:
//   * If equal: found
//   * If target < middle: search left half
//   * If target > middle: search right half

PerfMetrics binarySearchByAgeArray(const ResidentArray& arr, int targetAge) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Binary search assumes array is sorted by age
    int left = 0;
    int right = arr.count - 1;
    int count = 0;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (arr.data[mid].age == targetAge) {
            // Found a match, count all residents with this age
            count = 1;
            
            // Search left to find first occurrence
            int leftIdx = mid - 1;
            while (leftIdx >= 0 && arr.data[leftIdx].age == targetAge) {
                count++;
                leftIdx--;
            }
            
            // Search right to find last occurrence
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

// ============================================================
// SECTION 5: LINEAR SEARCH FOR LINKED LIST BY AGE GROUP
// ============================================================
// Time Complexity: O(n) with pointer traversal overhead
// Space Complexity: O(1)
// Must use pointers to traverse the linked list

PerfMetrics searchByAgeGroupList(const LinkedList& list, const std::string& ageGroupCode) {
    auto start = std::chrono::high_resolution_clock::now();
    
    int count = 0;
    Node* current = list.head;
    
    // Traverse linked list using pointers
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

// ============================================================
// SECTION 6: LINEAR SEARCH FOR LINKED LIST BY TRANSPORT MODE
// ============================================================
// Time Complexity: O(n) with pointer overhead
// Space Complexity: O(1)

PerfMetrics searchByModeList(const LinkedList& list, const std::string& mode) {
    auto start = std::chrono::high_resolution_clock::now();
    
    int count = 0;
    Node* current = list.head;
    
    // Traverse linked list using pointers
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

// ============================================================
// SECTION 7: LINEAR SEARCH FOR LINKED LIST BY DISTANCE THRESHOLD
// ============================================================
// Time Complexity: O(n) with pointer overhead
// Space Complexity: O(1)
// Finds residents with dailyDistance > threshold

PerfMetrics searchByDistanceThresholdList(const LinkedList& list, double threshold) {
    auto start = std::chrono::high_resolution_clock::now();
    
    int count = 0;
    Node* current = list.head;
    
    // Traverse linked list using pointers
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
