#include "analysis.hpp"
#include <iostream>
#include <iomanip>
#include <cstring>

// ============================================================
// SECTION 1: Age Group Classification Implementation
// ============================================================

std::string getAgeGroup(int age) {
    if (age >= 6 && age <= 17) {
        return "6-17: Children & Teenagers";
    } else if (age >= 18 && age <= 25) {
        return "18-25: University Students";
    } else if (age >= 26 && age <= 45) {
        return "26-45: Working Adults (Early Career)";
    } else if (age >= 46 && age <= 60) {
        return "46-60: Working Adults (Late Career)";
    } else if (age >= 61 && age <= 100) {
        return "61-100: Senior Citizens / Retirees";
    }
    return "Unknown";
}

std::string getAgeGroupCode(int age) {
    if (age >= 6 && age <= 17) {
        return "6-17";
    } else if (age >= 18 && age <= 25) {
        return "18-25";
    } else if (age >= 26 && age <= 45) {
        return "26-45";
    } else if (age >= 46 && age <= 60) {
        return "46-60";
    } else if (age >= 61 && age <= 100) {
        return "61+";
    }
    return "Unknown";
}

// ============================================================
// SECTION 2: Helper Struct for Mode Analysis
// ============================================================
// Tracks statistics for each transport mode
struct ModeInfo {
    char mode[50];
    int count;
    double totalEmission;

    ModeInfo() : count(0), totalEmission(0.0) {
        mode[0] = '\0';
    }
};

// Internal helper: return age group index (0-4), or -1 if out of range
static int ageGroupIndex(int age) {
    if (age >= 6  && age <= 17)  return 0;
    if (age >= 18 && age <= 25)  return 1;
    if (age >= 26 && age <= 45)  return 2;
    if (age >= 46 && age <= 60)  return 3;
    if (age >= 61 && age <= 100) return 4;
    return -1;
}

// Short display names for table columns (kept narrow for alignment)
static const char* GROUP_SHORT[5] = {
    "6-17 (Child/Teen)",
    "18-25 (University)",
    "26-45 (Work Early)",
    "46-60 (Work Late)",
    "61+   (Senior)"
};

// Full names matching the assignment brief
static const char* GROUP_FULL[5] = {
    "6-17: Children & Teenagers",
    "18-25: University Students / Young Adults",
    "26-45: Working Adults (Early Career)",
    "46-60: Working Adults (Late Career)",
    "61-100: Senior Citizens / Retirees"
};

// ============================================================
// SECTION 3: Analyze Emissions by Age Group (Array)
// ============================================================

