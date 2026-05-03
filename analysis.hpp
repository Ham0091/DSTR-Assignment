#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP

#include "dataStructures.hpp"
#include <string>

// ----- age group helpers (used by reports) -----

// map age -> full label used in the tables
// returns: "6-17: Children", "18-25: University", "26-45: Working (Early)",
//          "46-60: Working (Late)", "61-100: Senior"
std::string getAgeGroup(int age);

// map age -> short code for searches
// returns: "6-17", "18-25", "26-45", "46-60", "61+"
std::string getAgeGroupCode(int age);

// ---- array analysis bits ----

// breakdown by age group (count/total/avg + top mode)
void analyzeEmissionsByAgeGroupArray(const ResidentArray& arr);

// breakdown by transport mode (sorted by total emission desc)
void analyzeEmissionsByModeArray(const ResidentArray& arr);

// ---- linked list analysis ----

// same as array output, but walks the list
void analyzeEmissionsByAgeGroupList(const LinkedList& list);

// compare CityA/CityB/CityC totals and modes
void analyzeEmissionsByCityList(const LinkedList& list);

// ---- city planner recs ----

// print policy recs for array data
void printCityPlannerRecommendations(const ResidentArray& arr);

// print policy recs for linked list data
void printCityPlannerRecommendationsList(const LinkedList& list);

#endif // analysis.hpp guard
