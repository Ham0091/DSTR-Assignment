#include "sorting.hpp"
#include <chrono>
#include <iostream>

// ============================================================
// SECTION 1: Helper Functions for Array Comparison and Swapping
// ============================================================

// Helper: Compare two residents by a specific field
// Returns: true if a should come before b (ascending order)
bool compareByAge(const Resident& a, const Resident& b) {
    return a.age < b.age;
}

bool compareByDistance(const Resident& a, const Resident& b) {
    return a.dailyDistance < b.dailyDistance;
}

bool compareByEmission(const Resident& a, const Resident& b) {
    return a.monthlyEmission < b.monthlyEmission;
}

// Helper: Compare by field number
// field: 1=age, 2=emission, 3=distance
bool compareResidents(const Resident& a, const Resident& b, int field) {
    switch (field) {
        case 1: return compareByAge(a, b);
        case 2: return compareByEmission(a, b);
        case 3: return compareByDistance(a, b);
        default: return compareByAge(a, b);
    }
}

// Helper: Swap two residents in array
void swapResidents(Resident& a, Resident& b) {
    Resident temp = a;
    a = b;
    b = temp;
}

// ============================================================
// SECTION 2: BUBBLE SORT FOR ARRAY
// ============================================================
// Time Complexity: O(n²) - nested loops
// Space Complexity: O(1) - in-place sorting
// Best case: O(n) - when array is already sorted
// Worst case: O(n²) - when array is reverse sorted

