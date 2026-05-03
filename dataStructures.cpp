#include "dataStructures.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

// --- loadCSV for ResidentArray ---
// read CSV into the fixed array + compute monthlyEmission
// basic guards for missing file / bad rows / overflow
void loadCSV(ResidentArray& arr, const std::string& filename, const std::string& cityLabel) {
    std::ifstream file(filename);
    
    // bail early if file won't open
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return;
    }
    
    std::string line;
    int lineNumber = 0;
    
    // toss the header row
    if (!std::getline(file, line)) {
        std::cerr << "Error: File '" << filename << "' is empty or cannot be read." << std::endl;
        file.close();
        return;
    }
    
    // rest of the lines are data
    while (std::getline(file, line)) {
        lineNumber++;
        
        // stop if we hit MAX_SIZE
        if (arr.count >= MAX_SIZE) {
            std::cerr << "Warning: Maximum capacity (600) reached. Stopping data loading." << std::endl;
            break;
        }
        
        // ignore blank lines
        if (line.empty()) {
            continue;
        }
        
        // split the CSV line
        std::istringstream iss(line);
        std::string residentID, modeOfTransport, ageStr, distanceStr, factorStr, daysStr;
        
        try {
            // pull fields out, throw if any are missing
            if (!std::getline(iss, residentID, ',')) throw std::runtime_error("Missing ResidentID");
            if (!std::getline(iss, ageStr, ',')) throw std::runtime_error("Missing Age");
            if (!std::getline(iss, modeOfTransport, ',')) throw std::runtime_error("Missing ModeOfTransport");
            if (!std::getline(iss, distanceStr, ',')) throw std::runtime_error("Missing DailyDistance");
            if (!std::getline(iss, factorStr, ',')) throw std::runtime_error("Missing CarbonEmissionFactor");
            if (!std::getline(iss, daysStr, ',')) throw std::runtime_error("Missing AvgDaysPerMonth");
            
            // string -> numbers
            int age = std::stoi(ageStr);
            double dailyDistance = std::stod(distanceStr);
            double carbonEmissionFactor = std::stod(factorStr);
            int avgDaysPerMonth = std::stoi(daysStr);
            
            // monthlyEmission calc (daily * factor * days)
            double monthlyEmission = dailyDistance * carbonEmissionFactor * avgDaysPerMonth;
            
            // stash in array slot
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

// --- loadCSV for LinkedList ---
// same parsing as array version, just append nodes
// counts new nodes so we can print a decent summary
void loadCSV(LinkedList& list, const std::string& filename, const std::string& cityLabel) {
    std::ifstream file(filename);
    
    // can't open? just bail
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return;
    }
    
    std::string line;
    int lineNumber = 0;
    int initialSize = list.size;
    
    // skip the header row
    if (!std::getline(file, line)) {
        std::cerr << "Error: File '" << filename << "' is empty or cannot be read." << std::endl;
        file.close();
        return;
    }
    
    // data rows
    while (std::getline(file, line)) {
        lineNumber++;
        
        // ignore empty rows
        if (line.empty()) {
            continue;
        }
        
        // split the row
        std::istringstream iss(line);
        std::string residentID, modeOfTransport, ageStr, distanceStr, factorStr, daysStr;
        
        try {
            // pull fields, complain if any are missing
            if (!std::getline(iss, residentID, ',')) throw std::runtime_error("Missing ResidentID");
            if (!std::getline(iss, ageStr, ',')) throw std::runtime_error("Missing Age");
            if (!std::getline(iss, modeOfTransport, ',')) throw std::runtime_error("Missing ModeOfTransport");
            if (!std::getline(iss, distanceStr, ',')) throw std::runtime_error("Missing DailyDistance");
            if (!std::getline(iss, factorStr, ',')) throw std::runtime_error("Missing CarbonEmissionFactor");
            if (!std::getline(iss, daysStr, ',')) throw std::runtime_error("Missing AvgDaysPerMonth");
            
            // string -> numbers
            int age = std::stoi(ageStr);
            double dailyDistance = std::stod(distanceStr);
            double carbonEmissionFactor = std::stod(factorStr);
            int avgDaysPerMonth = std::stoi(daysStr);
            
            // calc monthlyEmission
            double monthlyEmission = dailyDistance * carbonEmissionFactor * avgDaysPerMonth;
            
            // build resident object
            Resident resident;
            resident.residentID = residentID;
            resident.age = age;
            resident.modeOfTransport = modeOfTransport;
            resident.dailyDistance = dailyDistance;
            resident.carbonEmissionFactor = carbonEmissionFactor;
            resident.avgDaysPerMonth = avgDaysPerMonth;
            resident.cityLabel = cityLabel;
            resident.monthlyEmission = monthlyEmission;
            
            // append to list (tail keeps it O(1))
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

// --- printResident ---
// one row of the table, all the key fields
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

// --- printHorizontalLine ---
// simple ASCII line, 80 cols should work in most terminals
void printHorizontalLine() {
    std::cout << std::string(80, '=') << "\n";
}

// --- printCentered ---
// crude centering for 80-col terminals, should work fine
// padding = (80 - text.length()) / 2
void printCentered(const std::string& text) {
    const int consoleWidth = 80;
    int padding = (consoleWidth - text.length()) / 2;
    
    // print padding then the text
    if (padding > 0) {
        std::cout << std::string(padding, ' ');
    }
    std::cout << text << "\n";
}
