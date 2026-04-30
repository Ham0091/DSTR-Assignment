#include "analysis.hpp"
#include <iostream>
#include <iomanip>
#include <cstring>

// ============================================================
// SECTION 1: Age Group Classification Implementation
// ============================================================

std::string getAgeGroup(int age) {
    if (age >= 6 && age <= 17) {
        return "6-17: Children";
    } else if (age >= 18 && age <= 25) {
        return "18-25: University";
    } else if (age >= 26 && age <= 45) {
        return "26-45: Working (Early)";
    } else if (age >= 46 && age <= 60) {
        return "46-60: Working (Late)";
    } else if (age >= 61 && age <= 100) {
        return "61-100: Senior";
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

// ============================================================
// SECTION 3: Analyze Emissions by Age Group (Array)
// ============================================================

void analyzeEmissionsByAgeGroupArray(const ResidentArray& arr) {
    if (arr.count == 0) {
        std::cout << "No residents to analyze." << std::endl;
        return;
    }
    
    // Arrays for age group data (5 age groups)
    int ageGroupCounts[5] = {0, 0, 0, 0, 0};           // Count per age group
    double ageGroupEmissions[5] = {0.0, 0.0, 0.0, 0.0, 0.0};  // Total emissions
    
    // Array for transport modes per age group (max 6 modes × 5 age groups)
    ModeInfo modesByAgeGroup[5][10];
    int modeCountsByAgeGroup[5] = {0, 0, 0, 0, 0};
    
    // Age group names for indexing
    const char* ageGroupNames[] = {"6-17", "18-25", "26-45", "46-60", "61+"};
    
    // Process each resident
    for (int i = 0; i < arr.count; i++) {
        int ageGroupIdx = -1;
        
        // Determine age group index
        if (arr.data[i].age >= 6 && arr.data[i].age <= 17) {
            ageGroupIdx = 0;
        } else if (arr.data[i].age >= 18 && arr.data[i].age <= 25) {
            ageGroupIdx = 1;
        } else if (arr.data[i].age >= 26 && arr.data[i].age <= 45) {
            ageGroupIdx = 2;
        } else if (arr.data[i].age >= 46 && arr.data[i].age <= 60) {
            ageGroupIdx = 3;
        } else if (arr.data[i].age >= 61 && arr.data[i].age <= 100) {
            ageGroupIdx = 4;
        }
        
        if (ageGroupIdx == -1) continue;
        
        // Count and emissions
        ageGroupCounts[ageGroupIdx]++;
        ageGroupEmissions[ageGroupIdx] += arr.data[i].monthlyEmission;
        
        // Track modes for this age group
        bool modeFound = false;
        for (int m = 0; m < modeCountsByAgeGroup[ageGroupIdx]; m++) {
            if (strcmp(modesByAgeGroup[ageGroupIdx][m].mode, arr.data[i].modeOfTransport.c_str()) == 0) {
                modesByAgeGroup[ageGroupIdx][m].count++;
                modesByAgeGroup[ageGroupIdx][m].totalEmission += arr.data[i].monthlyEmission;
                modeFound = true;
                break;
            }
        }
        
        if (!modeFound && modeCountsByAgeGroup[ageGroupIdx] < 10) {
            strcpy_s(modesByAgeGroup[ageGroupIdx][modeCountsByAgeGroup[ageGroupIdx]].mode, 
                     arr.data[i].modeOfTransport.c_str());
            modesByAgeGroup[ageGroupIdx][modeCountsByAgeGroup[ageGroupIdx]].count = 1;
            modesByAgeGroup[ageGroupIdx][modeCountsByAgeGroup[ageGroupIdx]].totalEmission = arr.data[i].monthlyEmission;
            modeCountsByAgeGroup[ageGroupIdx]++;
        }
    }
    
    // Display results
    printCentered("=== CARBON EMISSIONS ANALYSIS BY AGE GROUP ===");
    std::cout << std::endl;
    
    printHorizontalLine();
    std::cout << std::setw(15) << std::left << "Age Group"
              << " | " << std::setw(12) << std::right << "Count"
              << " | " << std::setw(15) << std::right << "Total Emission"
              << " | " << std::setw(15) << std::right << "Avg Emission"
              << " | " << std::setw(20) << std::left << "Top Mode"
              << std::endl;
    printHorizontalLine();
    
    double grandTotalEmission = 0.0;
    int grandTotalCount = 0;
    
    for (int i = 0; i < 5; i++) {
        if (ageGroupCounts[i] == 0) continue;
        
        double avgEmission = ageGroupEmissions[i] / ageGroupCounts[i];
        
        // Find most used mode
        std::string topMode = "N/A";
        int maxModeCount = 0;
        for (int m = 0; m < modeCountsByAgeGroup[i]; m++) {
            if (modesByAgeGroup[i][m].count > maxModeCount) {
                maxModeCount = modesByAgeGroup[i][m].count;
                topMode = modesByAgeGroup[i][m].mode;
            }
        }
        
        std::cout << std::setw(15) << std::left << ageGroupNames[i]
                  << " | " << std::setw(12) << std::right << ageGroupCounts[i]
                  << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << ageGroupEmissions[i]
                  << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << avgEmission
                  << " | " << std::setw(20) << std::left << topMode
                  << std::endl;
        
        grandTotalEmission += ageGroupEmissions[i];
        grandTotalCount += ageGroupCounts[i];
    }
    
    printHorizontalLine();
    double overallAvg = (grandTotalCount > 0) ? (grandTotalEmission / grandTotalCount) : 0.0;
    std::cout << std::setw(15) << std::left << "TOTAL"
              << " | " << std::setw(12) << std::right << grandTotalCount
              << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << grandTotalEmission
              << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << overallAvg
              << " | " << std::setw(20) << std::left << ""
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
    
    // Array to track modes (max 20 different modes)
    ModeInfo modes[20];
    int modeCount = 0;
    
    // Count residents and emissions for each mode
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
    
    // Sort modes by total emission (highest first) using bubble sort
    for (int i = 0; i < modeCount - 1; i++) {
        for (int j = 0; j < modeCount - 1 - i; j++) {
            if (modes[j].totalEmission < modes[j + 1].totalEmission) {
                ModeInfo temp = modes[j];
                modes[j] = modes[j + 1];
                modes[j + 1] = temp;
            }
        }
    }
    
    // Display results
    printCentered("=== CARBON EMISSIONS ANALYSIS BY TRANSPORT MODE ===");
    std::cout << std::endl;
    
    printHorizontalLine();
    std::cout << std::setw(20) << std::left << "Transport Mode"
              << " | " << std::setw(12) << std::right << "Count"
              << " | " << std::setw(15) << std::right << "Total Emission"
              << " | " << std::setw(15) << std::right << "Avg per User"
              << std::endl;
    printHorizontalLine();
    
    double grandTotalEmission = 0.0;
    int grandTotalCount = 0;
    
    for (int i = 0; i < modeCount; i++) {
        double avgEmission = (modes[i].count > 0) ? (modes[i].totalEmission / modes[i].count) : 0.0;
        
        std::cout << std::setw(20) << std::left << modes[i].mode
                  << " | " << std::setw(12) << std::right << modes[i].count
                  << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << modes[i].totalEmission
                  << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << avgEmission
                  << std::endl;
        
        grandTotalEmission += modes[i].totalEmission;
        grandTotalCount += modes[i].count;
    }
    
    printHorizontalLine();
    double overallAvg = (grandTotalCount > 0) ? (grandTotalEmission / grandTotalCount) : 0.0;
    std::cout << std::setw(20) << std::left << "TOTAL"
              << " | " << std::setw(12) << std::right << grandTotalCount
              << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << grandTotalEmission
              << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << overallAvg
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
    
    // Arrays for age group data (5 age groups)
    int ageGroupCounts[5] = {0, 0, 0, 0, 0};
    double ageGroupEmissions[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
    
    // Array for transport modes per age group
    ModeInfo modesByAgeGroup[5][10];
    int modeCountsByAgeGroup[5] = {0, 0, 0, 0, 0};
    
    const char* ageGroupNames[] = {"6-17", "18-25", "26-45", "46-60", "61+"};
    
    // Traverse linked list using pointers
    Node* current = list.head;
    while (current != nullptr) {
        int ageGroupIdx = -1;
        
        // Determine age group index
        if (current->resident.age >= 6 && current->resident.age <= 17) {
            ageGroupIdx = 0;
        } else if (current->resident.age >= 18 && current->resident.age <= 25) {
            ageGroupIdx = 1;
        } else if (current->resident.age >= 26 && current->resident.age <= 45) {
            ageGroupIdx = 2;
        } else if (current->resident.age >= 46 && current->resident.age <= 60) {
            ageGroupIdx = 3;
        } else if (current->resident.age >= 61 && current->resident.age <= 100) {
            ageGroupIdx = 4;
        }
        
        if (ageGroupIdx != -1) {
            // Count and emissions
            ageGroupCounts[ageGroupIdx]++;
            ageGroupEmissions[ageGroupIdx] += current->resident.monthlyEmission;
            
            // Track modes for this age group
            bool modeFound = false;
            for (int m = 0; m < modeCountsByAgeGroup[ageGroupIdx]; m++) {
                if (strcmp(modesByAgeGroup[ageGroupIdx][m].mode, current->resident.modeOfTransport.c_str()) == 0) {
                    modesByAgeGroup[ageGroupIdx][m].count++;
                    modesByAgeGroup[ageGroupIdx][m].totalEmission += current->resident.monthlyEmission;
                    modeFound = true;
                    break;
                }
            }
            
            if (!modeFound && modeCountsByAgeGroup[ageGroupIdx] < 10) {
                strcpy_s(modesByAgeGroup[ageGroupIdx][modeCountsByAgeGroup[ageGroupIdx]].mode, 
                         current->resident.modeOfTransport.c_str());
                modesByAgeGroup[ageGroupIdx][modeCountsByAgeGroup[ageGroupIdx]].count = 1;
                modesByAgeGroup[ageGroupIdx][modeCountsByAgeGroup[ageGroupIdx]].totalEmission = current->resident.monthlyEmission;
                modeCountsByAgeGroup[ageGroupIdx]++;
            }
        }
        
        current = current->next;
    }
    
    // Display results (same format as array version)
    printCentered("=== CARBON EMISSIONS ANALYSIS BY AGE GROUP (Linked List) ===");
    std::cout << std::endl;
    
    printHorizontalLine();
    std::cout << std::setw(15) << std::left << "Age Group"
              << " | " << std::setw(12) << std::right << "Count"
              << " | " << std::setw(15) << std::right << "Total Emission"
              << " | " << std::setw(15) << std::right << "Avg Emission"
              << " | " << std::setw(20) << std::left << "Top Mode"
              << std::endl;
    printHorizontalLine();
    
    double grandTotalEmission = 0.0;
    int grandTotalCount = 0;
    
    for (int i = 0; i < 5; i++) {
        if (ageGroupCounts[i] == 0) continue;
        
        double avgEmission = ageGroupEmissions[i] / ageGroupCounts[i];
        
        // Find most used mode
        std::string topMode = "N/A";
        int maxModeCount = 0;
        for (int m = 0; m < modeCountsByAgeGroup[i]; m++) {
            if (modesByAgeGroup[i][m].count > maxModeCount) {
                maxModeCount = modesByAgeGroup[i][m].count;
                topMode = modesByAgeGroup[i][m].mode;
            }
        }
        
        std::cout << std::setw(15) << std::left << ageGroupNames[i]
                  << " | " << std::setw(12) << std::right << ageGroupCounts[i]
                  << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << ageGroupEmissions[i]
                  << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << avgEmission
                  << " | " << std::setw(20) << std::left << topMode
                  << std::endl;
        
        grandTotalEmission += ageGroupEmissions[i];
        grandTotalCount += ageGroupCounts[i];
    }
    
    printHorizontalLine();
    double overallAvg = (grandTotalCount > 0) ? (grandTotalEmission / grandTotalCount) : 0.0;
    std::cout << std::setw(15) << std::left << "TOTAL"
              << " | " << std::setw(12) << std::right << grandTotalCount
              << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << grandTotalEmission
              << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) << overallAvg
              << " | " << std::setw(20) << std::left << ""
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
    
    // City data: 3 cities
    const char* cities[] = {"CityA", "CityB", "CityC"};
    int cityCounts[3] = {0, 0, 0};
    double cityEmissions[3] = {0.0, 0.0, 0.0};
    
    // Modes per city (max 10 modes per city)
    ModeInfo modesByCity[3][10];
    int modeCountsByCity[3] = {0, 0, 0};
    
    // Traverse linked list
    Node* current = list.head;
    while (current != nullptr) {
        int cityIdx = -1;
        
        // Determine city index
        if (current->resident.cityLabel == "CityA") {
            cityIdx = 0;
        } else if (current->resident.cityLabel == "CityB") {
            cityIdx = 1;
        } else if (current->resident.cityLabel == "CityC") {
            cityIdx = 2;
        }
        
        if (cityIdx != -1) {
            // Count and emissions
            cityCounts[cityIdx]++;
            cityEmissions[cityIdx] += current->resident.monthlyEmission;
            
            // Track modes for this city
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
    
    // Display results
    printCentered("=== CARBON EMISSIONS ANALYSIS BY CITY ===");
    std::cout << std::endl;
    
    for (int c = 0; c < 3; c++) {
        if (cityCounts[c] == 0) continue;
        
        std::cout << "\n--- " << cities[c] << " ---" << std::endl;
        printHorizontalLine();
        std::cout << std::setw(20) << std::left << "Transport Mode"
                  << " | " << std::setw(12) << std::right << "Count"
                  << " | " << std::setw(15) << std::right << "Total Emission"
                  << " | " << std::setw(15) << std::right << "Avg per User"
                  << std::endl;
        printHorizontalLine();
        
        for (int m = 0; m < modeCountsByCity[c]; m++) {
            double avgEmission = (modesByCity[c][m].count > 0) ? 
                                (modesByCity[c][m].totalEmission / modesByCity[c][m].count) : 0.0;
            
            std::cout << std::setw(20) << std::left << modesByCity[c][m].mode
                      << " | " << std::setw(12) << std::right << modesByCity[c][m].count
                      << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) 
                      << modesByCity[c][m].totalEmission
                      << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) 
                      << avgEmission
                      << std::endl;
        }
        
        printHorizontalLine();
        double avgCityEmission = (cityCounts[c] > 0) ? (cityEmissions[c] / cityCounts[c]) : 0.0;
        std::cout << std::setw(20) << std::left << "CITY TOTAL"
                  << " | " << std::setw(12) << std::right << cityCounts[c]
                  << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) 
                  << cityEmissions[c]
                  << " | " << std::setw(15) << std::right << std::fixed << std::setprecision(2) 
                  << avgCityEmission
                  << std::endl;
    }
    
    printHorizontalLine();
    std::cout << std::endl;
}