void analyzeEmissionsByAgeGroupArray(const ResidentArray& arr) {
    if (arr.count == 0) {
        std::cout << "No residents to analyze." << std::endl;
        return;
    }

    int ageGroupCounts[5]          = {0, 0, 0, 0, 0};
    double ageGroupEmissions[5]    = {0.0, 0.0, 0.0, 0.0, 0.0};
    ModeInfo modesByAgeGroup[5][10];
    int modeCountsByAgeGroup[5]    = {0, 0, 0, 0, 0};

    for (int i = 0; i < arr.count; i++) {
        int g = ageGroupIndex(arr.data[i].age);
        if (g == -1) continue;

        ageGroupCounts[g]++;
        ageGroupEmissions[g] += arr.data[i].monthlyEmission;

        bool modeFound = false;
        for (int m = 0; m < modeCountsByAgeGroup[g]; m++) {
            if (strcmp(modesByAgeGroup[g][m].mode, arr.data[i].modeOfTransport.c_str()) == 0) {
                modesByAgeGroup[g][m].count++;
                modesByAgeGroup[g][m].totalEmission += arr.data[i].monthlyEmission;
                modeFound = true;
                break;
            }
        }
        if (!modeFound && modeCountsByAgeGroup[g] < 10) {
            strcpy_s(modesByAgeGroup[g][modeCountsByAgeGroup[g]].mode,
                     arr.data[i].modeOfTransport.c_str());
            modesByAgeGroup[g][modeCountsByAgeGroup[g]].count = 1;
            modesByAgeGroup[g][modeCountsByAgeGroup[g]].totalEmission = arr.data[i].monthlyEmission;
            modeCountsByAgeGroup[g]++;
        }
    }

    printCentered("=== CARBON EMISSIONS ANALYSIS BY AGE GROUP (Array) ===");
    std::cout << std::endl;

    printHorizontalLine();
    std::cout << std::setw(20) << std::left  << "Age Group"
              << " | " << std::setw(5)  << std::right << "Count"
              << " | " << std::setw(15) << std::right << "Total Emission"
              << " | " << std::setw(14) << std::right << "Avg Emission"
              << " | " << std::setw(18) << std::left  << "Top Mode"
              << std::endl;
    printHorizontalLine();

    double grandTotal = 0.0;
    int grandCount    = 0;

    for (int i = 0; i < 5; i++) {
        if (ageGroupCounts[i] == 0) continue;

        double avg = ageGroupEmissions[i] / ageGroupCounts[i];

        std::string topMode = "N/A";
        int maxCount = 0;
        for (int m = 0; m < modeCountsByAgeGroup[i]; m++) {
            if (modesByAgeGroup[i][m].count > maxCount) {
                maxCount = modesByAgeGroup[i][m].count;
                topMode  = modesByAgeGroup[i][m].mode;
            }
        }

        std::cout << std::setw(20) << std::left  << GROUP_SHORT[i]
                  << " | " << std::setw(5)  << std::right << ageGroupCounts[i]
                  << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << ageGroupEmissions[i]
                  << " | " << std::setw(14) << std::right << std::fixed << std::setprecision(2) << avg
                  << " | " << std::setw(18) << std::left  << topMode
                  << std::endl;

        grandTotal += ageGroupEmissions[i];
        grandCount += ageGroupCounts[i];
    }

    printHorizontalLine();
    double overallAvg = (grandCount > 0) ? (grandTotal / grandCount) : 0.0;
    std::cout << std::setw(20) << std::left  << "TOTAL"
              << " | " << std::setw(5)  << std::right << grandCount
              << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << grandTotal
              << " | " << std::setw(14) << std::right << std::fixed << std::setprecision(2) << overallAvg
              << " | " << std::setw(18) << std::left  << ""
              << std::endl;
    printHorizontalLine();
    std::cout << std::endl;
}

// ============================================================
// SECTION 4: Analyze Emissions by Transport Mode (Array)
// ============================================================

