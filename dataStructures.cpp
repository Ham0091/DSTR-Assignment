#include "dataStructures.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

// ============================================================
// SECTION 1: loadCSV Implementation for ResidentArray
// ============================================================
// Loads CSV file into fixed-size array.
// Parses each line and calculates monthlyEmission.
// Handles errors: missing file, wrong format, exceeding capacity.
void loadCSV(ResidentArray& arr, const std::string& filename, const std::string& cityLabel) {
    std::ifstream file(filename);
    
    // Error handling: Check if file opens successfully
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return;
    }
    
    std::string line;
    int lineNumber = 0;
    
    // Skip header line
    if (!std::getline(file, line)) {
        std::cerr << "Error: File '" << filename << "' is empty or cannot be read." << std::endl;
        file.close();
        return;
    }
    
    // Read each data line
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Check if we've exceeded capacity
        if (arr.count >= MAX_SIZE) {
            std::cerr << "Warning: Maximum capacity (600) reached. Stopping data loading." << std::endl;
            break;
        }
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Parse the CSV line
        std::istringstream iss(line);
        std::string residentID, modeOfTransport, ageStr, distanceStr, factorStr, daysStr;
        
        try {
            // Parse CSV fields (comma-separated)
            if (!std::getline(iss, residentID, ',')) throw std::runtime_error("Missing ResidentID");
            if (!std::getline(iss, ageStr, ',')) throw std::runtime_error("Missing Age");
            if (!std::getline(iss, modeOfTransport, ',')) throw std::runtime_error("Missing ModeOfTransport");
            if (!std::getline(iss, distanceStr, ',')) throw std::runtime_error("Missing DailyDistance");
            if (!std::getline(iss, factorStr, ',')) throw std::runtime_error("Missing CarbonEmissionFactor");
            if (!std::getline(iss, daysStr, ',')) throw std::runtime_error("Missing AvgDaysPerMonth");
            
            // Convert strings to appropriate types
            int age = std::stoi(ageStr);
            double dailyDistance = std::stod(distanceStr);
            double carbonEmissionFactor = std::stod(factorStr);
            int avgDaysPerMonth = std::stoi(daysStr);
            
            // Calculate monthlyEmission
            double monthlyEmission = dailyDistance * carbonEmissionFactor * avgDaysPerMonth;
            
            // Store resident in array
            Resident resident;
            resident.residentID = residentID;
            resident.age = age;
            resident.modeOfTransport = modeOfTransport;
            resident.dailyDistance = dailyDistance;
            resident.carbonEmissionFactor = carbonEmissionFactor;
            resident.avgDaysPerMonth = avgDaysPerMonth;
            resident.cityLabel = cityLabel;
            resident.monthlyEmission = monthlyEmission;
            
            arr.data[arr.count++] = resident;
            
        } catch (const std::exception& e) {
            std::cerr << "Warning: Line " << lineNumber << " in '" << filename 
                      << "' has incorrect format: " << e.what() << " - skipping this line." << std::endl;
        }
    }
    
    file.close();
    std::cout << "Loaded " << arr.count << " residents from array: " << filename << std::endl;
}

// ============================================================
// SECTION 2: loadCSV Implementation for LinkedList
// ============================================================
// Loads CSV file into linked list.
// Same parsing logic as array version, but appends to linked list.
// Verifies list.size increases correctly.
void loadCSV(LinkedList& list, const std::string& filename, const std::string& cityLabel) {
    std::ifstream file(filename);
    
    // Error handling: Check if file opens successfully
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return;
    }
    
    std::string line;
    int lineNumber = 0;
    int initialSize = list.size;
    
    // Skip header line
    if (!std::getline(file, line)) {
        std::cerr << "Error: File '" << filename << "' is empty or cannot be read." << std::endl;
        file.close();
        return;
    }
    
    // Read each data line
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Parse the CSV line
        std::istringstream iss(line);
        std::string residentID, modeOfTransport, ageStr, distanceStr, factorStr, daysStr;
        
        try {
            // Parse CSV fields (comma-separated)
            if (!std::getline(iss, residentID, ',')) throw std::runtime_error("Missing ResidentID");
            if (!std::getline(iss, ageStr, ',')) throw std::runtime_error("Missing Age");
            if (!std::getline(iss, modeOfTransport, ',')) throw std::runtime_error("Missing ModeOfTransport");
            if (!std::getline(iss, distanceStr, ',')) throw std::runtime_error("Missing DailyDistance");
            if (!std::getline(iss, factorStr, ',')) throw std::runtime_error("Missing CarbonEmissionFactor");
            if (!std::getline(iss, daysStr, ',')) throw std::runtime_error("Missing AvgDaysPerMonth");
            
            // Convert strings to appropriate types
            int age = std::stoi(ageStr);
            double dailyDistance = std::stod(distanceStr);
            double carbonEmissionFactor = std::stod(factorStr);
            int avgDaysPerMonth = std::stoi(daysStr);
            
            // Calculate monthlyEmission
            double monthlyEmission = dailyDistance * carbonEmissionFactor * avgDaysPerMonth;
            
            // Create resident object
            Resident resident;
            resident.residentID = residentID;
            resident.age = age;
            resident.modeOfTransport = modeOfTransport;
            resident.dailyDistance = dailyDistance;
            resident.carbonEmissionFactor = carbonEmissionFactor;
            resident.avgDaysPerMonth = avgDaysPerMonth;
            resident.cityLabel = cityLabel;
            resident.monthlyEmission = monthlyEmission;
            
            // Append to linked list
            list.append(resident);
            
        } catch (const std::exception& e) {
            std::cerr << "Warning: Line " << lineNumber << " in '" << filename 
                      << "' has incorrect format: " << e.what() << " - skipping this line." << std::endl;
        }
    }
    
    file.close();
    int loadedCount = list.size - initialSize;
    std::cout << "Loaded " << loadedCount << " residents into linked list from: " << filename << std::endl;
}

// ============================================================
// SECTION 3: printResident Implementation
// ============================================================
// Display a single resident in formatted table row.
// Shows all key information: ID, Age, Mode, Distance, Factor, Days, MonthlyEmission
void printResident(const Resident& r) {
    std::cout << std::setw(15) << std::left << r.residentID
              << " | " << std::setw(5) << std::right << r.age
              << " | " << std::setw(20) << std::left << r.modeOfTransport
              << " | " << std::setw(10) << std::fixed << std::setprecision(2) << r.dailyDistance
              << " | " << std::setw(10) << std::fixed << std::setprecision(3) << r.carbonEmissionFactor
              << " | " << std::setw(5) << r.avgDaysPerMonth
              << " | " << std::setw(12) << std::fixed << std::setprecision(2) << r.monthlyEmission
              << std::endl;
}

// ============================================================
// SECTION 4: printHorizontalLine Implementation
// ============================================================
// Print a horizontal line for table borders.
// Adjusted width to match the printResident format.
void printHorizontalLine() {
    std::cout << std::string(120, '-') << std::endl;
}

// ============================================================
// SECTION 5: printCentered Implementation
// ============================================================
// Center text in console output by adding padding.
// Assumes console width of 80 characters (adjustable).
void printCentered(const std::string& text) {
    const int consoleWidth = 100;
    int padding = (consoleWidth - text.length()) / 2;
    
    if (padding > 0) {
        std::cout << std::string(padding, ' ');
    }
    std::cout << text << std::endl;
}
