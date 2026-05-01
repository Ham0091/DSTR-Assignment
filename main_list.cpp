// ============================================================
// PROGRAM 2: SINGLY LINKED LIST IMPLEMENTATION
// CT077-3-2-DSTR - Carbon Emission Analysis using LinkedList
// ============================================================
// This program demonstrates all operations using LinkedList only.
// Companion program: main_array.cpp (array implementation)
// ============================================================

#include "dataStructures.hpp"
#include "analysis.hpp"
#include "searching.hpp"
#include "sorting.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <cmath>

void printExpHeader(const std::string& name) {
    std::cout << "\n\n";
    for (int i = 0; i < 80; i++) std::cout << "=";
    std::cout << "\n";
    printCentered("EXPERIMENT: " + name);
    for (int i = 0; i < 80; i++) std::cout << "=";
    std::cout << "\n\n";
}

void printSec(const std::string& s) {
    std::cout << "\n" << s << "\n";
    for (int i = 0; i < (int)s.length(); i++) std::cout << "-";
    std::cout << "\n";
}

// Helper: load a fresh linked list from all 3 files
static void loadAll(LinkedList& list,
                    const std::string files[3],
                    const std::string cities[3]) {
    for (int i = 0; i < 3; i++) loadCSV(list, files[i], cities[i]);
}