void analyzeEmissionsByModeArray(const ResidentArray& arr) {
    if (arr.count == 0) {
        std::cout << "No residents to analyze." << std::endl;
        return;
    }

    ModeInfo modes[20];
    int modeCount = 0;

    for (int i = 0; i < arr.count; i++) {
        bool modeFound = false;
        for (int m = 0; m < modeCount; m++) {
            if (strcmp(modes[m].mode, arr.data[i].modeOfTransport.c_str()) == 0) {
                modes[m].count++;
                modes[m].totalEmission += arr.data[i].monthlyEmission;
                modeFound = true;
                break;
            }
        }
        if (!modeFound && modeCount < 20) {
            strcpy_s(modes[modeCount].mode, arr.data[i].modeOfTransport.c_str());
            modes[modeCount].count = 1;
            modes[modeCount].totalEmission = arr.data[i].monthlyEmission;
            modeCount++;
        }
    }

    // Sort by total emission descending (bubble sort)
    for (int i = 0; i < modeCount - 1; i++) {
        for (int j = 0; j < modeCount - 1 - i; j++) {
            if (modes[j].totalEmission < modes[j + 1].totalEmission) {
                ModeInfo temp = modes[j];
                modes[j]     = modes[j + 1];
                modes[j + 1] = temp;
            }
        }
    }

    printCentered("=== CARBON EMISSIONS ANALYSIS BY TRANSPORT MODE (Array) ===");
    std::cout << std::endl;

    printHorizontalLine();
    std::cout << std::setw(20) << std::left  << "Transport Mode"
              << " | " << std::setw(5)  << std::right << "Count"
              << " | " << std::setw(15) << std::right << "Total Emission"
              << " | " << std::setw(14) << std::right << "Avg per User"
              << std::endl;
    printHorizontalLine();

    double grandTotal = 0.0;
    int grandCount    = 0;

    for (int i = 0; i < modeCount; i++) {
        double avg = (modes[i].count > 0) ? (modes[i].totalEmission / modes[i].count) : 0.0;
        std::cout << std::setw(20) << std::left  << modes[i].mode
                  << " | " << std::setw(5)  << std::right << modes[i].count
                  << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << modes[i].totalEmission
                  << " | " << std::setw(14) << std::right << std::fixed << std::setprecision(2) << avg
                  << std::endl;
        grandTotal += modes[i].totalEmission;
        grandCount += modes[i].count;
    }

    printHorizontalLine();
    double overallAvg = (grandCount > 0) ? (grandTotal / grandCount) : 0.0;
    std::cout << std::setw(20) << std::left  << "TOTAL"
              << " | " << std::setw(5)  << std::right << grandCount
              << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << grandTotal
              << " | " << std::setw(14) << std::right << std::fixed << std::setprecision(2) << overallAvg
              << std::endl;
    printHorizontalLine();
    std::cout << std::endl;
}

// ============================================================
// SECTION 5: Analyze Emissions by Age Group (Linked List)
// ============================================================

void analyzeEmissionsByAgeGroupList(const LinkedList& list) {
    if (list.size == 0) {
        std::cout << "No residents to analyze." << std::endl;
        return;
    }

    int ageGroupCounts[5]          = {0, 0, 0, 0, 0};
    double ageGroupEmissions[5]    = {0.0, 0.0, 0.0, 0.0, 0.0};
    ModeInfo modesByAgeGroup[5][10];
    int modeCountsByAgeGroup[5]    = {0, 0, 0, 0, 0};

    Node* current = list.head;
    while (current != nullptr) {
        int g = ageGroupIndex(current->resident.age);
        if (g != -1) {
            ageGroupCounts[g]++;
            ageGroupEmissions[g] += current->resident.monthlyEmission;

            bool modeFound = false;
            for (int m = 0; m < modeCountsByAgeGroup[g]; m++) {
                if (strcmp(modesByAgeGroup[g][m].mode, current->resident.modeOfTransport.c_str()) == 0) {
                    modesByAgeGroup[g][m].count++;
                    modesByAgeGroup[g][m].totalEmission += current->resident.monthlyEmission;
                    modeFound = true;
                    break;
                }
            }
            if (!modeFound && modeCountsByAgeGroup[g] < 10) {
                strcpy_s(modesByAgeGroup[g][modeCountsByAgeGroup[g]].mode,
                         current->resident.modeOfTransport.c_str());
                modesByAgeGroup[g][modeCountsByAgeGroup[g]].count = 1;
                modesByAgeGroup[g][modeCountsByAgeGroup[g]].totalEmission = current->resident.monthlyEmission;
                modeCountsByAgeGroup[g]++;
            }
        }
        current = current->next;
    }

    printCentered("=== CARBON EMISSIONS ANALYSIS BY AGE GROUP (Linked List) ===");
    std::cout << std::endl;

    printHorizontalLine();
    std::cout << std::setw(20) << std::left  << "Age Group"
              << " | " << std::setw(5)  << std::right << "Count"
              << " | " << std::setw(15) << std::right << "Total Emission"
              << " | " << std::setw(14) << std::right << "Avg Emission"
              << " | " << std::setw(18) << std::left  << "Top Mode"
              << std::endl;
    printHorizontalLine();

    double grandTotal = 0.0;
    int grandCount    = 0;

    for (int i = 0; i < 5; i++) {
        if (ageGroupCounts[i] == 0) continue;

        double avg = ageGroupEmissions[i] / ageGroupCounts[i];

        std::string topMode = "N/A";
        int maxCount = 0;
        for (int m = 0; m < modeCountsByAgeGroup[i]; m++) {
            if (modesByAgeGroup[i][m].count > maxCount) {
                maxCount = modesByAgeGroup[i][m].count;
                topMode  = modesByAgeGroup[i][m].mode;
            }
        }

        std::cout << std::setw(20) << std::left  << GROUP_SHORT[i]
                  << " | " << std::setw(5)  << std::right << ageGroupCounts[i]
                  << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << ageGroupEmissions[i]
                  << " | " << std::setw(14) << std::right << std::fixed << std::setprecision(2) << avg
                  << " | " << std::setw(18) << std::left  << topMode
                  << std::endl;

        grandTotal += ageGroupEmissions[i];
        grandCount += ageGroupCounts[i];
    }

    printHorizontalLine();
    double overallAvg = (grandCount > 0) ? (grandTotal / grandCount) : 0.0;
    std::cout << std::setw(20) << std::left  << "TOTAL"
              << " | " << std::setw(5)  << std::right << grandCount
              << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << grandTotal
              << " | " << std::setw(14) << std::right << std::fixed << std::setprecision(2) << overallAvg
              << " | " << std::setw(18) << std::left  << ""
              << std::endl;
    printHorizontalLine();
    std::cout << std::endl;
}

