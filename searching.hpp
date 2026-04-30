#ifndef SEARCHING_HPP
#define SEARCHING_HPP

#include "dataStructures.hpp"
#include "sorting.hpp"

// ============================================================
// SECTION 1: Array-Based Search Functions
// ============================================================
// All search functions return PerfMetrics with timing data

// Linear search: Find all residents in specified age group
// Time Complexity: O(n) - must check every element
// ageGroupCode: "6-17", "18-25", "26-45", "46-60", "61+"
PerfMetrics searchByAgeGroupArray(const ResidentArray& arr, const std::string& ageGroupCode);

// Linear search: Find all residents using specified transport mode
// Time Complexity: O(n)
// mode: "Car", "Bus", "Bicycle", "Walking", etc.
PerfMetrics searchByModeArray(const ResidentArray& arr, const std::string& mode);

// Linear search: Find residents with dailyDistance > threshold
// Time Complexity: O(n)
PerfMetrics searchByDistanceThresholdArray(const ResidentArray& arr, double threshold);

// Binary search: Find residents with exact age
// Time Complexity: O(log n) - MUCH FASTER than linear search
// IMPORTANT: Array must be pre-sorted by age field!
// Returns count of residents with the target age
PerfMetrics binarySearchByAgeArray(const ResidentArray& arr, int targetAge);

// ============================================================
// SECTION 2: Linked List Search Functions
// ============================================================
// Pointer traversal required for linked list

// Linear search: Find all residents in age group (linked list)
// Time Complexity: O(n) with pointer overhead
PerfMetrics searchByAgeGroupList(const LinkedList& list, const std::string& ageGroupCode);

// Linear search: Find all residents by mode (linked list)
// Time Complexity: O(n) with pointer overhead
PerfMetrics searchByModeList(const LinkedList& list, const std::string& mode);

// Linear search: Find residents with dailyDistance > threshold (linked list)
// Time Complexity: O(n) with pointer overhead
PerfMetrics searchByDistanceThresholdList(const LinkedList& list, double threshold);

#endif // SEARCHING_HPP
