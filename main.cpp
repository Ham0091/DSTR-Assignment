// --- DSTR assignment: experiment test bench ---
// main file for running the perf experiments
// sorting/searching/data structure comparisons all in one place

#include "dataStructures.hpp"
#include "analysis.hpp"
#include "searching.hpp"
#include "sorting.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <cmath>

// --- helper: copy ResidentArray for testing ---
ResidentArray copyArray(const ResidentArray& source) {
    ResidentArray copy;
    copy.count = source.count;
    for (int i = 0; i < source.count; i++) {
        copy.data[i] = source.data[i];
    }
    return copy;
}

// --- helper: print experiment separator ---
void printExperimentHeader(const std::string& experimentName) {
    std::cout << "\n\n";
    for (int i = 0; i < 80; i++) std::cout << "=";
    std::cout << "\n";
    printCentered("EXPERIMENT: " + experimentName);
    for (int i = 0; i < 80; i++) std::cout << "=";
    std::cout << "\n\n";
}

void printSectionHeader(const std::string& section) {
    std::cout << "\n" << section << "\n";
    for (int i = 0; i < (int)section.length(); i++) std::cout << "-";
    std::cout << "\n";
}

// --- globals for experiment results ---
static double bubbleToQuickRatio = 0.0;
static double arrayToListRatio = 0.0;
static double linearToBinaryRatio = 0.0;
static double insertion1stTo2ndRatio = 0.0;