// ============================================================
// SECTION 6: Analyze Emissions by City (Linked List)
// ============================================================

void analyzeEmissionsByCityList(const LinkedList& list) {
    if (list.size == 0) {
        std::cout << "No residents to analyze." << std::endl;
        return;
    }

    const char* cities[]   = {"CityA", "CityB", "CityC"};
    int cityCounts[3]      = {0, 0, 0};
    double cityEmissions[3]= {0.0, 0.0, 0.0};
    ModeInfo modesByCity[3][10];
    int modeCountsByCity[3]= {0, 0, 0};

    Node* current = list.head;
    while (current != nullptr) {
        int cityIdx = -1;
        if (current->resident.cityLabel == "CityA") cityIdx = 0;
        else if (current->resident.cityLabel == "CityB") cityIdx = 1;
        else if (current->resident.cityLabel == "CityC") cityIdx = 2;

        if (cityIdx != -1) {
            cityCounts[cityIdx]++;
            cityEmissions[cityIdx] += current->resident.monthlyEmission;

            bool modeFound = false;
            for (int m = 0; m < modeCountsByCity[cityIdx]; m++) {
                if (strcmp(modesByCity[cityIdx][m].mode, current->resident.modeOfTransport.c_str()) == 0) {
                    modesByCity[cityIdx][m].count++;
                    modesByCity[cityIdx][m].totalEmission += current->resident.monthlyEmission;
                    modeFound = true;
                    break;
                }
            }
            if (!modeFound && modeCountsByCity[cityIdx] < 10) {
                strcpy_s(modesByCity[cityIdx][modeCountsByCity[cityIdx]].mode,
                         current->resident.modeOfTransport.c_str());
                modesByCity[cityIdx][modeCountsByCity[cityIdx]].count = 1;
                modesByCity[cityIdx][modeCountsByCity[cityIdx]].totalEmission = current->resident.monthlyEmission;
                modeCountsByCity[cityIdx]++;
            }
        }
        current = current->next;
    }

    printCentered("=== CARBON EMISSIONS ANALYSIS BY CITY (Linked List) ===");
    std::cout << std::endl;

    for (int c = 0; c < 3; c++) {
        if (cityCounts[c] == 0) continue;
        std::cout << "\n--- " << cities[c] << " ---\n";
        printHorizontalLine();
        std::cout << std::setw(20) << std::left  << "Transport Mode"
                  << " | " << std::setw(5)  << std::right << "Count"
                  << " | " << std::setw(15) << std::right << "Total Emission"
                  << " | " << std::setw(14) << std::right << "Avg per User"
                  << std::endl;
        printHorizontalLine();

        for (int m = 0; m < modeCountsByCity[c]; m++) {
            double avg = (modesByCity[c][m].count > 0) ?
                         (modesByCity[c][m].totalEmission / modesByCity[c][m].count) : 0.0;
            std::cout << std::setw(20) << std::left  << modesByCity[c][m].mode
                      << " | " << std::setw(5)  << std::right << modesByCity[c][m].count
                      << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << modesByCity[c][m].totalEmission
                      << " | " << std::setw(14) << std::right << std::fixed << std::setprecision(2) << avg
                      << std::endl;
        }
        printHorizontalLine();
        double avgCity = (cityCounts[c] > 0) ? (cityEmissions[c] / cityCounts[c]) : 0.0;
        std::cout << std::setw(20) << std::left  << "CITY TOTAL"
                  << " | " << std::setw(5)  << std::right << cityCounts[c]
                  << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << cityEmissions[c]
                  << " | " << std::setw(14) << std::right << std::fixed << std::setprecision(2) << avgCity
                  << std::endl;
    }

    printHorizontalLine();
    std::cout << std::endl;
}

