#ifndef SEARCHING_HPP
#define SEARCHING_HPP

#include "dataStructures.hpp"
#include "sorting.hpp"

// --- array search bits ---
// all searches return PerfMetrics timing info

// linear scan by age group (O(n), nothing fancy)
// ageGroupCode: "6-17", "18-25", "26-45", "46-60", "61+"
PerfMetrics searchByAgeGroupArray(const ResidentArray& arr, const std::string& ageGroupCode);

// linear scan by transport mode (O(n))
// mode: "Car", "Bus", "Bicycle", "Walking", etc.
PerfMetrics searchByModeArray(const ResidentArray& arr, const std::string& mode);

// linear scan for dailyDistance > threshold
PerfMetrics searchByDistanceThresholdArray(const ResidentArray& arr, double threshold);

// binary search by exact age (O(log n), fast if sorted)
// IMPORTANT: array must already be sorted by age
// returns how many match the target age
PerfMetrics binarySearchByAgeArray(const ResidentArray& arr, int targetAge);

// --- linked list search bits ---
// pointer traversal only (no index access)

// linear scan by age group (linked list, pointer overhead)
PerfMetrics searchByAgeGroupList(const LinkedList& list, const std::string& ageGroupCode);

// linear scan by mode (linked list)
PerfMetrics searchByModeList(const LinkedList& list, const std::string& mode);

// linear scan for dailyDistance > threshold (linked list)
PerfMetrics searchByDistanceThresholdList(const LinkedList& list, double threshold);

#endif // searching.hpp guard