PerfMetrics sortArrayByAge(ResidentArray& arr) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Bubble sort with early-exit: if no swaps in a full pass, data is sorted (O(n) best case)
    for (int i = 0; i < arr.count - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < arr.count - 1 - i; j++) {
            if (!compareByAge(arr.data[j], arr.data[j + 1])) {
                swapResidents(arr.data[j], arr.data[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Resident) * arr.count;
    metrics.itemsProcessed = arr.count;
    
    return metrics;
}

PerfMetrics sortArrayByDistance(ResidentArray& arr) {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < arr.count - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < arr.count - 1 - i; j++) {
            if (!compareByDistance(arr.data[j], arr.data[j + 1])) {
                swapResidents(arr.data[j], arr.data[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Resident) * arr.count;
    metrics.itemsProcessed = arr.count;
    
    return metrics;
}

PerfMetrics sortArrayByEmission(ResidentArray& arr) {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < arr.count - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < arr.count - 1 - i; j++) {
            if (!compareByEmission(arr.data[j], arr.data[j + 1])) {
                swapResidents(arr.data[j], arr.data[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Resident) * arr.count;
    metrics.itemsProcessed = arr.count;
    
    return metrics;
}

// ============================================================
// SECTION 3: QUICK SORT FOR ARRAY
// ============================================================
// Time Complexity: O(n log n) average, O(n²) worst case
// Space Complexity: O(log n) for recursion stack
// Generally FASTER than bubble sort on random data

// Helper: Partition for quicksort
// Returns the index of the pivot after partitioning
int partitionArray(ResidentArray& arr, int low, int high, int field) {
    // Choose middle element as pivot
    int midIndex = low + (high - low) / 2;
    swapResidents(arr.data[midIndex], arr.data[high]);
    
    Resident& pivot = arr.data[high];
    int i = low - 1;
    
    // Partition: elements < pivot on left, > pivot on right
    for (int j = low; j < high; j++) {
        if (compareResidents(arr.data[j], pivot, field)) {
            i++;
            swapResidents(arr.data[i], arr.data[j]);
        }
    }
    swapResidents(arr.data[i + 1], arr.data[high]);
    return i + 1;
}

// Helper: Recursive quicksort
void quickSortArrayHelper(ResidentArray& arr, int low, int high, int field) {
    if (low < high) {
        int pi = partitionArray(arr, low, high, field);
        quickSortArrayHelper(arr, low, pi - 1, field);
        quickSortArrayHelper(arr, pi + 1, high, field);
    }
}

// ============================================================
// SECTION 4: INSERTION SORT FOR ARRAY
// ============================================================
// Time Complexity: O(n) best case (already sorted), O(n²) worst case
// Space Complexity: O(1) - in-place
// Often faster than bubble sort due to fewer comparisons

void insertionSortArray(ResidentArray& arr, int field) {
    // Build sorted array one element at a time
    for (int i = 1; i < arr.count; i++) {
        Resident key = arr.data[i];
        int j = i - 1;
        
        // Shift elements greater than key one position right
        while (j >= 0 && !compareResidents(arr.data[j], key, field)) {
            arr.data[j + 1] = arr.data[j];
            j--;
        }
        // Insert key at correct position
        arr.data[j + 1] = key;
    }
}

// ============================================================
// SECTION 5: FLEXIBLE ARRAY SORTING WITH ALGORITHM SELECTION
// ============================================================

PerfMetrics sortArrayWithAlgorithm(ResidentArray& arr, int algorithm, int field) {
    auto start = std::chrono::high_resolution_clock::now();
    
    switch (algorithm) {
        case 1:
            // Bubble sort with early-exit for O(n) best case on sorted data
            for (int i = 0; i < arr.count - 1; i++) {
                bool swapped = false;
                for (int j = 0; j < arr.count - 1 - i; j++) {
                    if (!compareResidents(arr.data[j], arr.data[j + 1], field)) {
                        swapResidents(arr.data[j], arr.data[j + 1]);
                        swapped = true;
                    }
                }
                if (!swapped) break;
            }
            break;
            
        case 2:
            // Quick sort
            if (arr.count > 1) {
                quickSortArrayHelper(arr, 0, arr.count - 1, field);
            }
            break;
            
        case 3:
            // Insertion sort
            insertionSortArray(arr, field);
            break;
            
        default:
            std::cerr << "Unknown algorithm: " << algorithm << std::endl;
            break;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Resident) * arr.count;
    metrics.itemsProcessed = arr.count;
    
    return metrics;
}

// ============================================================
// SECTION 6: BUBBLE SORT FOR LINKED LIST
// ============================================================
// Time Complexity: O(n²) - nested loops with pointer traversal
// Space Complexity: O(1) - only node pointers, no extra space
// Much slower than array version due to pointer overhead

PerfMetrics sortLinkedListByAge(LinkedList& list) {
    auto start = std::chrono::high_resolution_clock::now();
    
    if (!list.head || !list.head->next) {
        // List is empty or has only one element
        auto end = std::chrono::high_resolution_clock::now();
        long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        PerfMetrics metrics;
        metrics.executionTimeUs = duration;
        metrics.memoryBytes = sizeof(Node) * list.size;
        metrics.itemsProcessed = list.size;
        return metrics;
    }
    
    // Bubble sort on linked list: traverse and swap node data
    bool swapped;
    int pass = 0;
    const int maxPasses = (list.size > 0) ? list.size : 1;
    do {
        swapped = false;
        Node* current = list.head;
        int steps = 0;
        
        while (current && current->next) {
            if (compareByAge(current->next->resident, current->resident)) {
                // Swap resident data
                Resident temp = current->resident;
                current->resident = current->next->resident;
                current->next->resident = temp;
                swapped = true;
            }
            current = current->next;
            if (++steps > list.size) {
                std::cerr << "Warning: possible cycle detected in linked list during bubble sort." << std::endl;
                swapped = false;
                break;
            }
        }
    } while (swapped && ++pass < maxPasses);
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Node) * list.size;
    metrics.itemsProcessed = list.size;
    
    return metrics;
}

PerfMetrics sortLinkedListByDistance(LinkedList& list) {
    auto start = std::chrono::high_resolution_clock::now();
    
    if (!list.head || !list.head->next) {
        auto end = std::chrono::high_resolution_clock::now();
        long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        PerfMetrics metrics;
        metrics.executionTimeUs = duration;
        metrics.memoryBytes = sizeof(Node) * list.size;
        metrics.itemsProcessed = list.size;
        return metrics;
    }
    
    bool swapped;
    int pass = 0;
    const int maxPasses = (list.size > 0) ? list.size : 1;
    do {
        swapped = false;
        Node* current = list.head;
        int steps = 0;
        
        while (current && current->next) {
            if (compareByDistance(current->next->resident, current->resident)) {
                Resident temp = current->resident;
                current->resident = current->next->resident;
                current->next->resident = temp;
                swapped = true;
            }
            current = current->next;
            if (++steps > list.size) {
                std::cerr << "Warning: possible cycle detected in linked list during bubble sort." << std::endl;
                swapped = false;
                break;
            }
        }
    } while (swapped && ++pass < maxPasses);
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Node) * list.size;
    metrics.itemsProcessed = list.size;
    
    return metrics;
}

PerfMetrics sortLinkedListByEmission(LinkedList& list) {
    auto start = std::chrono::high_resolution_clock::now();
    
    if (!list.head || !list.head->next) {
        auto end = std::chrono::high_resolution_clock::now();
        long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        PerfMetrics metrics;
        metrics.executionTimeUs = duration;
        metrics.memoryBytes = sizeof(Node) * list.size;
        metrics.itemsProcessed = list.size;
        return metrics;
    }
    
    bool swapped;
    int pass = 0;
    const int maxPasses = (list.size > 0) ? list.size : 1;
    do {
        swapped = false;
        Node* current = list.head;
        int steps = 0;
        
        while (current && current->next) {
            if (compareByEmission(current->next->resident, current->resident)) {
                Resident temp = current->resident;
                current->resident = current->next->resident;
                current->next->resident = temp;
                swapped = true;
            }
            current = current->next;
            if (++steps > list.size) {
                std::cerr << "Warning: possible cycle detected in linked list during bubble sort." << std::endl;
                swapped = false;
                break;
            }
        }
    } while (swapped && ++pass < maxPasses);
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Node) * list.size;
    metrics.itemsProcessed = list.size;
    
    return metrics;
}

// ============================================================
// SECTION 7: QUICK SORT FOR LINKED LIST
// ============================================================
// Time Complexity: O(n log n) average, O(n²) worst case
// Space Complexity: O(log n) for recursion stack
// Partition linked list by creating new lists

// Helper: Partition linked list by pivot
// Returns pair of (smaller list, larger list)
void partitionLinkedList(Node* head, Resident pivot, Node*& smaller, Node*& larger, int field) {
    smaller = nullptr;
    larger = nullptr;
    Node* smallerTail = nullptr;
    Node* largerTail = nullptr;
    
    Node* current = head;
    while (current) {
        Node* next = current->next;
        current->next = nullptr;
        
        if (compareResidents(current->resident, pivot, field)) {
            if (!smaller) {
                smaller = current;
                smallerTail = current;
            } else {
                smallerTail->next = current;
                smallerTail = current;
            }
        } else {
            if (!larger) {
                larger = current;
                largerTail = current;
            } else {
                largerTail->next = current;
                largerTail = current;
            }
        }
        current = next;
    }
}

// Helper: Recursive quicksort for linked list
Node* quickSortLinkedListHelper(Node* head, int field) {
    if (!head || !head->next) {
        return head;
    }

    // Find middle node (pivot) and its predecessor using slow/fast pointers
    Node* prevPivot = nullptr;
    Node* slow = head;
    Node* fast = head;

    while (fast && fast->next) {
        prevPivot = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    // Extract pivot node from the list before partitioning.
    // Without this, if all elements are equal the pivot ends up in
    // 'larger' on every recursive call, causing infinite recursion.
    Node* pivotNode = slow;
    if (prevPivot) {
        prevPivot->next = pivotNode->next;
    } else {
        head = pivotNode->next;
    }
    pivotNode->next = nullptr;

    // Partition remaining nodes (pivot excluded)
    Node* smaller = nullptr;
    Node* larger = nullptr;
    partitionLinkedList(head, pivotNode->resident, smaller, larger, field);

    // Recursively sort both partitions
    smaller = quickSortLinkedListHelper(smaller, field);
    larger = quickSortLinkedListHelper(larger, field);

    // Reconnect: smaller -> pivot -> larger
    pivotNode->next = larger;

    if (!smaller) {
        return pivotNode;
    }

    Node* tail = smaller;
    while (tail->next) {
        tail = tail->next;
    }
    tail->next = pivotNode;

    return smaller;
}

// ============================================================
// SECTION 8: INSERTION SORT FOR LINKED LIST
// ============================================================
// Time Complexity: O(n²) but with fewer swaps than bubble sort
// Space Complexity: O(1)
// More efficient for linked list since insertion is O(1) with pointer

Node* insertionSortLinkedListHelper(Node* head, int field) {
    if (!head || !head->next) {
        return head;
    }
    
    Node* sorted = nullptr;
    Node* current = head;
    
    while (current) {
        Node* next = current->next;
        
        // Find position to insert current node
        if (!sorted || compareResidents(current->resident, sorted->resident, field)) {
            current->next = sorted;
            sorted = current;
        } else {
            Node* pos = sorted;
            while (pos->next && !compareResidents(current->resident, pos->next->resident, field)) {
                pos = pos->next;
            }
            current->next = pos->next;
            pos->next = current;
        }
        
        current = next;
    }
    
    return sorted;
}

// ============================================================
// SECTION 9: FLEXIBLE LINKED LIST SORTING WITH ALGORITHM SELECTION
// ============================================================

PerfMetrics sortLinkedListWithAlgorithm(LinkedList& list, int algorithm, int field) {
    auto start = std::chrono::high_resolution_clock::now();
    
    switch (algorithm) {
        case 1: {
            // Bubble sort
            if (!list.head || !list.head->next) break;
            
            bool swapped;
            int pass = 0;
            const int maxPasses = (list.size > 0) ? list.size : 1;
            do {
                swapped = false;
                Node* current = list.head;
                int steps = 0;
                
                while (current && current->next) {
                    if (compareResidents(current->next->resident, current->resident, field)) {
                        Resident temp = current->resident;
                        current->resident = current->next->resident;
                        current->next->resident = temp;
                        swapped = true;
                    }
                    current = current->next;
                    if (++steps > list.size) {
                        std::cerr << "Warning: possible cycle detected in linked list during bubble sort." << std::endl;
                        swapped = false;
                        break;
                    }
                }
            } while (swapped && ++pass < maxPasses);
            break;
        }
        
        case 2: {
            // Quick sort — nodes are relinked, so update tail afterwards
            list.head = quickSortLinkedListHelper(list.head, field);
            if (list.head) {
                Node* t = list.head;
                while (t->next) t = t->next;
                list.tail = t;
            } else {
                list.tail = nullptr;
            }
            break;
        }

        case 3: {
            // Insertion sort — nodes are relinked, so update tail afterwards
            list.head = insertionSortLinkedListHelper(list.head, field);
            if (list.head) {
                Node* t = list.head;
                while (t->next) t = t->next;
                list.tail = t;
            } else {
                list.tail = nullptr;
            }
            break;
        }
            
        default:
            std::cerr << "Unknown algorithm: " << algorithm << std::endl;
            break;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(Node) * list.size;
    metrics.itemsProcessed = list.size;
    
    return metrics;
}