// ============================================================
// SECTION 7: Internal helper - compute group stats from array
// ============================================================
// Fills counts[], avgEmissions[], topModes[] for 5 age groups.
static void computeGroupStats(const ResidentArray& arr,
                               int counts[5],
                               double avgEmissions[5],
                               std::string topModes[5]) {
    double totalEmissions[5] = {0.0};
    ModeInfo modes[5][10];
    int modeCounts[5] = {0};

    for (int i = 0; i < arr.count; i++) {
        int g = ageGroupIndex(arr.data[i].age);
        if (g == -1) continue;
        counts[g]++;
        totalEmissions[g] += arr.data[i].monthlyEmission;

        bool found = false;
        for (int m = 0; m < modeCounts[g]; m++) {
            if (strcmp(modes[g][m].mode, arr.data[i].modeOfTransport.c_str()) == 0) {
                modes[g][m].count++;
                found = true;
                break;
            }
        }
        if (!found && modeCounts[g] < 10) {
            strcpy_s(modes[g][modeCounts[g]].mode, arr.data[i].modeOfTransport.c_str());
            modes[g][modeCounts[g]].count = 1;
            modeCounts[g]++;
        }
    }

    for (int g = 0; g < 5; g++) {
        avgEmissions[g] = (counts[g] > 0) ? totalEmissions[g] / counts[g] : 0.0;
        int best = 0;
        for (int m = 1; m < modeCounts[g]; m++) {
            if (modes[g][m].count > modes[g][best].count) best = m;
        }
        topModes[g] = (modeCounts[g] > 0) ? modes[g][best].mode : "N/A";
    }
}

// ============================================================
// SECTION 8: Internal helper - compute group stats from linked list
// ============================================================
static void computeGroupStatsList(const LinkedList& list,
                                   int counts[5],
                                   double avgEmissions[5],
                                   std::string topModes[5]) {
    double totalEmissions[5] = {0.0};
    ModeInfo modes[5][10];
    int modeCounts[5] = {0};

    Node* cur = list.head;
    while (cur != nullptr) {
        int g = ageGroupIndex(cur->resident.age);
        if (g != -1) {
            counts[g]++;
            totalEmissions[g] += cur->resident.monthlyEmission;

            bool found = false;
            for (int m = 0; m < modeCounts[g]; m++) {
                if (strcmp(modes[g][m].mode, cur->resident.modeOfTransport.c_str()) == 0) {
                    modes[g][m].count++;
                    found = true;
                    break;
                }
            }
            if (!found && modeCounts[g] < 10) {
                strcpy_s(modes[g][modeCounts[g]].mode, cur->resident.modeOfTransport.c_str());
                modes[g][modeCounts[g]].count = 1;
                modeCounts[g]++;
            }
        }
        cur = cur->next;
    }

    for (int g = 0; g < 5; g++) {
        avgEmissions[g] = (counts[g] > 0) ? totalEmissions[g] / counts[g] : 0.0;
        int best = 0;
        for (int m = 1; m < modeCounts[g]; m++) {
            if (modes[g][m].count > modes[g][best].count) best = m;
        }
        topModes[g] = (modeCounts[g] > 0) ? modes[g][best].mode : "N/A";
    }
}

