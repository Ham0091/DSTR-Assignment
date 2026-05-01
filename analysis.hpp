#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP

#include "dataStructures.hpp"
#include <string>

// ============================================================
// SECTION 1: Age Group Classification Functions
// ============================================================

// Convert age to age group name
// Returns: "6-17: Children", "18-25: University", "26-45: Working (Early)",
//          "46-60: Working (Late)", "61-100: Senior"
std::string getAgeGroup(int age);

// Convert age to age group code (for searching)
// Returns: "6-17", "18-25", "26-45", "46-60", "61+"
std::string getAgeGroupCode(int age);

// ============================================================
// SECTION 2: Analysis Functions for ResidentArray
// ============================================================

// Analyze and display emissions by age group
// Shows: count, total emissions, average emission, most used transport mode
void analyzeEmissionsByAgeGroupArray(const ResidentArray& arr);

// Analyze and display emissions by transport mode
// Shows: count, total emissions, average emission
// Sorted by total emissions (highest first)
void analyzeEmissionsByModeArray(const ResidentArray& arr);

// ============================================================
// SECTION 3: Analysis Functions for LinkedList
// ============================================================

// Analyze and display emissions by age group in linked list
// Same output as array version but uses pointer traversal
void analyzeEmissionsByAgeGroupList(const LinkedList& list);

// Analyze and display emissions by city in linked list
// Compares emissions across CityA, CityB, CityC
// Shows breakdown by transport mode per city
void analyzeEmissionsByCityList(const LinkedList& list);

// ============================================================
// SECTION 4: City Planner Recommendation Functions
// ============================================================

// Print evidence-based policy recommendations for city planners (array)
void printCityPlannerRecommendations(const ResidentArray& arr);

// Print evidence-based policy recommendations for city planners (linked list)
void printCityPlannerRecommendationsList(const LinkedList& list);

#endif // ANALYSIS_HPP
