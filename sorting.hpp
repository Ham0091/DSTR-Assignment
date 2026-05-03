#ifndef SORTING_HPP
#define SORTING_HPP

#include "dataStructures.hpp"
#include <chrono>

// --- perf metrics ---
// capture timing + memory for the sort runs
// executionTimeUs: time in microseconds
// memoryBytes: total memory used by the container
// itemsProcessed: number of residents sorted
struct PerfMetrics {
    long long executionTimeUs;     // execution time in microseconds
    long long memoryBytes;         // memory used in bytes
    int itemsProcessed;            // number of items processed
};

// --- array sorting ---
// sort the ResidentArray and return PerfMetrics

// sort by age (ascending) - bubble sort
PerfMetrics sortArrayByAge(ResidentArray& arr);

// sort by daily distance (ascending) - bubble sort
PerfMetrics sortArrayByDistance(ResidentArray& arr);

// sort by monthly emission (ascending) - bubble sort
PerfMetrics sortArrayByEmission(ResidentArray& arr);

// flexible array sort w/ algorithm selection
// algorithm: 1=Bubble, 2=Quick, 3=Insertion
// field: 1=age, 2=emission, 3=distance
PerfMetrics sortArrayWithAlgorithm(ResidentArray& arr, int algorithm, int field);

// --- linked list sorting ---
// sort the LinkedList and return PerfMetrics

// sort list by age (ascending) - bubble sort
PerfMetrics sortLinkedListByAge(LinkedList& list);

// sort list by daily distance (ascending) - bubble sort
PerfMetrics sortLinkedListByDistance(LinkedList& list);

// sort list by monthly emission (ascending) - bubble sort
PerfMetrics sortLinkedListByEmission(LinkedList& list);

// flexible list sort w/ algorithm selection
// algorithm: 1=Bubble, 2=Quick, 3=Insertion
// field: 1=age, 2=emission, 3=distance
PerfMetrics sortLinkedListWithAlgorithm(LinkedList& list, int algorithm, int field);

#endif // sorting.hpp guard
