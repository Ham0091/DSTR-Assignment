#ifndef SORTING_HPP
#define SORTING_HPP

#include "dataStructures.hpp"
#include <chrono>

// ============================================================
// SECTION 1: Performance Metrics Struct
// ============================================================
// Records performance data for sorting operations.
// executionTimeUs: Time taken in microseconds (for precision)
// memoryBytes: Total memory used by the data structure
// itemsProcessed: Number of residents sorted
struct PerfMetrics {
    long long executionTimeUs;     // execution time in microseconds
    long long memoryBytes;         // memory used in bytes
    int itemsProcessed;            // number of items processed
};

// ============================================================
// SECTION 2: Array-Based Sorting Functions
// ============================================================
// These functions sort the ResidentArray and return performance metrics.

// Sort array by age (ascending) - uses bubble sort
PerfMetrics sortArrayByAge(ResidentArray& arr);

// Sort array by daily distance (ascending) - uses bubble sort
PerfMetrics sortArrayByDistance(ResidentArray& arr);

// Sort array by monthly emission (ascending) - uses bubble sort
PerfMetrics sortArrayByEmission(ResidentArray& arr);

// Flexible array sorting with algorithm selection
// algorithm: 1=Bubble, 2=Quick, 3=Insertion
// field: 1=age, 2=emission, 3=distance
PerfMetrics sortArrayWithAlgorithm(ResidentArray& arr, int algorithm, int field);

// ============================================================
// SECTION 3: Linked List Sorting Functions
// ============================================================
// These functions sort the LinkedList and return performance metrics.

// Sort linked list by age (ascending) - uses bubble sort
PerfMetrics sortLinkedListByAge(LinkedList& list);

// Sort linked list by daily distance (ascending) - uses bubble sort
PerfMetrics sortLinkedListByDistance(LinkedList& list);

// Sort linked list by monthly emission (ascending) - uses bubble sort
PerfMetrics sortLinkedListByEmission(LinkedList& list);

// Flexible linked list sorting with algorithm selection
// algorithm: 1=Bubble, 2=Quick, 3=Insertion
// field: 1=age, 2=emission, 3=distance
PerfMetrics sortLinkedListWithAlgorithm(LinkedList& list, int algorithm, int field);

#endif // SORTING_HPP