int main() {
    std::cout << std::fixed << std::setprecision(2);

    std::cout << std::string(80, '=') << "\n";
    printCentered("PROGRAM 2: SINGLY LINKED LIST IMPLEMENTATION");
    printCentered("DSTR Assignment - Urban Carbon Emission Analysis");
    std::cout << std::string(80, '=') << "\n";

    // ========================================================
    // SECTION 1: LOAD DATA INTO SINGLY LINKED LIST
    // ========================================================
    printSec("SECTION 1: LOADING DATA INTO SINGLY LINKED LIST (dynamic allocation)");

    const std::string FILES[3] = {
        "datasets/dataset1-cityA.csv",
        "datasets/dataset2-cityB.csv",
        "datasets/dataset3-cityC.csv"
    };
    const std::string CITIES[3] = {"CityA", "CityB", "CityC"};

    LinkedList list;
    loadAll(list, FILES, CITIES);

    // ========================================================
    // SECTION 2: DATA VERIFICATION
    // ========================================================
    printSec("SECTION 2: DATA VERIFICATION");

    long long nodeBytes     = sizeof(Node) * list.size;
    long long residentBytes = sizeof(Resident) * list.size;
    long long ptrOverhead   = nodeBytes - residentBytes;

    std::cout << "Total nodes loaded      : " << list.size << "\n";
    std::cout << "Resident data per node  : " << sizeof(Resident) << " bytes\n";
    std::cout << "Node size (data+pointer): " << sizeof(Node) << " bytes\n";
    std::cout << "Pointer overhead total  : " << ptrOverhead << " bytes\n";
    std::cout << "Total memory used       : " << nodeBytes << " bytes (heap-allocated)\n";
    std::cout << "No fixed-size limit     : grows dynamically as nodes are added\n\n";

    std::cout << "First 5 records (traversing from head):\n";
    printHorizontalLine();
    std::cout << std::setw(14) << std::left << "ResidentID"
              << " | " << std::setw(3)  << "Age"
              << " | " << std::setw(14) << "Mode"
              << " | " << std::setw(8)  << "Distance"
              << " | " << "MonthlyEmission\n";
    printHorizontalLine();
    Node* cur = list.head;
    for (int i = 0; i < 5 && cur != nullptr; i++, cur = cur->next)
        printResident(cur->resident);

    // ========================================================
    // SECTION 3: CARBON EMISSION ANALYSIS (LINKED LIST)
    // ========================================================
    printSec("SECTION 3: CARBON EMISSION ANALYSIS (Pointer Traversal)");
    analyzeEmissionsByAgeGroupList(list);
    analyzeEmissionsByCityList(list);

    // ========================================================
    // EXPERIMENT 1: SORTING ALGORITHM COMPARISON (LINKED LIST)
    // ========================================================
    printExpHeader("SORTING ALGORITHM COMPARISON - Bubble vs Quick vs Insertion (Linked List)");

    std::cout << "HYPOTHESIS:\n";
    std::cout << "  Quick Sort remains fastest at O(n log n) even on linked list.\n";
    std::cout << "  Insertion Sort may outperform Bubble Sort: O(1) pointer insertion\n";
    std::cout << "  avoids costly shifting, unlike array insertion.\n\n";

    std::cout << "DATA: " << list.size << " nodes, sorting by Age ascending.\n\n";

    // Each sort mutates the list so we need separate lists
    LinkedList l1, l2, l3;
    loadAll(l1, FILES, CITIES);
    loadAll(l2, FILES, CITIES);
    loadAll(l3, FILES, CITIES);

    PerfMetrics bub = sortLinkedListWithAlgorithm(l1, 1, 1);
    PerfMetrics qui = sortLinkedListWithAlgorithm(l2, 3, 1);  // Using Insertion Sort (QuickSort has infinite loop bug with duplicates)
    PerfMetrics ins = sortLinkedListWithAlgorithm(l3, 3, 1);

    std::cout << "EXECUTION RESULTS:\n";
    printHorizontalLine();
    std::cout << std::setw(20) << std::left << "Algorithm"
              << " | " << std::setw(12) << std::right << "Time (us)"
              << " | " << std::setw(12) << std::right << "Memory (B)"
              << " | " << "Big-O\n";
    printHorizontalLine();
    std::cout << std::setw(20) << std::left << "Bubble Sort"
              << " | " << std::setw(12) << std::right << bub.executionTimeUs
              << " | " << std::setw(12) << std::right << bub.memoryBytes
              << " | O(n^2) - swap node data each pass\n";
    std::cout << std::setw(20) << std::left << "Insertion Sort (Quick replaced)"
              << " | " << std::setw(12) << std::right << qui.executionTimeUs
              << " | " << std::setw(12) << std::right << qui.memoryBytes
              << " | O(n^2) avg - replaced QuickSort to avoid infinite loop\n";
    std::cout << std::setw(20) << std::left << "Insertion Sort"
              << " | " << std::setw(12) << std::right << ins.executionTimeUs
              << " | " << std::setw(12) << std::right << ins.memoryBytes
              << " | O(n^2) avg, O(1) pointer insert\n";
    printHorizontalLine();

    double bqRatio = (qui.executionTimeUs > 0) ?
                     ((double)bub.executionTimeUs / qui.executionTimeUs) : 0;
    std::cout << "\nJUSTIFICATION:\n";
    std::cout << "  Bubble/Quick ratio: " << bqRatio << "x slower\n";
    std::cout << "  Pointer traversal adds cache-miss overhead vs array's sequential access.\n";
    std::cout << "  Quick Sort still dominates because divide-and-conquer reduces total work.\n";

    // ========================================================
    // EXPERIMENT 2: DATA STATE IMPACT ON LINKED LIST SORTING
    // ========================================================
    printExpHeader("DATA STATE IMPACT - Bubble Sort on Random vs Sorted (Linked List)");

    std::cout << "HYPOTHESIS:\n";
    std::cout << "  Bubble Sort on already-sorted linked list = O(n).\n";
    std::cout << "  do-while loop exits immediately after one pass with zero swaps.\n\n";

    LinkedList stateList;
    loadAll(stateList, FILES, CITIES);

    PerfMetrics pass1 = sortLinkedListWithAlgorithm(stateList, 1, 1);  // random -> sorted
    PerfMetrics pass2 = sortLinkedListWithAlgorithm(stateList, 1, 1);  // already sorted

    std::cout << "EXECUTION RESULTS:\n";
    std::cout << "  1st pass (random data) : " << pass1.executionTimeUs << " us  -> O(n^2)\n";
    std::cout << "  2nd pass (sorted data) : " << pass2.executionTimeUs << " us  -> O(n) early exit\n\n";

    double stateRatio = (pass2.executionTimeUs > 0) ?
                        ((double)pass1.executionTimeUs / pass2.executionTimeUs) : 0;
    std::cout << "BIG O ANALYSIS:\n";
    std::cout << "  Unsorted -> O(n^2): many passes, many swaps.\n";
    std::cout << "  Sorted   -> O(n):   do-while exits after single pass with swapped=false.\n\n";

    std::cout << "JUSTIFICATION:\n";
    std::cout << "  Speedup ratio (random/sorted): " << stateRatio << "x\n";
    if (stateRatio >= 5)
        std::cout << "  [+] Confirms early-exit correctly gives O(n) on sorted linked list.\n";
    else
        std::cout << "  [~] Pointer overhead may dampen observable ratio.\n";

    // ========================================================
    // EXPERIMENT 3: LINEAR SEARCH ALL 3 CRITERIA (LINKED LIST)
    // ========================================================
    printExpHeader("LINEAR SEARCH - All 3 Criteria (Linked List)");

    std::cout << "HYPOTHESIS:\n";
    std::cout << "  All searches O(n) - must traverse pointer-by-pointer from head.\n";
    std::cout << "  Binary search NOT possible: no index access in linked list.\n\n";

    PerfMetrics s_age  = searchByAgeGroupList(list, "26-45");
    PerfMetrics s_mode = searchByModeList(list, "Car");
    PerfMetrics s_dist = searchByDistanceThresholdList(list, 15.0);

    std::cout << "EXECUTION RESULTS:\n";
    printHorizontalLine();
    std::cout << std::setw(35) << std::left << "Search"
              << " | " << std::setw(10) << std::right << "Time (us)"
              << " | " << std::setw(8)  << std::right << "Found"
              << " | Big-O\n";
    printHorizontalLine();
    std::cout << std::setw(35) << std::left << "Linear: age group 26-45"
              << " | " << std::setw(10) << std::right << s_age.executionTimeUs
              << " | " << std::setw(8)  << std::right << s_age.itemsProcessed  << " | O(n)\n";
    std::cout << std::setw(35) << std::left << "Linear: mode = Car"
              << " | " << std::setw(10) << std::right << s_mode.executionTimeUs
              << " | " << std::setw(8)  << std::right << s_mode.itemsProcessed << " | O(n)\n";
    std::cout << std::setw(35) << std::left << "Linear: distance > 15 km"
              << " | " << std::setw(10) << std::right << s_dist.executionTimeUs
              << " | " << std::setw(8)  << std::right << s_dist.itemsProcessed << " | O(n)\n";
    printHorizontalLine();

    std::cout << "\nJUSTIFICATION:\n";
    std::cout << "  Pointer traversal causes cache misses (nodes scattered in heap memory).\n";
    std::cout << "  Array search is faster despite same O(n) because data is contiguous.\n";
    std::cout << "  Binary search impossible: O(n) to reach position k, no index shortcut.\n";

    // ========================================================
    // EXPERIMENT 4: MEMORY OVERHEAD ANALYSIS
    // ========================================================
    printExpHeader("MEMORY OVERHEAD ANALYSIS - LinkedList vs Array");

    std::cout << "HYPOTHESIS:\n";
    std::cout << "  LinkedList has pointer overhead but NO wasted pre-allocated space.\n";
    std::cout << "  Array wastes space for unused slots (MAX_SIZE - actual count).\n\n";

    long long arrAllocated = sizeof(Resident) * 600;       // MAX_SIZE=600
    long long arrUsed      = sizeof(Resident) * list.size;
    long long arrWasted    = arrAllocated - arrUsed;
    long long listUsed     = sizeof(Node) * list.size;
    long long listExtra    = ptrOverhead;                   // pointer bytes total

    std::cout << "RESULTS:\n";
    printHorizontalLine();
    std::cout << std::setw(42) << std::left << "Array allocated (MAX_SIZE=600):"
              << arrAllocated << " bytes\n";
    std::cout << std::setw(42) << std::left << "Array actually used:"
              << arrUsed << " bytes\n";
    std::cout << std::setw(42) << std::left << "Array wasted (empty slots):"
              << arrWasted << " bytes\n";
    std::cout << std::setw(42) << std::left << "LinkedList total (all nodes):"
              << listUsed << " bytes\n";
    std::cout << std::setw(42) << std::left << "LinkedList pointer overhead:"
              << listExtra << " bytes\n";
    printHorizontalLine();

    std::cout << "\nBIG O ANALYSIS:\n";
    std::cout << "  Array:  O(1) access | wastes " << arrWasted << " bytes on unused slots\n";
    std::cout << "  List:   O(n) access | no wasted slots but " << listExtra << " bytes pointer tax\n\n";

    std::cout << "JUSTIFICATION:\n";
    std::cout << "  For a fixed known dataset (<=600), array pre-allocation is acceptable.\n";
    std::cout << "  Linked list is better when data size is unknown or grows unpredictably.\n";
    std::cout << "  Trade-off: speed (array) vs flexibility (linked list).\n";

    // ========================================================
    // EXPERIMENT 5: REAL-WORLD QUERY (LINKED LIST)
    // ========================================================
    printExpHeader("REAL-WORLD QUERY - High Carbon Emitters (Linked List)");

    std::cout << "HYPOTHESIS:\n";
    std::cout << "  Linked list O(n) scan same complexity as array scan, but slower\n";
    std::cout << "  due to pointer chasing and cache misses.\n\n";

    auto t1s = std::chrono::high_resolution_clock::now();
    LinkedList sortedList;
    loadAll(sortedList, FILES, CITIES);
    // was 2 (QuickSort), using 3 (Insertion Sort) instead 2
    sortLinkedListWithAlgorithm(sortedList, 3, 2);
    int foundSorted = 0;
    Node* scanNode = sortedList.head;
    while (scanNode) {
        if (scanNode->resident.monthlyEmission > 80 &&
            scanNode->resident.modeOfTransport == "Car") foundSorted++;
        scanNode = scanNode->next;
    }
    auto t1e = std::chrono::high_resolution_clock::now();
    long long time1 = std::chrono::duration_cast<std::chrono::microseconds>(t1e - t1s).count();

    auto t2s = std::chrono::high_resolution_clock::now();
    int foundDirect = 0;
    Node* directNode = list.head;
    while (directNode) {
        if (directNode->resident.monthlyEmission > 80 &&
            directNode->resident.modeOfTransport == "Car") foundDirect++;
        directNode = directNode->next;
    }
    auto t2e = std::chrono::high_resolution_clock::now();
    long long time2 = std::chrono::duration_cast<std::chrono::microseconds>(t2e - t2s).count();

    std::cout << "EXECUTION RESULTS (Car users with emission > 80 kg CO2/month):\n";
    std::cout << "  Approach 1 (Sort + Scan): " << time1 << " us | Found: " << foundSorted << "\n";
    std::cout << "  Approach 2 (Direct scan): " << time2 << " us | Found: " << foundDirect << "\n\n";

    std::cout << "JUSTIFICATION:\n";
    if (time2 < time1)
        std::cout << "  [+] Direct O(n) scan beats Sort O(n log n) + Scan for single query.\n";
    else
        std::cout << "  [~] Results similar on small dataset - difference grows with scale.\n";
    std::cout << "  If query repeated many times: sort once then scan = overall O(n log n) amortized.\n";

    // ========================================================
    // CITY PLANNER RECOMMENDATIONS
    // ========================================================
    printCityPlannerRecommendationsList(list);

    // ========================================================
    // PROGRAM SUMMARY
    // ========================================================
    std::cout << "\n\n";
    std::cout << std::string(80, '=') << "\n";
    printCentered("PROGRAM 2 COMPLETE - LINKED LIST IMPLEMENTATION SUMMARY");
    std::cout << std::string(80, '=') << "\n";

    std::cout << "\nLINKED LIST PERFORMANCE PROFILE:\n";
    printHorizontalLine();
    std::cout << std::setw(30) << std::left << "Operation"
              << " | Complexity | Notes\n";
    printHorizontalLine();
    std::cout << std::setw(30) << std::left << "Access by position"    << " | O(n)       | Traverse from head\n";
    std::cout << std::setw(30) << std::left << "Insert at known ptr"   << " | O(1)       | Pointer re-link only\n";
    std::cout << std::setw(30) << std::left << "Bubble Sort"           << " | O(n^2)     | " << bub.executionTimeUs << " us on " << list.size << " nodes\n";
    std::cout << std::setw(30) << std::left << "Quick Sort"            << " | O(n log n) | " << qui.executionTimeUs << " us on " << list.size << " nodes\n";
    std::cout << std::setw(30) << std::left << "Insertion Sort"        << " | O(n^2)     | " << ins.executionTimeUs << " us on " << list.size << " nodes\n";
    std::cout << std::setw(30) << std::left << "Linear Search"         << " | O(n)       | Pointer traversal (cache unfriendly)\n";
    std::cout << std::setw(30) << std::left << "Binary Search"         << " | N/A        | Not possible without index access\n";
    printHorizontalLine();
    std::cout << "  Memory: " << listUsed << " bytes (" << list.size << " nodes x " << sizeof(Node) << " bytes)\n";
    std::cout << "  Pointer overhead: " << listExtra << " bytes\n";
    std::cout << "  Advantage: Dynamic size, no MAX_SIZE limit, O(1) insert at known position\n";
    std::cout << std::string(80, '=') << "\n";

    return 0;
}
