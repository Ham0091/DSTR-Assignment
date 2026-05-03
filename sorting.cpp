#include "sorting.hpp"
#include <chrono>
#include <iostream>

// --- helper funcs for comparing/swapping ---

// compare by field (true = a before b)
bool compareByAge(const Resident& a, const Resident& b) {
    return a.age < b.age;
}

bool compareByDistance(const Resident& a, const Resident& b) {
    return a.dailyDistance < b.dailyDistance;
}

bool compareByEmission(const Resident& a, const Resident& b) {
    return a.monthlyEmission < b.monthlyEmission;
}

// compare by field number (1=age, 2=emission, 3=distance)
bool compareResidents(const Resident& a, const Resident& b, int field) {
    switch (field) {
        case 1: return compareByAge(a, b);
        case 2: return compareByEmission(a, b);
        case 3: return compareByDistance(a, b);
        default: return compareByAge(a, b);
    }
}

// swap two residents in the array
void swapResidents(Resident& a, Resident& b) {
    Resident temp = a;
    a = b;
    b = temp;
}

// --- bubble sort for array ---
// O(n^2) time, O(1) space (best case O(n) when already sorted)

PerfMetrics sortArrayByAge(ResidentArray& arr) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // bubble sort w/ early-exit if no swaps in a pass
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

// --- quick sort for array ---
// O(n log n) avg, O(n^2) worst, recursion stack O(log n)

// partition for quicksort (returns pivot index)
int partitionArray(ResidentArray& arr, int low, int high, int field) {
    // pick middle element as pivot
    int midIndex = low + (high - low) / 2;
    swapResidents(arr.data[midIndex], arr.data[high]);
    
    Resident& pivot = arr.data[high];
    int i = low - 1;
    
    // partition: < pivot on left, > pivot on right
    for (int j = low; j < high; j++) {
        if (compareResidents(arr.data[j], pivot, field)) {
            i++;
            swapResidents(arr.data[i], arr.data[j]);
        }
    }
    swapResidents(arr.data[i + 1], arr.data[high]);
    return i + 1;
}

// recursive quicksort
void quickSortArrayHelper(ResidentArray& arr, int low, int high, int field) {
    if (low < high) {
        int pi = partitionArray(arr, low, high, field);
        quickSortArrayHelper(arr, low, pi - 1, field);
        quickSortArrayHelper(arr, pi + 1, high, field);
    }
}

// --- insertion sort for array ---
// O(n) best case, O(n^2) worst, in-place

void insertionSortArray(ResidentArray& arr, int field) {
    // build sorted array one element at a time
    for (int i = 1; i < arr.count; i++) {
        Resident key = arr.data[i];
        int j = i - 1;
        
        // shift elements greater than key to the right
        while (j >= 0 && !compareResidents(arr.data[j], key, field)) {
            arr.data[j + 1] = arr.data[j];
            j--;
        }
        // drop key into place
        arr.data[j + 1] = key;
    }
}

// --- flexible array sort (algorithm picker) ---

PerfMetrics sortArrayWithAlgorithm(ResidentArray& arr, int algorithm, int field) {
    auto start = std::chrono::high_resolution_clock::now();
    
    switch (algorithm) {
        case 1:
            // bubble sort w/ early-exit (best case O(n))
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
            // quick sort
            if (arr.count > 1) {
                quickSortArrayHelper(arr, 0, arr.count - 1, field);
            }
            break;
            
        case 3:
            // insertion sort
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

// --- bubble sort for linked list ---
// O(n^2) time, O(1) space, pointer overhead makes it slow

PerfMetrics sortLinkedListByAge(LinkedList& list) {
    auto start = std::chrono::high_resolution_clock::now();
    
    if (!list.head || !list.head->next) {
        // list is empty or a single node
        auto end = std::chrono::high_resolution_clock::now();
        long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        PerfMetrics metrics;
        metrics.executionTimeUs = duration;
        metrics.memoryBytes = sizeof(Node) * list.size;
        metrics.itemsProcessed = list.size;
        return metrics;
    }
    
    // bubble sort on linked list: traverse + swap node data
    bool swapped;
    int pass = 0;
    const int maxPasses = (list.size > 0) ? list.size : 1;
    do {
        swapped = false;
        Node* current = list.head;
        int steps = 0;
        
        while (current && current->next) {
            if (compareByAge(current->next->resident, current->resident)) {
                // swap resident data
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

// --- quick sort for linked list ---
// O(n log n) avg, O(n^2) worst, recursion stack O(log n)
// we partition by relinking nodes

// partition linked list by pivot (returns smaller/larger lists)
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

// recursive quicksort for linked list
Node* quickSortLinkedListHelper(Node* head, int field) {
    if (!head || !head->next) {
        return head;
    }

    // find middle node (pivot) using slow/fast pointers
    Node* prevPivot = nullptr;
    Node* slow = head;
    Node* fast = head;

    while (fast && fast->next) {
        prevPivot = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    // pull pivot out before partitioning
    // without this, all-equal data can recurse forever (ask me how i know)
    Node* pivotNode = slow;
    if (prevPivot) {
        prevPivot->next = pivotNode->next;
    } else {
        head = pivotNode->next;
    }
    pivotNode->next = nullptr;

    // partition remaining nodes (pivot excluded)
    Node* smaller = nullptr;
    Node* larger = nullptr;
    partitionLinkedList(head, pivotNode->resident, smaller, larger, field);

    // recursively sort both sides
    smaller = quickSortLinkedListHelper(smaller, field);
    larger = quickSortLinkedListHelper(larger, field);

    // reconnect: smaller -> pivot -> larger
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

// --- insertion sort for linked list ---
// O(n^2) time, O(1) space, usually fewer swaps than bubble

Node* insertionSortLinkedListHelper(Node* head, int field) {
    if (!head || !head->next) {
        return head;
    }
    
    Node* sorted = nullptr;
    Node* current = head;
    
    while (current) {
        Node* next = current->next;
        
        // find where to insert current node
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

// --- flexible linked list sort (algorithm picker) ---

PerfMetrics sortLinkedListWithAlgorithm(LinkedList& list, int algorithm, int field) {
    auto start = std::chrono::high_resolution_clock::now();
    
    switch (algorithm) {
        case 1: {
            // bubble sort
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
            // quick sort — nodes are relinked, so update tail after
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
            // insertion sort — nodes are relinked, so update tail after
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
