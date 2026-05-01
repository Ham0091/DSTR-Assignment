// ============================================================
// PROGRAM 1: ARRAY IMPLEMENTATION
// CT077-3-2-DSTR - Carbon Emission Analysis using Fixed Arrays
// ============================================================
// This program demonstrates all operations using ResidentArray only.
// Companion program: main_list.cpp (linked list implementation)
// ============================================================

#include "dataStructures.hpp"
#include "analysis.hpp"
#include "searching.hpp"
#include "sorting.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <cmath>

// Helper: deep copy a ResidentArray
ResidentArray copyArray(const ResidentArray& src) {
    ResidentArray copy;
    copy.count = src.count;
    for (int i = 0; i < src.count; i++) copy.data[i] = src.data[i];
    return copy;
}

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

int main() {
    std::cout << std::fixed << std::setprecision(2);

    std::cout << std::string(80, '=') << "\n";
    printCentered("PROGRAM 1: ARRAY IMPLEMENTATION");
    printCentered("DSTR Assignment - Urban Carbon Emission Analysis");
    std::cout << std::string(80, '=') << "\n";

    // ========================================================
    // SECTION 1: LOAD DATA INTO FIXED-SIZE ARRAY
    // ========================================================
    printSec("SECTION 1: LOADING DATA INTO FIXED-SIZE ARRAY (MAX_SIZE=600)");

    ResidentArray arr;
    const std::string FILES[3] = {
        "datasets/dataset1-cityA.csv",
        "datasets/dataset2-cityB.csv",
        "datasets/dataset3-cityC.csv"
    };
    const std::string CITIES[3] = {"CityA", "CityB", "CityC"};

    for (int i = 0; i < 3; i++) {
        loadCSV(arr, FILES[i], CITIES[i]);
    }

    // ========================================================
    // SECTION 2: DATA VERIFICATION
    // ========================================================
    printSec("SECTION 2: DATA VERIFICATION");

    std::cout << "Total residents loaded : " << arr.count << "\n";
    std::cout << "Memory used (data)     : " << (sizeof(Resident) * arr.count) << " bytes\n";
    std::cout << "Memory allocated       : " << (sizeof(Resident) * MAX_SIZE) << " bytes (fixed)\n";
    std::cout << "Unused slots           : " << (MAX_SIZE - arr.count) << " x "
              << sizeof(Resident) << " bytes = "
              << ((MAX_SIZE - arr.count) * sizeof(Resident)) << " bytes wasted\n\n";

    std::cout << "First 5 records:\n";
    printHorizontalLine();
    std::cout << std::setw(14) << std::left << "ResidentID"
              << " | " << std::setw(3)  << "Age"
              << " | " << std::setw(14) << "Mode"
              << " | " << std::setw(8)  << "Distance"
              << " | " << "MonthlyEmission\n";
    printHorizontalLine();
    for (int i = 0; i < 5 && i < arr.count; i++) printResident(arr.data[i]);

    // ========================================================
    // SECTION 3: CARBON EMISSION ANALYSIS (ARRAY)
    // ========================================================
    printSec("SECTION 3: CARBON EMISSION ANALYSIS");
    analyzeEmissionsByAgeGroupArray(arr);
    analyzeEmissionsByModeArray(arr);

    // ========================================================
    // EXPERIMENT 1: SORTING ALGORITHM COMPARISON
    // ========================================================
    printExpHeader("SORTING ALGORITHM COMPARISON - Bubble vs Quick vs Insertion (Array)");

    std::cout << "HYPOTHESIS:\n";
    std::cout << "  Quick Sort will be fastest: O(n log n) vs Bubble/Insertion O(n^2).\n";
    std::cout << "  For n=" << arr.count << ": Quick needs ~"
              << (arr.count * (int)log2(arr.count)) << " ops vs Bubble ~"
              << (arr.count * arr.count / 2) << " ops.\n\n";

    std::cout << "DATA: " << arr.count << " residents, sorting by Age ascending.\n\n";

    ResidentArray a1 = copyArray(arr), a2 = copyArray(arr), a3 = copyArray(arr);
    PerfMetrics bub = sortArrayWithAlgorithm(a1, 1, 1);
    PerfMetrics qui = sortArrayWithAlgorithm(a2, 2, 1);
    PerfMetrics ins = sortArrayWithAlgorithm(a3, 3, 1);

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
              << " | O(n^2) worst, O(n) best\n";
    std::cout << std::setw(20) << std::left << "Quick Sort"
              << " | " << std::setw(12) << std::right << qui.executionTimeUs
              << " | " << std::setw(12) << std::right << qui.memoryBytes
              << " | O(n log n) avg\n";
    std::cout << std::setw(20) << std::left << "Insertion Sort"
              << " | " << std::setw(12) << std::right << ins.executionTimeUs
              << " | " << std::setw(12) << std::right << ins.memoryBytes
              << " | O(n^2) avg, O(n) best\n";
    printHorizontalLine();

    double bqRatio = (qui.executionTimeUs > 0) ?
                     ((double)bub.executionTimeUs / qui.executionTimeUs) : 0;
    std::cout << "\nJUSTIFICATION:\n";
    std::cout << "  Bubble/Quick ratio: " << bqRatio << "x slower\n";
    if (bqRatio >= 3)
        std::cout << "  [+] O(n log n) advantage confirmed over O(n^2)\n";
    else
        std::cout << "  [~] Small n limits visible difference; grows at scale (see Exp 4)\n";

    // ========================================================
    // EXPERIMENT 2: DATA STATE IMPACT (BUBBLE SORT - ARRAY)
    // ========================================================
    printExpHeader("DATA STATE IMPACT - Bubble Sort on Random vs Sorted (Array)");

    std::cout << "HYPOTHESIS:\n";
    std::cout << "  Bubble Sort on already-sorted data = O(n) best case.\n";
    std::cout << "  The 2nd pass (sorted) should be dramatically faster than 1st (random).\n\n";

    std::cout << "DATA: Same array sorted twice with Bubble Sort.\n\n";

    ResidentArray stateTest = copyArray(arr);
    PerfMetrics pass1 = sortArrayWithAlgorithm(stateTest, 1, 1);  // random -> sorted
    PerfMetrics pass2 = sortArrayWithAlgorithm(stateTest, 1, 1);  // already sorted

    std::cout << "EXECUTION RESULTS:\n";
    std::cout << "  1st pass (random data) : " << pass1.executionTimeUs << " us  -> O(n^2)\n";
    std::cout << "  2nd pass (sorted data) : " << pass2.executionTimeUs << " us  -> O(n) early exit\n\n";

    double stateRatio = (pass2.executionTimeUs > 0) ?
                        ((double)pass1.executionTimeUs / pass2.executionTimeUs) : 0;
    std::cout << "BIG O ANALYSIS:\n";
    std::cout << "  Random: O(n^2) - maximum swaps\n";
    std::cout << "  Sorted: O(n)   - early-exit after 1 pass with zero swaps\n\n";

    std::cout << "JUSTIFICATION:\n";
    std::cout << "  Speedup ratio (random/sorted): " << stateRatio << "x\n";
    if (stateRatio >= 5)
        std::cout << "  [+] Dramatic speedup - early-exit correctly reduces to O(n) on sorted data\n";
    else
        std::cout << "  [~] Modern CPU branch prediction may reduce the observable difference\n";

    // ========================================================
    // EXPERIMENT 3: LINEAR vs BINARY SEARCH (ARRAY)
    // ========================================================
    printExpHeader("LINEAR vs BINARY SEARCH (Array)");

    std::cout << "HYPOTHESIS:\n";
    std::cout << "  Binary Search O(log n) is faster than Linear Search O(n) on sorted data.\n";
    std::cout << "  Linear needs ~" << arr.count / 2 << " comparisons avg; Binary needs ~"
              << (int)log2(arr.count) << " max.\n\n";

    PerfMetrics linAge  = searchByAgeGroupArray(arr, "26-45");
    PerfMetrics linMode = searchByModeArray(arr, "Car");
    PerfMetrics linDist = searchByDistanceThresholdArray(arr, 15.0);

    ResidentArray sortedForBin = copyArray(arr);
    sortArrayWithAlgorithm(sortedForBin, 2, 1);
    PerfMetrics binAge = binarySearchByAgeArray(sortedForBin, 35);

    std::cout << "EXECUTION RESULTS:\n";
    printHorizontalLine();
    std::cout << std::setw(35) << std::left << "Search"
              << " | " << std::setw(10) << std::right << "Time (us)"
              << " | " << std::setw(8)  << std::right << "Found"
              << " | Big-O\n";
    printHorizontalLine();
    std::cout << std::setw(35) << std::left << "Linear: age group 26-45"
              << " | " << std::setw(10) << std::right << linAge.executionTimeUs
              << " | " << std::setw(8)  << std::right << linAge.itemsProcessed  << " | O(n)\n";
    std::cout << std::setw(35) << std::left << "Linear: mode = Car"
              << " | " << std::setw(10) << std::right << linMode.executionTimeUs
              << " | " << std::setw(8)  << std::right << linMode.itemsProcessed << " | O(n)\n";
    std::cout << std::setw(35) << std::left << "Linear: distance > 15 km"
              << " | " << std::setw(10) << std::right << linDist.executionTimeUs
              << " | " << std::setw(8)  << std::right << linDist.itemsProcessed << " | O(n)\n";
    std::cout << std::setw(35) << std::left << "Binary: exact age = 35 (sorted)"
              << " | " << std::setw(10) << std::right << binAge.executionTimeUs
              << " | " << std::setw(8)  << std::right << binAge.itemsProcessed  << " | O(log n)\n";
    printHorizontalLine();

    double lbRatio = (binAge.executionTimeUs > 0) ?
                     ((double)linAge.executionTimeUs / binAge.executionTimeUs) : 0;
    std::cout << "\nJUSTIFICATION:\n";
    std::cout << "  Linear/Binary speedup: " << lbRatio << "x\n";
    std::cout << "  NOTE: Binary search requires pre-sorted data (sorting cost not included).\n";
    std::cout << "  Array's O(1) index access makes binary search highly effective.\n";

    // ========================================================
    // EXPERIMENT 4: SCALING TEST (ARRAY)
    // ========================================================
    printExpHeader("SCALING TEST - O(n^2) vs O(n log n) Growth (Array)");

    std::cout << "HYPOTHESIS:\n";
    std::cout << "  Bubble Sort time grows quadratically (n doubles -> time ~4x).\n";
    std::cout << "  Quick Sort time grows near-linearly (n doubles -> time ~2x).\n\n";

    ResidentArray small, medium, large;
    loadCSV(small,  FILES[0], CITIES[0]);
    loadCSV(medium, FILES[0], CITIES[0]);
    loadCSV(medium, FILES[1], CITIES[1]);
    large = copyArray(arr);

    ResidentArray s1=copyArray(small),  s2=copyArray(medium), s3=copyArray(large);
    ResidentArray s4=copyArray(small),  s5=copyArray(medium), s6=copyArray(large);
    PerfMetrics bS=sortArrayWithAlgorithm(s1,1,1), bM=sortArrayWithAlgorithm(s2,1,1), bL=sortArrayWithAlgorithm(s3,1,1);
    PerfMetrics qS=sortArrayWithAlgorithm(s4,2,1), qM=sortArrayWithAlgorithm(s5,2,1), qL=sortArrayWithAlgorithm(s6,2,1);

    std::cout << "DATA SIZES: Small=" << small.count
              << " | Medium=" << medium.count
              << " | Large=" << large.count << "\n\n";

    std::cout << "EXECUTION RESULTS:\n";
    printHorizontalLine();
    std::cout << std::setw(18) << std::left << "Algorithm"
              << " | " << std::setw(10) << std::right << "Small(us)"
              << " | " << std::setw(10) << std::right << "Medium(us)"
              << " | " << std::setw(10) << std::right << "Large(us)"
              << " | " << std::setw(8)  << std::right << "M/S ratio"
              << " | L/S ratio\n";
    printHorizontalLine();
    double bmRatio = (bS.executionTimeUs>0) ? (double)bM.executionTimeUs/bS.executionTimeUs : 0;
    double blRatio = (bS.executionTimeUs>0) ? (double)bL.executionTimeUs/bS.executionTimeUs : 0;
    double qmRatio = (qS.executionTimeUs>0) ? (double)qM.executionTimeUs/qS.executionTimeUs : 0;
    double qlRatio = (qS.executionTimeUs>0) ? (double)qL.executionTimeUs/qS.executionTimeUs : 0;
    std::cout << std::setw(18) << std::left << "Bubble O(n^2)"
              << " | " << std::setw(10) << std::right << bS.executionTimeUs
              << " | " << std::setw(10) << std::right << bM.executionTimeUs
              << " | " << std::setw(10) << std::right << bL.executionTimeUs
              << " | " << std::setw(8)  << std::right << bmRatio
              << " | " << blRatio << "\n";
    std::cout << std::setw(18) << std::left << "Quick O(n log n)"
              << " | " << std::setw(10) << std::right << qS.executionTimeUs
              << " | " << std::setw(10) << std::right << qM.executionTimeUs
              << " | " << std::setw(10) << std::right << qL.executionTimeUs
              << " | " << std::setw(8)  << std::right << qmRatio
              << " | " << qlRatio << "\n";
    printHorizontalLine();

    std::cout << "\nJUSTIFICATION:\n";
    std::cout << "  Theoretical O(n^2): size ratio " << ((double)medium.count/small.count)
              << "x -> time ratio " << (((double)medium.count/small.count)*((double)medium.count/small.count)) << "x.\n";
    std::cout << "  Bubble actual M/S ratio " << bmRatio << "x vs expected "
              << (((double)medium.count/small.count)*((double)medium.count/small.count)) << "x.\n";
    std::cout << "  Quick Sort grows much slower - critical advantage at scale.\n";

    // ========================================================
    // EXPERIMENT 5: REAL-WORLD QUERY (ARRAY)
    // ========================================================
    printExpHeader("REAL-WORLD QUERY - High Carbon Emitters (Array)");

    std::cout << "HYPOTHESIS:\n";
    std::cout << "  Direct O(n) scan faster than Sort + Scan for a one-time query.\n";
    std::cout << "  Sorting only pays off if the same sorted data is queried many times.\n\n";

    auto t1s = std::chrono::high_resolution_clock::now();
    ResidentArray sortedEm = copyArray(arr);
    sortArrayWithAlgorithm(sortedEm, 2, 2);
    int foundSorted = 0;
    for (int i = 0; i < sortedEm.count; i++)
        if (sortedEm.data[i].monthlyEmission > 80 && sortedEm.data[i].modeOfTransport == "Car")
            foundSorted++;
    auto t1e = std::chrono::high_resolution_clock::now();
    long long time1 = std::chrono::duration_cast<std::chrono::microseconds>(t1e - t1s).count();

    auto t2s = std::chrono::high_resolution_clock::now();
    int foundDirect = 0;
    for (int i = 0; i < arr.count; i++)
        if (arr.data[i].monthlyEmission > 80 && arr.data[i].modeOfTransport == "Car")
            foundDirect++;
    auto t2e = std::chrono::high_resolution_clock::now();
    long long time2 = std::chrono::duration_cast<std::chrono::microseconds>(t2e - t2s).count();

    std::cout << "EXECUTION RESULTS (Car users with emission > 80 kg CO2/month):\n";
    std::cout << "  Approach 1 (Sort O(n log n) + Scan O(n)): " << time1 << " us | Found: " << foundSorted << "\n";
    std::cout << "  Approach 2 (Direct scan O(n)):             " << time2 << " us | Found: " << foundDirect << "\n\n";

    std::cout << "JUSTIFICATION:\n";
    if (time2 < time1)
        std::cout << "  [+] Direct scan is faster for a single one-off query.\n";
    else
        std::cout << "  [~] Sort overhead minimal here; sorted array enables fast future queries.\n";
    std::cout << "  Strategy: sort once, query many times = O(n log n) + k*O(log n) total.\n";

    // ========================================================
    // CITY PLANNER RECOMMENDATIONS
    // ========================================================
    printCityPlannerRecommendations(arr);

    // ========================================================
    // PROGRAM SUMMARY
    // ========================================================
    std::cout << "\n\n";
    std::cout << std::string(80, '=') << "\n";
    printCentered("PROGRAM 1 COMPLETE - ARRAY IMPLEMENTATION SUMMARY");
    std::cout << std::string(80, '=') << "\n";

    std::cout << "\nARRAY PERFORMANCE PROFILE:\n";
    printHorizontalLine();
    std::cout << std::setw(30) << std::left << "Operation"
              << " | Complexity | Notes\n";
    printHorizontalLine();
    std::cout << std::setw(30) << std::left << "Access by index"      << " | O(1)       | Direct memory address\n";
    std::cout << std::setw(30) << std::left << "Bubble Sort"          << " | O(n^2)     | " << bub.executionTimeUs << " us on " << arr.count << " items\n";
    std::cout << std::setw(30) << std::left << "Quick Sort"           << " | O(n log n) | " << qui.executionTimeUs << " us on " << arr.count << " items\n";
    std::cout << std::setw(30) << std::left << "Insertion Sort"       << " | O(n^2)     | " << ins.executionTimeUs << " us on " << arr.count << " items\n";
    std::cout << std::setw(30) << std::left << "Linear Search"        << " | O(n)       | Cache-friendly sequential scan\n";
    std::cout << std::setw(30) << std::left << "Binary Search"        << " | O(log n)   | Requires pre-sorted array\n";
    printHorizontalLine();
    std::cout << "  Memory: " << (sizeof(Resident)*arr.count) << " bytes used / "
              << (sizeof(Resident)*MAX_SIZE) << " bytes allocated (fixed)\n";
    std::cout << "  Limitation: Cannot grow beyond MAX_SIZE=" << MAX_SIZE << "\n";
    std::cout << std::string(80, '=') << "\n";

    return 0;
}