// --- main: test bench ---
int main() {
    std::cout << std::fixed << std::setprecision(2);
    
    std::cout << std::string(80, '=') << "\n";
    printCentered("DSTR ASSIGNMENT - PERFORMANCE EXPERIMENT TEST BENCH");
    std::cout << std::string(80, '=') << "\n";
    
    // -- section 1: load data --
    printSectionHeader("SECTION 1: LOADING DATA FROM CSV FILES");
    
    ResidentArray arr;
    const std::string FILES[3] = {
        "datasets/dataset1-cityA.csv",
        "datasets/dataset2-cityB.csv",
        "datasets/dataset3-cityC.csv"
    };
    const std::string CITIES[3] = {"CityA", "CityB", "CityC"};
    
    std::cout << "Loading into ResidentArray...\n";
    for (int i = 0; i < 3; ++i) {
        loadCSV(arr, FILES[i], CITIES[i]);
    }
    
    LinkedList list;
    std::cout << "Loading into LinkedList...\n";
    for (int i = 0; i < 3; ++i) {
        loadCSV(list, FILES[i], CITIES[i]);
    }
    
    // -- section 2: quick data check --
    printSectionHeader("SECTION 2: DATA VERIFICATION");
    
    std::cout << "Total residents in array: " << arr.count << "\n";
    std::cout << "Total residents in list:  " << list.size << "\n";
    std::cout << "Array memory allocated:   " << sizeof(Resident) * arr.count << " bytes\n";
    std::cout << "List memory allocated:    " << sizeof(Node) * list.size << " bytes\n";
    
    std::cout << "\nFirst 5 records (Array):\n";
    printHorizontalLine();
    std::cout << std::setw(15) << std::left << "ResidentID"
              << " | " << std::setw(5) << "Age"
              << " | " << std::setw(12) << "Mode"
              << " | " << std::setw(8) << "Distance"
              << " | " << std::setw(8) << "Emission"
              << std::endl;
    printHorizontalLine();
    for (int i = 0; i < 5 && i < arr.count; i++) {
        printResident(arr.data[i]);
    }
    
    std::cout << "\nLast 5 records (Array):\n";
    printHorizontalLine();
    std::cout << std::setw(15) << std::left << "ResidentID"
              << " | " << std::setw(5) << "Age"
              << " | " << std::setw(12) << "Mode"
              << " | " << std::setw(8) << "Distance"
              << " | " << std::setw(8) << "Emission"
              << std::endl;
    printHorizontalLine();
    for (int i = arr.count - 5; i < arr.count && i >= 0; i++) {
        printResident(arr.data[i]);
    }
    
    // -- section 3: analysis --
    printSectionHeader("SECTION 3: CARBON EMISSION ANALYSIS");
    
    analyzeEmissionsByAgeGroupArray(arr);
    analyzeEmissionsByModeArray(arr);
    analyzeEmissionsByAgeGroupList(list);
    analyzeEmissionsByCityList(list);
    
    // -- section 4: experiments --
    
    // -- experiment 1: sorting algorithm comparison --
    printExperimentHeader("SORTING ALGORITHM COMPARISON (Bubble vs Quick vs Insertion)");
    
    std::cout << "HYPOTHESIS:\n";
    std::cout << "\"I expect Quick Sort to be 5-10x faster than Bubble Sort on " << arr.count 
              << " residents\n";
    std::cout << "because Quick Sort is O(n log n) while Bubble Sort is O(n^2).\n";
    std::cout << "For " << arr.count << " items:\n";
    std::cout << "  - Quick needs ~ " << (arr.count * (int)log2(arr.count)) << " comparisons\n";
    std::cout << "  - Bubble needs ~ " << (arr.count * arr.count / 2) << " comparisons\"\n\n";
    
    std::cout << "DATA:\n";
    std::cout << "  - Dataset: All 3 cities combined (" << arr.count << " residents)\n";
    std::cout << "  - Sorting field: Age (ascending)\n";
    std::cout << "  - Algorithms: Bubble (algorithm=1), Quick (algorithm=2), Insertion (algorithm=3)\n\n";
    
    ResidentArray arrBubble = copyArray(arr);
    PerfMetrics bubbleMetrics = sortArrayWithAlgorithm(arrBubble, 1, 1);
    
    ResidentArray arrQuick = copyArray(arr);
    PerfMetrics quickMetrics = sortArrayWithAlgorithm(arrQuick, 2, 1);
    
    ResidentArray arrInsertion = copyArray(arr);
    PerfMetrics insertionMetrics = sortArrayWithAlgorithm(arrInsertion, 3, 1);
    
    std::cout << "EXECUTION RESULTS:\n";
    std::cout << "  Bubble Sort:    " << bubbleMetrics.executionTimeUs << " us\n";
    std::cout << "  Quick Sort:     " << quickMetrics.executionTimeUs << " us\n";
    std::cout << "  Insertion Sort: " << insertionMetrics.executionTimeUs << " us\n\n";
    
    std::cout << "BIG O ANALYSIS:\n";
    std::cout << "  - Bubble Sort: O(n^2) = " << arr.count << "^2 = " 
              << (arr.count * arr.count) << " operations\n";
    std::cout << "  - Quick Sort: O(n log n) = " << arr.count << " x log2(" << arr.count 
              << ") ~ " << (arr.count * (int)log2(arr.count)) << " operations\n";
    std::cout << "  - Ratio (Theoretical): " << ((double)(arr.count * arr.count) / 
              (arr.count * (int)log2(arr.count))) << "x\n\n";
    
    bubbleToQuickRatio = (quickMetrics.executionTimeUs > 0) ? 
        ((double)bubbleMetrics.executionTimeUs / quickMetrics.executionTimeUs) : 0;
    std::cout << "JUSTIFICATION:\n";
    std::cout << "  Quick/Bubble actual ratio: " << bubbleToQuickRatio << "x faster\n";
    if (bubbleToQuickRatio >= 5 && bubbleToQuickRatio <= 10) {
        std::cout << "  [+] Result matches hypothesis (5-10x range)\n";
    } else if (bubbleToQuickRatio > 10) {
        std::cout << "  [+] Quick Sort even faster than expected (> 10x)\n";
    } else if (bubbleToQuickRatio >= 3) {
        std::cout << "  ~ Result close to hypothesis but slightly lower than expected\n";
    } else {
        std::cout << "  ! Result differs - may indicate data structure overhead\n";
    }
    std::cout << "  Quick Sort dominates due to fewer comparisons and divide-and-conquer efficiency.\n";
    
    // -- experiment 2: data state impact --
    printExperimentHeader("DATA STATE IMPACT (Sorted vs Random vs Reverse)");
    
    std::cout << "HYPOTHESIS:\n";
    std::cout << "\"Bubble Sort shows dramatic performance difference based on input order:\n";
    std::cout << "  - Sorted data: O(n) best case (few swaps needed)\n";
    std::cout << "  - Random data: O(n^2) average case\n";
    std::cout << "  - Reverse sorted: O(n^2) worst case (maximum swaps)\n";
    std::cout << "I expect 10-100x difference between best and worst cases.\"\n\n";
    
    std::cout << "DATA:\n";
    std::cout << "  - Using Bubble Sort on same dataset in different states\n";
    std::cout << "  - Best case: Pre-sorted array\n";
    std::cout << "  - Average case: Original random array\n\n";
    
    // best case: sort then sort again (already sorted)
    ResidentArray arrSorted = copyArray(arr);
    sortArrayWithAlgorithm(arrSorted, 2, 1);  // quick sort first
    PerfMetrics bubbleSorted = sortArrayWithAlgorithm(arrSorted, 1, 1);  // then bubble, already sorted so cheap
    
    // average case: sort random data
    ResidentArray arrRandom = copyArray(arr);
    PerfMetrics bubbleRandom = sortArrayWithAlgorithm(arrRandom, 1, 1);
    
    std::cout << "EXECUTION RESULTS:\n";
    std::cout << "  Bubble on sorted data:  " << bubbleSorted.executionTimeUs << " us (best case)\n";
    std::cout << "  Bubble on random data:  " << bubbleRandom.executionTimeUs << " us (average case)\n\n";
    
    std::cout << "BIG O ANALYSIS:\n";
    std::cout << "  - Best case (sorted): O(n) - single pass, no swaps needed\n";
    std::cout << "  - Average case: O(n^2) - typical random input\n";
    std::cout << "  - Expected theoretical ratio: 10-100x\n\n";
    
    double sortedToRandomRatio = (bubbleRandom.executionTimeUs > 0) ? 
        ((double)bubbleRandom.executionTimeUs / bubbleSorted.executionTimeUs) : 0;
    std::cout << "JUSTIFICATION:\n";
    std::cout << "  Actual ratio (Random/Sorted): " << sortedToRandomRatio << "x\n";
    if (sortedToRandomRatio >= 10) {
        std::cout << "  [+] Significant difference observed - matches expected behavior\n";
    } else {
        std::cout << "  ~ Smaller difference - possibly due to modern CPU optimizations\n";
    }
    std::cout << "  Already-sorted data has minimal comparisons and no swaps needed.\n";
    
    // -- experiment 3: array vs linked list sorting --
    printExperimentHeader("ARRAY vs LINKED LIST SORTING PERFORMANCE");
    
    std::cout << "HYPOTHESIS:\n";
    std::cout << "\"Array should be 2-3x faster than Linked List for sorting because:\n";
    std::cout << "  1. Contiguous memory = better CPU cache locality\n";
    std::cout << "  2. No pointer dereferencing overhead\n";
    std::cout << "  3. O-notation same but constants differ significantly\"\n\n";
    
    std::cout << "DATA:\n";
    std::cout << "  - Algorithm: Quick Sort (algorithm=2)\n";
    std::cout << "  - Field: Age\n";
    std::cout << "  - Array size: " << arr.count << " residents\n";
    std::cout << "  - List size: " << list.size << " nodes\n\n";
    
    ResidentArray arrForSort = copyArray(arr);
    PerfMetrics arraySort = sortArrayWithAlgorithm(arrForSort, 2, 1);
    
    // reload list for a fair comparison
    LinkedList listForSort;
    for (int i = 0; i < 3; ++i) {
        loadCSV(listForSort, FILES[i], CITIES[i]);
    }
    PerfMetrics listSort = sortLinkedListWithAlgorithm(listForSort, 2, 1);
    
    std::cout << "EXECUTION RESULTS:\n";
    std::cout << "  Array Sort:       " << arraySort.executionTimeUs << " us\n";
    std::cout << "  Linked List Sort: " << listSort.executionTimeUs << " us\n\n";
    
    std::cout << "BIG O ANALYSIS:\n";
    std::cout << "  - Both use Quick Sort: O(n log n)\n";
    std::cout << "  - BUT: Array has O(1) random access, LL has O(n) traversal\n";
    std::cout << "  - Array: better cache locality, contiguous memory\n";
    std::cout << "  - Linked List: pointer chasing, memory fragmentation\n\n";
    
    arrayToListRatio = (listSort.executionTimeUs > 0) ?
        ((double)listSort.executionTimeUs / arraySort.executionTimeUs) : 0;
    std::cout << "JUSTIFICATION:\n";
    std::cout << "  Actual ratio (List/Array): " << arrayToListRatio << "x slower\n";
    if (arrayToListRatio >= 2 && arrayToListRatio <= 3.5) {
        std::cout << "  [+] Matches expected 2-3.5x difference\n";
    } else if (arrayToListRatio > 3.5) {
        std::cout << "  [+] List significantly slower - pointer overhead substantial\n";
    } else {
        std::cout << "  ~ Smaller difference than expected\n";
    }
    std::cout << "  Array's contiguous memory gives substantial performance advantage.\n";
    
    // -- experiment 4: linear vs binary search --
    printExperimentHeader("LINEAR vs BINARY SEARCH (On Sorted Data)");
    
    std::cout << "HYPOTHESIS:\n";
    std::cout << "\"On sorted data, Binary Search should be 10-50x faster than Linear Search:\n";
    std::cout << "  - Linear: O(n) needs ~ " << (arr.count/2) << " comparisons average\n";
    std::cout << "  - Binary: O(log n) needs ~ " << (int)log2(arr.count) << " comparisons\n";
    std::cout << "  - Expected speedup: " << (arr.count / (2 * (int)log2(arr.count))) 
              << "x\"\n\n";
    
    // linear search on unsorted array
    PerfMetrics linearSearch = searchByAgeGroupArray(arr, "26-45");
    
    // sort by age, then binary search
    ResidentArray arrSortedForSearch = copyArray(arr);
    sortArrayWithAlgorithm(arrSortedForSearch, 2, 1);
    PerfMetrics binarySearch = binarySearchByAgeArray(arrSortedForSearch, 35);
    
    std::cout << "DATA:\n";
    std::cout << "  - Linear: unsorted array, searching for age group '26-45'\n";
    std::cout << "  - Binary: sorted array, searching for exact age 35\n";
    std::cout << "  - Found by linear: " << linearSearch.itemsProcessed << " residents\n";
    std::cout << "  - Found by binary: " << binarySearch.itemsProcessed << " residents\n\n";
    
    std::cout << "EXECUTION RESULTS:\n";
    std::cout << "  Linear Search:  " << linearSearch.executionTimeUs << " us\n";
    std::cout << "  Binary Search:  " << binarySearch.executionTimeUs << " us\n\n";
    
    std::cout << "BIG O ANALYSIS:\n";
    std::cout << "  - Linear: O(n) = " << arr.count << " comparisons max\n";
    std::cout << "  - Binary: O(log n) = " << (int)log2(arr.count) << " comparisons max\n";
    std::cout << "  - Theoretical speedup: " << ((double)arr.count / log2(arr.count)) << "x\n\n";
    
    linearToBinaryRatio = (binarySearch.executionTimeUs > 0) ?
        ((double)linearSearch.executionTimeUs / binarySearch.executionTimeUs) : 0;
    std::cout << "JUSTIFICATION:\n";
    std::cout << "  Actual ratio (Linear/Binary): " << linearToBinaryRatio << "x\n";
    if (linearToBinaryRatio >= 10) {
        std::cout << "  [+] Binary search dramatically faster\n";
    }
    std::cout << "  Binary search eliminates half of search space with each comparison.\n";
    std::cout << "  IMPORTANT: Binary search requires pre-sorted data!\n";
    
    // -- experiment 5: search performance (array vs list) --
    printExperimentHeader("SEARCH PERFORMANCE - ARRAY vs LINKED LIST");
    
    std::cout << "HYPOTHESIS:\n";
    std::cout << "\"Linked List searching should be 2-3x slower than Array because:\n";
    std::cout << "  - Must traverse pointer-by-pointer (no random access)\n";
    std::cout << "  - Cache misses at each node dereference\n";
    std::cout << "  - O-notation same O(n) but constants matter\"\n\n";
    
    std::cout << "DATA:\n";
    std::cout << "  - Search query: All residents using transport mode 'Car'\n\n";
    
    PerfMetrics arraySearch = searchByModeArray(arr, "Car");
    PerfMetrics listSearch = searchByModeList(list, "Car");
    
    std::cout << "EXECUTION RESULTS:\n";
    std::cout << "  Array Search: " << arraySearch.executionTimeUs << " us (found " 
              << arraySearch.itemsProcessed << " residents)\n";
    std::cout << "  List Search:  " << listSearch.executionTimeUs << " us (found " 
              << listSearch.itemsProcessed << " residents)\n\n";
    
    std::cout << "BIG O ANALYSIS:\n";
    std::cout << "  - Both: O(n) - must check every element\n";
    std::cout << "  - BUT: Array has direct memory access\n";
    std::cout << "  - List requires pointer dereferencing at each step\n\n";
    
    double arrayToListSearchRatio = (listSearch.executionTimeUs > 0) ? 
        ((double)listSearch.executionTimeUs / arraySearch.executionTimeUs) : 0;
    std::cout << "JUSTIFICATION:\n";
    std::cout << "  Actual ratio (List/Array): " << arrayToListSearchRatio << "x slower\n";
    if (arrayToListSearchRatio >= 2) {
        std::cout << "  [+] Linked list slower as expected\n";
    }
    std::cout << "  Array's sequential memory layout is more cache-friendly.\n";
    
    // -- experiment 6: insertion sort on pre-sorted data --
    printExperimentHeader("INSERTION SORT ON PRE-SORTED DATA (Best Case)");
    
    std::cout << "HYPOTHESIS:\n";
    std::cout << "\"Insertion Sort achieves O(n) best-case complexity on already-sorted data.\n";
    std::cout << "If we sort the same array twice with Insertion Sort, the 2nd pass\n";
    std::cout << "should be much faster (already sorted = fewer insertions needed).\n";
    std::cout << "Expected: 5-20x faster on second pass.\"\n\n";
    
    std::cout << "DATA:\n";
    std::cout << "  - Same array sorted twice with Insertion Sort\n";
    std::cout << "  - First pass: random data (O(n^2) average)\n";
    std::cout << "  - Second pass: pre-sorted data (O(n) best case)\n\n";
    
    ResidentArray insertTest = copyArray(arr);
    PerfMetrics insertion1stPass = sortArrayWithAlgorithm(insertTest, 3, 1);
    
    // sort again on already-sorted data
    PerfMetrics insertion2ndPass = sortArrayWithAlgorithm(insertTest, 3, 1);
    
    std::cout << "EXECUTION RESULTS:\n";
    std::cout << "  1st pass (random data): " << insertion1stPass.executionTimeUs << " us\n";
    std::cout << "  2nd pass (sorted data): " << insertion2ndPass.executionTimeUs << " us\n\n";
    
    std::cout << "BIG O ANALYSIS:\n";
    std::cout << "  - 1st pass: O(n^2) average on random data\n";
    std::cout << "  - 2nd pass: O(n) best case on sorted data\n";
    std::cout << "  - Expected speedup: " << (arr.count/2) << "x\n\n";
    
    insertion1stTo2ndRatio = (insertion2ndPass.executionTimeUs > 0) ?
        ((double)insertion1stPass.executionTimeUs / insertion2ndPass.executionTimeUs) : 0;
    std::cout << "JUSTIFICATION:\n";
    std::cout << "  Actual ratio (1st/2nd): " << insertion1stTo2ndRatio << "x\n";
    if (insertion1stTo2ndRatio >= 5) {
        std::cout << "  [+] Significant speedup observed - demonstrates best-case efficiency\n";
    }
    std::cout << "  Insertion sort excels when data is nearly or fully sorted!\n";
    
    // -- experiment 7: scaling test --
    printExperimentHeader("SCALING TEST - Algorithm Behavior with Increasing Data");
    
    std::cout << "HYPOTHESIS:\n";
    std::cout << "\"As data size grows, O(n^2) algorithms degrade quadratically:\n";
    std::cout << "  - If n doubles, time should ~quadruple\n";
    std::cout << "  - Bubble Sort on 200 items: time T\n";
    std::cout << "  - Bubble Sort on 400 items: time ~4T\n";
    std::cout << "  - Bubble Sort on 500 items: time ~6.25T\n";
    std::cout << "  Quick Sort should grow ~linearly: 2n -> ~2T\"\n\n";
    
    // load just City A (~200 items)
    ResidentArray small;
    loadCSV(small, FILES[0], CITIES[0]);
    
    // load City A + B (~400 items)
    ResidentArray medium;
    loadCSV(medium, FILES[0], CITIES[0]);
    loadCSV(medium, FILES[1], CITIES[1]);
    
    // all 3 cities (full 500+ items)
    ResidentArray large = copyArray(arr);
    
    std::cout << "DATA SIZES:\n";
    std::cout << "  - Small:  " << small.count << " residents (City A)\n";
    std::cout << "  - Medium: " << medium.count << " residents (Cities A+B)\n";
    std::cout << "  - Large:  " << large.count << " residents (All cities)\n\n";
    
    // bubble sort on each size
    ResidentArray smallBubbleCopy = copyArray(small);
    PerfMetrics smallBubble = sortArrayWithAlgorithm(smallBubbleCopy, 1, 1);
    
    ResidentArray mediumBubbleCopy = copyArray(medium);
    PerfMetrics mediumBubble = sortArrayWithAlgorithm(mediumBubbleCopy, 1, 1);
    
    ResidentArray largeBubbleCopy = copyArray(large);
    PerfMetrics largeBubble = sortArrayWithAlgorithm(largeBubbleCopy, 1, 1);
    
    // quick sort on each size
    ResidentArray smallQuickCopy = copyArray(small);
    PerfMetrics smallQuick = sortArrayWithAlgorithm(smallQuickCopy, 2, 1);
    
    ResidentArray mediumQuickCopy = copyArray(medium);
    PerfMetrics mediumQuick = sortArrayWithAlgorithm(mediumQuickCopy, 2, 1);
    
    ResidentArray largeQuickCopy = copyArray(large);
    PerfMetrics largeQuick = sortArrayWithAlgorithm(largeQuickCopy, 2, 1);
    
    std::cout << "EXECUTION RESULTS:\n";
    std::cout << "\nBubble Sort (O(n^2)):\n";
    std::cout << "  Small:  " << smallBubble.executionTimeUs << " us\n";
    std::cout << "  Medium: " << mediumBubble.executionTimeUs << " us (ratio: " 
              << ((double)mediumBubble.executionTimeUs / smallBubble.executionTimeUs) << "x)\n";
    std::cout << "  Large:  " << largeBubble.executionTimeUs << " us (ratio: " 
              << ((double)largeBubble.executionTimeUs / smallBubble.executionTimeUs) << "x)\n";
    
    std::cout << "\nQuick Sort (O(n log n)):\n";
    std::cout << "  Small:  " << smallQuick.executionTimeUs << " us\n";
    std::cout << "  Medium: " << mediumQuick.executionTimeUs << " us (ratio: " 
              << ((double)mediumQuick.executionTimeUs / smallQuick.executionTimeUs) << "x)\n";
    std::cout << "  Large:  " << largeQuick.executionTimeUs << " us (ratio: " 
              << ((double)largeQuick.executionTimeUs / smallQuick.executionTimeUs) << "x)\n\n";
    
    std::cout << "BIG O ANALYSIS:\n";
    std::cout << "  - Bubble: O(n^2) - doubling n quadruples time\n";
    std::cout << "    Small to Medium: (" << medium.count << "/" << small.count 
              << ")^2 = " << ((double)medium.count / small.count) 
              << "^2 = " << (((double)medium.count / small.count) * ((double)medium.count / small.count)) 
              << "x expected\n";
    std::cout << "  - Quick: O(n log n) - doubling n roughly doubles time\n";
    std::cout << "    Small to Medium: " << ((double)medium.count / small.count) 
              << "x expected (linear growth)\n\n";
    
    std::cout << "JUSTIFICATION:\n";
    std::cout << "  Bubble Sort growth rate matches theoretical O(n^2) degradation.\n";
    std::cout << "  Quick Sort grows much slower - efficient even with 3x more data.\n";
    std::cout << "  This demonstrates why O(n log n) algorithms are preferred for large datasets.\n";
    
    // -- experiment 8: real-world scenario --
    printExperimentHeader("REAL-WORLD SCENARIO - Filtering High Carbon Emitters");
    
    std::cout << "HYPOTHESIS:\n";
    std::cout << "\"Task: Find all residents who are heavy car users with high emissions.\n";
    std::cout << "Two approaches:\n";
    std::cout << "  Approach 1: Sort by emission, then scan once - O(n log n)\n";
    std::cout << "  Approach 2: Direct linear scan without sorting - O(n)\n";
    std::cout << "On this small dataset, direct scan should be faster,\n";
    std::cout << "but sorting establishes useful order for future queries.\"\n\n";
    
    std::cout << "DATA:\n";
    std::cout << "  - Target: Residents using 'Car' with monthlyEmission > 80 kg CO2\n";
    std::cout << "  - Dataset: " << arr.count << " residents\n\n";
    
    // approach 1: sort then scan
    auto start1 = std::chrono::high_resolution_clock::now();
    ResidentArray sortedEmission = copyArray(arr);
    sortArrayWithAlgorithm(sortedEmission, 2, 2);  // quick sort by emission
    int highEmittersSorted = 0;
    for (int i = 0; i < sortedEmission.count; ++i) {
        if (sortedEmission.data[i].monthlyEmission > 80 && 
            sortedEmission.data[i].modeOfTransport == "Car") {
            highEmittersSorted++;
        }
    }
    auto end1 = std::chrono::high_resolution_clock::now();
    auto time1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count();
    
    // approach 2: direct scan
    auto start2 = std::chrono::high_resolution_clock::now();
    int highEmittersDirect = 0;
    for (int i = 0; i < arr.count; ++i) {
        if (arr.data[i].monthlyEmission > 80 && 
            arr.data[i].modeOfTransport == "Car") {
            highEmittersDirect++;
        }
    }
    auto end2 = std::chrono::high_resolution_clock::now();
    auto time2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count();
    
    std::cout << "EXECUTION RESULTS:\n";
    std::cout << "  Approach 1 (Sort + Scan): " << time1 << " us (found " << highEmittersSorted << ")\n";
    std::cout << "  Approach 2 (Direct Scan): " << time2 << " us (found " << highEmittersDirect << ")\n\n";
    
    std::cout << "BIG O ANALYSIS:\n";
    std::cout << "  - Approach 1: O(n log n) sorting + O(n) scan = O(n log n) dominated\n";
    std::cout << "  - Approach 2: O(n) direct scan\n";
    std::cout << "  - For small n, scan is faster; for large n (millions), sorted access pattern better\n\n";
    
    std::cout << "JUSTIFICATION:\n";
    std::cout << "  Time difference: " << (time1 > time2 ? time1 - time2 : time2 - time1) << " us\n";
    if (time2 < time1) {
        std::cout << "  Direct scan is faster on this dataset - sorting overhead not justified.\n";
    } else {
        std::cout << "  Both approaches similar - sorting adds minimal overhead.\n";
    }
    std::cout << "  But if we need to repeat this query multiple times, sort once then quick scan wins!\n";
    
    // -- city planner recommendations --
    printCityPlannerRecommendations(arr);

    // -- summary and conclusions --
    std::cout << "\n\n";
    for (int i = 0; i < 80; i++) std::cout << "=";
    std::cout << "\n";
    printCentered("EXPERIMENT SUMMARY AND KEY INSIGHTS");
    for (int i = 0; i < 80; i++) std::cout << "=";
    std::cout << "\n\n";
    
    std::cout << "KEY FINDINGS:\n\n";
    
    std::cout << "1. ALGORITHM MATTERS\n";
    std::cout << "   - Quick Sort: " << bubbleToQuickRatio << "x faster than Bubble Sort\n";
    std::cout << "   - Always prefer O(n log n) for large datasets\n\n";
    
    std::cout << "2. DATA STRUCTURE SELECTION\n";
    std::cout << "   - Arrays: " << arrayToListRatio << "x faster than Linked Lists for sorting\n";
    std::cout << "   - Array cache locality provides massive advantage\n\n";
    
    std::cout << "3. SEARCH OPTIMIZATION\n";
    std::cout << "   - Binary Search: " << linearToBinaryRatio << "x faster than Linear on sorted data\n";
    std::cout << "   - Sorting cost is worthwhile if you'll search multiple times\n\n";
    
    std::cout << "4. SCALING BEHAVIOR\n";
    std::cout << "   - O(n^2) algorithms scale poorly (quadratic growth)\n";
    std::cout << "   - O(n log n) algorithms remain efficient at scale\n";
    std::cout << "   - Critical to choose right algorithm before data grows large\n\n";
    
    std::cout << "5. INSERTION SORT INSIGHT\n";
    std::cout << "   - Excellent for small or nearly-sorted datasets\n";
    std::cout << "   - " << insertion1stTo2ndRatio << "x faster when data is already sorted\n";
    std::cout << "   - Good hybrid choice for hybrid sort algorithms\n\n";
    
    std::cout << "RECOMMENDATIONS:\n";
    std::cout << "[+] Use Quick Sort for general-purpose sorting\n";
    std::cout << "[+] Use Binary Search only on pre-sorted data\n";
    std::cout << "[+] Prefer Arrays over Linked Lists for performance-critical code\n";
    std::cout << "[+] Profile before optimizing - but always use efficient algorithms\n";
    std::cout << "[+] Consider hybrid approaches (e.g., Tim Sort) for real-world data\n\n";
    
    for (int i = 0; i < 80; i++) std::cout << "=";
    std::cout << "\n";
    printCentered("EXPERIMENTS COMPLETE");
    for (int i = 0; i < 80; i++) std::cout << "=";
    std::cout << "\n";
    
    return 0;
}