// ============================================================
// SECTION 9: Shared recommendation printer
// ============================================================
static void printRecommendationBody(int totalResidents,
                                     int counts[5],
                                     double avgEmissions[5],
                                     std::string topModes[5]) {
    // Find highest and lowest emitting groups
    int highestGroup = 0, lowestGroup = -1;
    for (int g = 0; g < 5; g++) {
        if (avgEmissions[g] > avgEmissions[highestGroup]) highestGroup = g;
        if (counts[g] > 0) {
            if (lowestGroup == -1 || avgEmissions[g] < avgEmissions[lowestGroup])
                lowestGroup = g;
        }
    }
    if (lowestGroup == -1) lowestGroup = 0;

    // --- Header ---
    std::cout << "\n\n";
    for (int i = 0; i < 80; i++) std::cout << "=";
    std::cout << "\n";
    printCentered("CITY PLANNER RECOMMENDATIONS - SUSTAINABLE COMMUTING");
    for (int i = 0; i < 80; i++) std::cout << "=";
    std::cout << "\n\n";

    // --- Risk assessment table ---
    std::cout << "EMISSION RISK ASSESSMENT BY AGE GROUP (" << totalResidents << " residents):\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::setw(22) << std::left  << "Age Group"
              << " | " << std::setw(5)  << std::right << "Count"
              << " | " << std::setw(11) << std::right << "Avg(kg CO2)"
              << " | " << std::setw(15) << std::left  << "Top Mode"
              << " | Risk\n";
    std::cout << std::string(80, '-') << "\n";

    for (int g = 0; g < 5; g++) {
        if (counts[g] == 0) continue;
        const char* risk = (avgEmissions[g] > 80) ? "HIGH" :
                           (avgEmissions[g] > 40) ? "MEDIUM" : "LOW";
        std::cout << std::setw(22) << std::left  << GROUP_SHORT[g]
                  << " | " << std::setw(5)  << std::right << counts[g]
                  << " | " << std::setw(8)  << std::right << std::fixed << std::setprecision(1)
                            << avgEmissions[g] << " kg"
                  << " | " << std::setw(15) << std::left  << topModes[g]
                  << " | " << risk << "\n";
    }
    std::cout << std::string(80, '-') << "\n\n";

    // --- Per-group policy recommendations ---
    std::cout << "TARGETED POLICY RECOMMENDATIONS:\n";
    std::cout << std::string(80, '-') << "\n\n";

    std::cout << "[" << GROUP_FULL[0] << "]\n";
    std::cout << "  Evidence:  Avg " << std::fixed << std::setprecision(1)
              << avgEmissions[0] << " kg CO2/month | Top mode: " << topModes[0] << "\n";
    std::cout << "  Policy 1:  Build safe cycling routes and dedicated school crossing zones\n";
    std::cout << "  Policy 2:  Active travel programme (walking/cycling challenges in schools)\n";
    std::cout << "  Policy 3:  Subsidised school bus services to replace private car drop-offs\n\n";

    std::cout << "[" << GROUP_FULL[1] << "]\n";
    std::cout << "  Evidence:  Avg " << std::fixed << std::setprecision(1)
              << avgEmissions[1] << " kg CO2/month | Top mode: " << topModes[1] << "\n";
    std::cout << "  Policy 1:  Subsidise monthly bus and bicycle-sharing passes for students\n";
    std::cout << "  Policy 2:  Expand dedicated cycling lanes near university campuses\n";
    std::cout << "  Policy 3:  University carpooling app and green commuting incentive scheme\n\n";

    std::cout << "[" << GROUP_FULL[2] << "]";
    if (highestGroup == 2) std::cout << "  ** HIGHEST PRIORITY **";
    std::cout << "\n";
    std::cout << "  Evidence:  Avg " << std::fixed << std::setprecision(1)
              << avgEmissions[2] << " kg CO2/month | Top mode: " << topModes[2] << "\n";
    std::cout << "  Policy 1:  Employer-sponsored transit passes (government tax incentive)\n";
    std::cout << "  Policy 2:  Expand park-and-ride facilities near major business districts\n";
    std::cout << "  Policy 3:  HOV (High-Occupancy Vehicle) lanes and carpooling discounts\n";
    std::cout << "  Policy 4:  Flexible work hours to reduce peak-hour congestion\n\n";

    std::cout << "[" << GROUP_FULL[3] << "]\n";
    std::cout << "  Evidence:  Avg " << std::fixed << std::setprecision(1)
              << avgEmissions[3] << " kg CO2/month | Top mode: " << topModes[3] << "\n";
    std::cout << "  Policy 1:  Work-from-home policy support to reduce commuting frequency\n";
    std::cout << "  Policy 2:  E-bicycle subsidy scheme for commutes under 10 km\n";
    std::cout << "  Policy 3:  Improved bus frequency on suburban-to-city corridors\n\n";

    std::cout << "[" << GROUP_FULL[4] << "]\n";
    std::cout << "  Evidence:  Avg " << std::fixed << std::setprecision(1)
              << avgEmissions[4] << " kg CO2/month | Top mode: " << topModes[4] << "\n";
    std::cout << "  Policy 1:  Free or heavily discounted senior transit passes\n";
    std::cout << "  Policy 2:  Accessible public transport (low-floor buses, priority seating)\n";
    std::cout << "  Policy 3:  Age-friendly walking infrastructure (wider paths, rest areas)\n\n";

    // --- Overall top 5 ---
    std::cout << std::string(80, '-') << "\n";
    std::cout << "TOP 5 OVERALL POLICY PRIORITIES (highest-impact first):\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << "  1. [URGENT] Reduce car use in " << GROUP_FULL[highestGroup]
              << "\n     -> Highest avg emission: " << std::fixed << std::setprecision(1)
              << avgEmissions[highestGroup] << " kg CO2/month\n";
    std::cout << "  2. Invest in city-wide cycling infrastructure (benefits ALL age groups)\n";
    std::cout << "  3. Expand bus routes and frequency during peak hours (7-9am, 5-7pm)\n";
    std::cout << "  4. Introduce congestion pricing in high-emission transport corridors\n";
    std::cout << "  5. Launch carbon emission public awareness campaign across all 3 cities\n";
    std::cout << std::string(80, '=') << "\n";
}

// ============================================================
// SECTION 10: City Planner Recommendations (Array)
// ============================================================

void printCityPlannerRecommendations(const ResidentArray& arr) {
    if (arr.count == 0) {
        std::cout << "No data available for recommendations.\n";
        return;
    }
    int counts[5] = {0};
    double avgEmissions[5] = {0.0};
    std::string topModes[5];
    computeGroupStats(arr, counts, avgEmissions, topModes);
    printRecommendationBody(arr.count, counts, avgEmissions, topModes);
}

// ============================================================
// SECTION 11: City Planner Recommendations (Linked List)
// ============================================================

void printCityPlannerRecommendationsList(const LinkedList& list) {
    if (list.size == 0) {
        std::cout << "No data available for recommendations.\n";
        return;
    }
    int counts[5] = {0};
    double avgEmissions[5] = {0.0};
    std::string topModes[5];
    computeGroupStatsList(list, counts, avgEmissions, topModes);
    printRecommendationBody(list.size, counts, avgEmissions, topModes);
}
