// ============================================================
// CT077-3-2-DSTR Assignment — Carbon Emission Analysis
// Asia Pacific University (APU) | Group Members:
//   - Abdul Aziz Vayani  (TP080822) -- Data Structures & File Loading
//   - Rabiah             (TP077725) -- Carbon Emission Analysis (Array)
//   - Mohamed Haim Hathim(TP077981) -- Sorting & Searching (Array)
//   - Muhammad Sohaib Saim(TP085467)-- Linked List Analysis & main()
// ============================================================

// ============================================================
// SECTION 1 - Written by: Abdul Aziz Vayani (TP080822)
// This section defines all the data structures and helper
// functions needed to load and store the resident data.
// ============================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>   // needed for measuring execution time later
#include <cstring>

// This struct stores all the information for one resident.
// I included cityLabel so we know which dataset the resident
// came from, and monthlyEmission is calculated when loading
// so we don't have to recalculate it every time we need it.
struct Resident {
    std::string residentID;
    int         age;
    std::string modeOfTransport;
    double      dailyDistance;          // km
    double      carbonEmissionFactor;   // kg CO2 / km
    int         avgDaysPerMonth;
    std::string cityLabel;
    double      monthlyEmission;        // dailyDistance * carbonEmissionFactor * avgDaysPerMonth
};

// I set MAX_SIZE to 600 because we have 3 datasets with roughly
// 200 records each. 600 gives enough space for all of them.
const int MAX_SIZE = 600;

// This struct wraps a fixed array of Resident objects.
// I use count to track how many residents are actually loaded
// so we don't accidentally read empty slots.
struct ResidentArray {
    Resident data[MAX_SIZE];
    int      count;

    ResidentArray() : count(0) {} // start count at 0
};

// Node struct for the linked list. Each node holds one resident
// and a pointer to the next node. nullptr means end of list.
struct Node {
    Resident resident;
    Node*    next;

    Node(const Resident& r) : resident(r), next(nullptr) {}
};

// This is the linked list struct. It has a head pointer to
// the first node and a size counter. I chose linked list as
// the second data structure because unlike arrays it does not
// need a fixed size - nodes are created dynamically as needed.
struct LinkedList {
    Node* head;
    int   size;

    LinkedList() : head(nullptr), size(0) {}

    // This function adds a new node at the end of the list.
    // I traverse to the last node first then attach the new one.
    void append(const Resident& r) {
        Node* newNode = new Node(r);
        if (!head) {
            // list is empty so new node becomes the head
            head = newNode;
        } else {
            // traverse to the last node
            Node* cur = head;
            while (cur->next) cur = cur->next;
            cur->next = newNode;
        }
        ++size;
    }

    // Destructor to free all allocated memory when program ends.
    // Important to avoid memory leaks since we used new to create nodes.
    ~LinkedList() {
        Node* cur = head;
        while (cur) {
            Node* tmp = cur->next;
            delete cur;
            cur = tmp;
        }
    }
};

// This struct stores the results of a performance measurement.
// I use it to record execution time and memory for sorting
// and searching so we can compare array vs linked list later.
struct PerfMetrics {
    long long executionTimeUs;  // time in microseconds
    long long memoryBytes;      // estimated memory used
    int itemsProcessed;         // how many records were processed
};

// This helper removes spaces and newline characters from the
// start and end of a string. I needed this because CSV values
// sometimes have extra whitespace that would cause parsing errors.
static std::string trim(const std::string& s) {
    const char* ws = " \t\r\n";
    std::size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return ""; // string is all whitespace
    std::size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

// These two functions print formatted table borders and centered
// headings. I put them here so all sections can reuse them
// without repeating the same cout statements everywhere.
static void printHorizontalLine(int width = 120) {
    for (int i = 0; i < width; ++i) std::cout << "=";
    std::cout << "\n";
}

static void printCentered(const std::string& text, int width = 120) {
    int padding = (width - text.length()) / 2;
    for (int i = 0; i < padding; ++i) std::cout << " ";
    std::cout << text << "\n";
}

// This function reads one line from the CSV file and fills
// a Resident struct with the values. The columns must be in
// this order: ResidentID, Age, ModeOfTransport, DailyDistance,
// CarbonEmissionFactor, AvgDaysPerMonth.
// Returns false if the line is invalid so we can skip it.
static bool parseLine(const std::string& line,
                      const std::string& city,
                      Resident&          out)
{
    std::stringstream ss(line);
    std::string token;

    // read ResidentID
    if (!std::getline(ss, token, ',')) return false;
    out.residentID = trim(token);
    if (out.residentID.empty()) return false; // skip blank lines

    // read Age and convert to integer
    if (!std::getline(ss, token, ',')) return false;
    try { out.age = std::stoi(trim(token)); }
    catch (...) { return false; } // skip if not a valid number

    // read ModeOfTransport (can have spaces but no commas)
    if (!std::getline(ss, token, ',')) return false;
    out.modeOfTransport = trim(token);

    // read DailyDistance as decimal number
    if (!std::getline(ss, token, ',')) return false;
    try { out.dailyDistance = std::stod(trim(token)); }
    catch (...) { return false; }

    // read CarbonEmissionFactor
    if (!std::getline(ss, token, ',')) return false;
    try { out.carbonEmissionFactor = std::stod(trim(token)); }
    catch (...) { return false; }

    // read AvgDaysPerMonth - last column so read rest of line
    if (!std::getline(ss, token)) return false;
    try { out.avgDaysPerMonth = std::stoi(trim(token)); }
    catch (...) { return false; }

    // set city label and calculate monthly emission straight away
    out.cityLabel       = city;
    out.monthlyEmission = out.dailyDistance
                        * out.carbonEmissionFactor
                        * out.avgDaysPerMonth;
    return true;
}

// This function opens a CSV file and loads all records into
// the array. It skips the header row and also skips any lines
// that fail to parse. Stops when array is full.
bool loadCSV(ResidentArray&     arr,
             const std::string& filename,
             const std::string& city)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open \"" << filename << "\"\n";
        return false;
    }

    std::string line;
    std::getline(file, line); // skip the header row

    while (std::getline(file, line)) {
        if (trim(line).empty()) continue; // skip empty lines

        // stop if array is full
        if (arr.count >= MAX_SIZE) {
            std::cerr << "Warning: array capacity (" << MAX_SIZE
                      << ") reached. Remaining records skipped.\n";
            break;
        }

        Resident r;
        if (parseLine(line, city, r))
            arr.data[arr.count++] = r; // add to array and increment count
    }
    return true;
}

// Same as above but loads into linked list instead of array.
// The linked list has no size limit so we don't need to check capacity.
bool loadCSV(LinkedList&        list,
             const std::string& filename,
             const std::string& city)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open \"" << filename << "\"\n";
        return false;
    }

    std::string line;
    std::getline(file, line); // skip header row

    while (std::getline(file, line)) {
        if (trim(line).empty()) continue;
        Resident r;
        if (parseLine(line, city, r))
            list.append(r); // append to end of linked list
    }
    return true;
}

// This function prints one resident record in a readable format.
// Used when displaying the first and last 5 records.
static void printResident(const Resident& r) {
    std::cout << r.residentID
              << " | City: "        << r.cityLabel
              << " | Age: "         << r.age
              << " | Mode: "        << r.modeOfTransport
              << " | Distance: "    << r.dailyDistance    << " km"
              << " | EF: "          << r.carbonEmissionFactor << " kg/km"
              << " | Days/mo: "     << r.avgDaysPerMonth
              << " | Monthly CO2: " << r.monthlyEmission   << " kg"
              << "\n";
}

// ============================================================
// SECTION 2 - Carbon Emission Analysis (Array)
// Written by: Rabiah (TP077725)
// ============================================================

// This function takes an age value and returns which age group
// the resident belongs to based on the assignment requirements.
// I used simple if conditions to check the age range for each group.
static std::string getAgeGroup(int age) {
    if (age >= 6 && age <= 17)   return "6-17: Children & Teenagers";
    if (age >= 18 && age <= 25)  return "18-25: University Students";  // covers university students
    if (age >= 26 && age <= 45)  return "26-45: Working Adults (Early)";
    if (age >= 46 && age <= 60)  return "46-60: Working Adults (Late)";
    if (age >= 61 && age <= 100) return "61-100: Senior Citizens";
    return "Unknown"; // just in case age is outside expected range
}

// This second function returns a short code version of the age group
// I needed this separately because the search functions use short codes
// like "26-45" to match against user input
static std::string getAgeGroupCode(int age) {
    if (age >= 6 && age <= 17)   return "6-17";
    if (age >= 18 && age <= 25)  return "18-25";
    if (age >= 26 && age <= 45)  return "26-45";
    if (age >= 46 && age <= 60)  return "46-60";
    if (age >= 61 && age <= 100) return "61+";
    return "???";
}

// This function analyses carbon emissions by age group using the array.
// I loop through every resident in the array, figure out which age group
// they belong to using the index (0=children, 1=young adults, etc.)
// and then add up their monthly emissions into that group's total.
static void analyzeEmissionsByAgeGroupArray(const ResidentArray& arr) {
    printHorizontalLine();
    printCentered("AGE GROUP CARBON EMISSION ANALYSIS (Array)");
    printHorizontalLine();

    // I created a local struct to hold the stats for each age group
    // instead of using separate arrays for each value
    struct AgeGroupStats {
        int count;
        double totalEmission;
        double avgEmission;
        std::string preferredMode;
        double preferredModeEmission;
    };

    // 5 groups based on assignment requirements
    AgeGroupStats groups[5];
    // initialise all values to 0 first to avoid garbage values
    for (int g = 0; g < 5; ++g) {
        groups[g].count = 0;
        groups[g].totalEmission = 0;
        groups[g].avgEmission = 0;
        groups[g].preferredMode = "N/A";
        groups[g].preferredModeEmission = 0;
    }

    // I also need to track transport modes per group to find
    // which mode is most preferred in each age group.
    // Since STL containers like map are not allowed, I used
    // a 2D array of ModeInfo structs to store this manually.
    struct ModeInfo { std::string name; int count; double totalEmission; };
    ModeInfo modesByGroup[5][10]; // max 10 different modes per group
    int modeCountsByGroup[5];
    for (int g = 0; g < 5; ++g) modeCountsByGroup[g] = 0;

    // Loop through all residents in the array
    for (int i = 0; i < arr.count; ++i) {
        const Resident& r = arr.data[i];
        int groupIdx = -1; // -1 means not assigned yet

        // Assign group index based on age
        if (r.age >= 6 && r.age <= 17)        groupIdx = 0;
        else if (r.age >= 18 && r.age <= 25)  groupIdx = 1;
        else if (r.age >= 26 && r.age <= 45)  groupIdx = 2;
        else if (r.age >= 46 && r.age <= 60)  groupIdx = 3;
        else if (r.age >= 61 && r.age <= 100) groupIdx = 4;

        if (groupIdx != -1) {
            // Add this resident to the correct group
            groups[groupIdx].count++;
            groups[groupIdx].totalEmission += r.monthlyEmission;

            // Check if this transport mode already exists in the group
            // If yes update count, if no add it as a new entry
            int modeIdx = -1;
            for (int m = 0; m < modeCountsByGroup[groupIdx]; ++m) {
                if (modesByGroup[groupIdx][m].name == r.modeOfTransport) {
                    modeIdx = m;
                    break;
                }
            }
            // Mode not found so add it as new
            if (modeIdx == -1) {
                modeIdx = modeCountsByGroup[groupIdx]++;
                modesByGroup[groupIdx][modeIdx].name = r.modeOfTransport;
                modesByGroup[groupIdx][modeIdx].count = 0;
                modesByGroup[groupIdx][modeIdx].totalEmission = 0;
            }
            modesByGroup[groupIdx][modeIdx].count++;
            modesByGroup[groupIdx][modeIdx].totalEmission += r.monthlyEmission;
        }
    }

    // After collecting all data, calculate averages and find preferred mode
    // Preferred mode = the mode with highest average emission in that group
    for (int g = 0; g < 5; ++g) {
        if (groups[g].count > 0) {
            // Average = total emission divided by number of residents
            groups[g].avgEmission = groups[g].totalEmission / groups[g].count;

            double maxAvg = -1;
            for (int m = 0; m < modeCountsByGroup[g]; ++m) {
                double avgEmission = modesByGroup[g][m].totalEmission / modesByGroup[g][m].count;
                // Keep track of which mode has the highest average
                if (avgEmission > maxAvg) {
                    maxAvg = avgEmission;
                    groups[g].preferredMode = modesByGroup[g][m].name;
                    groups[g].preferredModeEmission = modesByGroup[g][m].count;
                }
            }
        }
    }

    // Print the results in a formatted table
    std::cout << "+------------------------+----------+-------------- +-------------- +------------------+----------+\n";
    std::cout << "| Age Group              | Count    | Total CO2    | Avg CO2      | Preferred Mode   | Mode Cnt |\n";
    std::cout << "+------------------------+----------+-------------- +-------------- +------------------+----------+\n";

    const char* ageLabels[5] = {
        "6-17: Children/Teen",
        "18-25: Young Adults",
        "26-45: Early Career",
        "46-60: Late Career",
        "61+: Retirees"
    };

    for (int g = 0; g < 5; ++g) {
        if (groups[g].count > 0) {
            printf("| %-22s | %8d | %12.2f | %12.2f | %-16s | %8d |\n",
                   ageLabels[g], groups[g].count, groups[g].totalEmission,
                   groups[g].avgEmission, groups[g].preferredMode.c_str(),
                   (int)groups[g].preferredModeEmission);
        } else {
            // Still print the row even if no residents in this group
            printf("| %-22s | %8d | %12.2f | %12.2f | %-16s | %8d |\n",
                   ageLabels[g], 0, 0.0, 0.0, "N/A", 0);
        }
    }
    std::cout << "+------------------------+----------+-------------- +-------------- +------------------+----------+\n";
}

// This function shows carbon emissions broken down by transport mode.
// I loop through all residents and group them by their mode of transport,
// tracking total emissions and total distance for each mode.
static void analyzeEmissionsByModeArray(const ResidentArray& arr) {
    printHorizontalLine();
    printCentered("CARBON EMISSIONS BY TRANSPORT MODE (Array)");
    printHorizontalLine();

    // Struct to hold stats per transport mode
    struct ModeStats {
        int count;
        double totalEmission;
        double avgEmission;
        double totalDistance;
        double avgDistance;
    };

    std::string modes[10]; // to store unique mode names
    ModeStats stats[10];
    int modeCount = 0;

    for (int i = 0; i < arr.count; ++i) {
        const Resident& r = arr.data[i];
        int modeIdx = -1;

        // Check if this mode already exists in our list
        for (int j = 0; j < modeCount; ++j) {
            if (modes[j] == r.modeOfTransport) {
                modeIdx = j;
                break;
            }
        }
        // If not found, add it as a new mode
        if (modeIdx == -1) {
            modeIdx = modeCount++;
            modes[modeIdx] = r.modeOfTransport;
            stats[modeIdx].count = 0;
            stats[modeIdx].totalEmission = 0;
            stats[modeIdx].totalDistance = 0;
        }

        stats[modeIdx].count++;
        stats[modeIdx].totalEmission += r.monthlyEmission;
        stats[modeIdx].totalDistance += r.dailyDistance;
    }

    // Print table header
    std::cout << "+-------+-----+------+------+------+------+\n";
    std::cout << "| Mode  | Cnt | CO2  | Avg  | Dist | Avg  |\n";
    std::cout << "+-------+-----+------+------+------+------+\n";

    for (int i = 0; i < modeCount; ++i) {
        // Calculate averages before printing
        stats[i].avgEmission = stats[i].totalEmission / stats[i].count;
        stats[i].avgDistance = stats[i].totalDistance / stats[i].count;

        printf("| %-5s | %3d | %6.2f | %6.2f | %6.2f | %6.2f |\n",
               modes[i].c_str(), stats[i].count, stats[i].totalEmission,
               stats[i].avgEmission, stats[i].totalDistance, stats[i].avgDistance);
    }
    std::cout << "+-------+-----+------+------+------+------+\n";
}

// ============================================================
// SECTION 3 - Written by: Mohamed Haim Hathim (TP077981)
// This section handles all sorting and searching operations
// for both the array and linked list data structures.
// I used bubble sort because it is easy to understand and
// implement. I also used chrono to measure how long each
// operation takes so we can compare array vs linked list.
// ============================================================

// This function sorts the array using bubble sort algorithm.
// The field parameter decides what to sort by:
//   field 1 = sort by age
//   field 2 = sort by monthly emission
//   field 3 = sort by daily distance
// I start the chrono timer before sorting and stop it after
// so I can record the execution time accurately.
static PerfMetrics sortArrayByFieldWithTiming(ResidentArray& arr, int field, const char* fieldName) {
    auto start = std::chrono::high_resolution_clock::now();

    // Bubble sort - outer loop runs n-1 times
    // each pass moves the largest unsorted element to the end
    for (int i = 0; i < arr.count - 1; ++i) {
        // inner loop compares adjacent elements
        for (int j = 0; j < arr.count - i - 1; ++j) {
            bool shouldSwap = false;

            // check which field we are sorting by
            if (field == 1)      // age ascending
                shouldSwap = arr.data[j].age > arr.data[j+1].age;
            else if (field == 2) // emission ascending
                shouldSwap = arr.data[j].monthlyEmission > arr.data[j+1].monthlyEmission;
            else if (field == 3) // distance ascending
                shouldSwap = arr.data[j].dailyDistance > arr.data[j+1].dailyDistance;

            // swap the two elements if they are in wrong order
            if (shouldSwap) {
                Resident temp = arr.data[j];
                arr.data[j] = arr.data[j+1];
                arr.data[j+1] = temp;
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    // calculate how many microseconds the sort took
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // record performance results
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = arr.count * sizeof(Resident); // memory = number of records x size of each
    metrics.itemsProcessed = arr.count;

    return metrics;
}

// This function does the same bubble sort but for the linked list.
// Sorting a linked list is a bit different from an array because
// we cannot access elements by index. Instead I traverse using
// pointers and swap the resident data inside the nodes.
// I swap the data instead of the nodes themselves because
// swapping pointers would be more complicated.
static PerfMetrics sortLinkedListByFieldWithTiming(LinkedList& list, int field, const char* fieldName) {
    auto start = std::chrono::high_resolution_clock::now();

    // handle empty list case
    if (!list.head) {
        PerfMetrics metrics;
        metrics.executionTimeUs = 0;
        metrics.memoryBytes = 0;
        metrics.itemsProcessed = 0;
        return metrics;
    }

    // bubble sort for linked list - keep looping until no swaps happen
    bool swapped = true;
    while (swapped) {
        swapped = false;
        Node* cur = list.head;
        // traverse from head to second last node
        while (cur && cur->next) {
            bool shouldSwap = false;

            if (field == 1)      // sort by age
                shouldSwap = cur->resident.age > cur->next->resident.age;
            else if (field == 2) // sort by emission
                shouldSwap = cur->resident.monthlyEmission > cur->next->resident.monthlyEmission;
            else if (field == 3) // sort by distance
                shouldSwap = cur->resident.dailyDistance > cur->next->resident.dailyDistance;

            if (shouldSwap) {
                // swap the resident data inside the two nodes
                Resident temp = cur->resident;
                cur->resident = cur->next->resident;
                cur->next->resident = temp;
                swapped = true; // a swap happened so we need another pass
            }
            cur = cur->next;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // count how many nodes are in the list for memory calculation
    int count = 0;
    Node* cur = list.head;
    while (cur) {
        count++;
        cur = cur->next;
    }

    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = count * sizeof(Node); // linked list uses Node size not Resident size
    metrics.itemsProcessed = count;

    return metrics;
}

// This function searches the array for residents in a specific age group.
// I use linear search because it works on unsorted data too.
// The ageGroupCode tells us which range to search for example "26-45".
// Time complexity is O(n) because we check every element once.
static PerfMetrics searchByAgeGroupArray(const ResidentArray& arr, const std::string& ageGroupCode) {
    auto start = std::chrono::high_resolution_clock::now();

    int found = 0;
    int minAge = 0, maxAge = 100;

    // convert the age group code to min and max age values
    if (ageGroupCode == "6-17")       { minAge = 6;  maxAge = 17;  }
    else if (ageGroupCode == "18-25") { minAge = 18; maxAge = 25;  }
    else if (ageGroupCode == "26-45") { minAge = 26; maxAge = 45;  }
    else if (ageGroupCode == "46-60") { minAge = 46; maxAge = 60;  }
    else if (ageGroupCode == "61+")   { minAge = 61; maxAge = 100; }

    // linear search through all residents
    for (int i = 0; i < arr.count; ++i) {
        if (arr.data[i].age >= minAge && arr.data[i].age <= maxAge)
            found++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(int) * arr.count;
    metrics.itemsProcessed = found;

    return metrics;
}

// This function searches for residents using a specific transport mode.
// For example searching "Car" will count all residents who use a car.
// Again I use linear search and scan through the whole array.
static PerfMetrics searchByModeArray(const ResidentArray& arr, const std::string& mode) {
    auto start = std::chrono::high_resolution_clock::now();

    int found = 0;
    for (int i = 0; i < arr.count; ++i) {
        if (arr.data[i].modeOfTransport == mode)
            found++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(int) * arr.count;
    metrics.itemsProcessed = found;

    return metrics;
}

// This function searches for residents whose daily distance
// is greater than a given threshold value.
// For example threshold of 15 finds all residents travelling more than 15km.
static PerfMetrics searchByDistanceThresholdArray(const ResidentArray& arr, double threshold) {
    auto start = std::chrono::high_resolution_clock::now();

    int found = 0;
    for (int i = 0; i < arr.count; ++i) {
        if (arr.data[i].dailyDistance > threshold)
            found++;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(int) * arr.count;
    metrics.itemsProcessed = found;
    
    return metrics;
}

// ============================================================
// SECTION 4 - Written by: Muhammad Sohaib Saim (TP085467)
// This section handles all linked list analysis functions
// and the main() function that runs the whole program.
// The linked list version works differently from the array
// because we cannot use index numbers. Instead I traverse
// the list using a pointer starting from the head node.
// ============================================================

// This function analyses carbon emissions by age group
// using the linked list. The logic is the same as the array
// version but instead of a for loop with index I use a
// while loop with a pointer to move through the nodes.
static void analyzeEmissionsByAgeGroupList(const LinkedList& list) {
    printHorizontalLine();
    printCentered("AGE GROUP CARBON EMISSION ANALYSIS (Linked List)");
    printHorizontalLine();

    // same stats struct as the array version
    struct AgeGroupStats {
        int count;
        double totalEmission;
        double avgEmission;
        std::string preferredMode;
        double preferredModeEmission;
    };

    // initialise 5 groups to zero
    AgeGroupStats groups[5];
    for (int g = 0; g < 5; ++g) {
        groups[g].count = 0;
        groups[g].totalEmission = 0;
        groups[g].avgEmission = 0;
        groups[g].preferredMode = "N/A";
        groups[g].preferredModeEmission = 0;
    }

    // track transport modes per group using a 2D array
    // because we cannot use map or vector (no STL allowed)
    struct ModeInfo { std::string name; int count; double totalEmission; };
    ModeInfo modesByGroup[5][10];
    int modeCountsByGroup[5];
    for (int g = 0; g < 5; ++g) modeCountsByGroup[g] = 0;

    // traverse the linked list from head to end
    // cur starts at head and moves to cur->next each iteration
    Node* cur = list.head;
    while (cur) {
        const Resident& r = cur->resident;
        int groupIdx = -1;

        // determine which age group this resident belongs to
        if (r.age >= 6 && r.age <= 17)        groupIdx = 0;
        else if (r.age >= 18 && r.age <= 25)  groupIdx = 1;
        else if (r.age >= 26 && r.age <= 45)  groupIdx = 2;
        else if (r.age >= 46 && r.age <= 60)  groupIdx = 3;
        else if (r.age >= 61 && r.age <= 100) groupIdx = 4;

        if (groupIdx != -1) {
            groups[groupIdx].count++;
            groups[groupIdx].totalEmission += r.monthlyEmission;

            // check if this mode already exists for this group
            int modeIdx = -1;
            for (int m = 0; m < modeCountsByGroup[groupIdx]; ++m) {
                if (modesByGroup[groupIdx][m].name == r.modeOfTransport) {
                    modeIdx = m;
                    break;
                }
            }
            // add new mode entry if not found
            if (modeIdx == -1) {
                modeIdx = modeCountsByGroup[groupIdx]++;
                modesByGroup[groupIdx][modeIdx].name = r.modeOfTransport;
                modesByGroup[groupIdx][modeIdx].count = 0;
                modesByGroup[groupIdx][modeIdx].totalEmission = 0;
            }
            modesByGroup[groupIdx][modeIdx].count++;
            modesByGroup[groupIdx][modeIdx].totalEmission += r.monthlyEmission;
        }

        cur = cur->next; // move to next node
    }

    // calculate average and find preferred mode for each group
    for (int g = 0; g < 5; ++g) {
        if (groups[g].count > 0) {
            groups[g].avgEmission = groups[g].totalEmission / groups[g].count;

            double maxAvg = -1;
            for (int m = 0; m < modeCountsByGroup[g]; ++m) {
                double avgEmission = modesByGroup[g][m].totalEmission / modesByGroup[g][m].count;
                if (avgEmission > maxAvg) {
                    maxAvg = avgEmission;
                    groups[g].preferredMode = modesByGroup[g][m].name;
                    groups[g].preferredModeEmission = modesByGroup[g][m].count;
                }
            }
        }
    }

    // print results table
    std::cout << "+------------------------+----------+-------------- +-------------- +------------------+----------+\n";
    std::cout << "| Age Group              | Count    | Total CO2    | Avg CO2      | Preferred Mode   | Mode Cnt |\n";
    std::cout << "+------------------------+----------+-------------- +-------------- +------------------+----------+\n";

    const char* ageLabels[5] = {
        "6-17: Children/Teen",
        "18-25: Young Adults",
        "26-45: Early Career",
        "46-60: Late Career",
        "61+: Retirees"
    };

    for (int g = 0; g < 5; ++g) {
        if (groups[g].count > 0) {
            printf("| %-22s | %8d | %12.2f | %12.2f | %-16s | %8d |\n",
                   ageLabels[g], groups[g].count, groups[g].totalEmission,
                   groups[g].avgEmission, groups[g].preferredMode.c_str(),
                   (int)groups[g].preferredModeEmission);
        } else {
            // print empty row for groups with no residents
            printf("| %-22s | %8d | %12.2f | %12.2f | %-16s | %8d |\n",
                   ageLabels[g], 0, 0.0, 0.0, "N/A", 0);
        }
    }
    std::cout << "+------------------------+----------+-------------- +-------------- +------------------+----------+\n";
}

// This function compares total carbon emissions across the 3 cities.
// I loop through the linked list and match each resident to their
// city using the cityLabel field, then accumulate emissions per city.
static void analyzeEmissionsByCityList(const LinkedList& list) {
    printHorizontalLine();
    printCentered("CARBON EMISSIONS BY CITY - DATASET COMPARISON (Linked List)");
    printHorizontalLine();

    struct CityStats {
        int count;
        double totalEmission;
        double avgEmission;
        int modeCount;
    };

    // hardcode 3 cities since we know the datasets
    std::string cities[3] = { "CityA", "CityB", "CityC" };
    CityStats stats[3] = {};

    // traverse linked list and match each resident to their city
    Node* cur = list.head;
    while (cur) {
        const Resident& r = cur->resident;

        for (int i = 0; i < 3; ++i) {
            if (cities[i] == r.cityLabel) {
                stats[i].count++;
                stats[i].totalEmission += r.monthlyEmission;
                break; // found the city so stop inner loop
            }
        }
        cur = cur->next;
    }

    std::cout << "+-------+----------+----------+----------+\n";
    std::cout << "| City  | Count    | CO2 (kg) | Avg CO2  |\n";
    std::cout << "+-------+----------+----------+----------+\n";

    for (int i = 0; i < 3; ++i) {
        if (stats[i].count > 0) {
            stats[i].avgEmission = stats[i].totalEmission / stats[i].count;
            printf("| %-5s | %8d | %8.2f | %8.2f |\n",
                   cities[i].c_str(), stats[i].count, stats[i].totalEmission,
                   stats[i].avgEmission);
        }
    }
    std::cout << "+-------+----------+----------+----------+\n";
}

// Search linked list by age group with timing.
// Same approach as the array version but using pointer traversal.
// Linear search - O(n) time complexity.
static PerfMetrics searchByAgeGroupList(const LinkedList& list, const std::string& ageGroupCode) {
    auto start = std::chrono::high_resolution_clock::now();

    int found = 0;
    int minAge = 0, maxAge = 100;

    // convert age group code to min/max range
    if (ageGroupCode == "6-17")       { minAge = 6;  maxAge = 17;  }
    else if (ageGroupCode == "18-25") { minAge = 18; maxAge = 25;  }
    else if (ageGroupCode == "26-45") { minAge = 26; maxAge = 45;  }
    else if (ageGroupCode == "46-60") { minAge = 46; maxAge = 60;  }
    else if (ageGroupCode == "61+")   { minAge = 61; maxAge = 100; }

    // traverse list and count matching residents
    Node* cur = list.head;
    while (cur) {
        if (cur->resident.age >= minAge && cur->resident.age <= maxAge)
            found++;
        cur = cur->next;
    }

    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(int) * list.size;
    metrics.itemsProcessed = found;

    return metrics;
}

// Search linked list by transport mode with timing.
static PerfMetrics searchByModeList(const LinkedList& list, const std::string& mode) {
    auto start = std::chrono::high_resolution_clock::now();

    int found = 0;
    Node* cur = list.head;
    while (cur) {
        if (cur->resident.modeOfTransport == mode)
            found++;
        cur = cur->next;
    }

    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(int) * list.size;
    metrics.itemsProcessed = found;

    return metrics;
}

// Search linked list by distance threshold with timing.
// Finds all residents whose daily distance is greater than
// the given threshold value.
static PerfMetrics searchByDistanceThresholdList(const LinkedList& list, double threshold) {
    auto start = std::chrono::high_resolution_clock::now();

    int found = 0;
    Node* cur = list.head;
    while (cur) {
        if (cur->resident.dailyDistance > threshold)
            found++;
        cur = cur->next;
    }

    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    PerfMetrics metrics;
    metrics.executionTimeUs = duration;
    metrics.memoryBytes = sizeof(int) * list.size;
    metrics.itemsProcessed = found;

    return metrics;
}

// ============================================================
// main() - Written by: Muhammad Sohaib Saim (TP085467)
// This is the entry point of the program. I set up both data
// structures, load all 3 datasets, then call all the analysis,
// sorting and searching functions from the other sections.
// ============================================================
int main() {
    // file paths to the 3 dataset CSV files
    const std::string FILES[3] = {
        "datasets/dataset1-cityA.csv",
        "datasets/dataset2-cityB.csv",
        "datasets/dataset3-cityC.csv"
    };
    const std::string CITIES[3] = { "CityA", "CityB", "CityC" };

    // ── PART 1: Fixed-Size Array ─────────────────────────────
    // load all 3 datasets into the array one by one
    ResidentArray arr;
    for (int i = 0; i < 3; ++i)
        loadCSV(arr, FILES[i], CITIES[i]);

    printHorizontalLine();
    printCentered("DATA STRUCTURE 1: FIXED-SIZE ARRAY");
    printHorizontalLine();
    std::cout << "Total residents loaded: " << arr.count << " / " << MAX_SIZE << "\n";
    std::cout << "Memory allocated: " << (MAX_SIZE * sizeof(Resident) / 1024) << " KB\n\n";

    // show first and last 5 records to verify data loaded correctly
    std::cout << "--- First 5 records ---\n";
    for (int i = 0; i < 5 && i < arr.count; ++i)
        printResident(arr.data[i]);

    std::cout << "\n--- Last 5 records ---\n";
    int start = arr.count - 5 < 0 ? 0 : arr.count - 5;
    for (int i = start; i < arr.count; ++i)
        printResident(arr.data[i]);

    // run Rabiah's carbon emission analysis functions
    analyzeEmissionsByAgeGroupArray(arr);
    std::cout << "\n";
    analyzeEmissionsByModeArray(arr);

    // run Haim's sorting functions and display performance table
    printHorizontalLine();
    printCentered("SORTING PERFORMANCE - ARRAY");
    printHorizontalLine();

    // make copies of arr so sorting one doesn't affect the others
    ResidentArray arrSortAge = arr;
    PerfMetrics metricsAge = sortArrayByFieldWithTiming(arrSortAge, 1, "Age");

    ResidentArray arrSortEmission = arr;
    PerfMetrics metricsEmission = sortArrayByFieldWithTiming(arrSortEmission, 2, "Monthly Emission");

    ResidentArray arrSortDistance = arr;
    PerfMetrics metricsDistance = sortArrayByFieldWithTiming(arrSortDistance, 3, "Daily Distance");

    std::cout << "+---------------------+----------+----------+\n";
    std::cout << "| Sort By             | Time(us) | Memory(B)|\n";
    std::cout << "+---------------------+----------+----------+\n";
    printf("| Age                 | %8lld | %8lld |\n", metricsAge.executionTimeUs, metricsAge.memoryBytes);
    printf("| Monthly Emission    | %8lld | %8lld |\n", metricsEmission.executionTimeUs, metricsEmission.memoryBytes);
    printf("| Daily Distance      | %8lld | %8lld |\n", metricsDistance.executionTimeUs, metricsDistance.memoryBytes);
    std::cout << "+---------------------+----------+----------+\n";

    // run Haim's searching functions and display performance table
    printHorizontalLine();
    printCentered("SEARCHING PERFORMANCE - ARRAY");
    printHorizontalLine();

    PerfMetrics search1 = searchByAgeGroupArray(arr, "26-45");
    PerfMetrics search2 = searchByModeArray(arr, "Car");
    PerfMetrics search3 = searchByDistanceThresholdArray(arr, 15.0);

    std::cout << "+----------------------------------+-------+-------+--------+\n";
    std::cout << "| Search Criteria                  | Found | Time  | Memory |\n";
    std::cout << "+----------------------------------+-------+-------+--------+\n";
    printf("| Age Group (26-45)                | %5d | %5lld | %6lld |\n", search1.itemsProcessed, search1.executionTimeUs, search1.memoryBytes);
    printf("| Transport Mode (Car)             | %5d | %5lld | %6lld |\n", search2.itemsProcessed, search2.executionTimeUs, search2.memoryBytes);
    printf("| Distance > 15 km                 | %5d | %5lld | %6lld |\n", search3.itemsProcessed, search3.executionTimeUs, search3.memoryBytes);
    std::cout << "+----------------------------------+-------+-------+--------+\n";

    // ── PART 2: Singly Linked List ───────────────────────────
    // load same datasets into linked list
    LinkedList list;
    for (int i = 0; i < 3; ++i)
        loadCSV(list, FILES[i], CITIES[i]);

    printHorizontalLine();
    printCentered("DATA STRUCTURE 2: SINGLY LINKED LIST");
    printHorizontalLine();
    std::cout << "Total residents loaded: " << list.size << "\n";
    std::cout << "Memory allocated: " << (list.size * sizeof(Node) / 1024) << " KB (approx)\n\n";

    // show first 5 records using pointer traversal
    std::cout << "--- First 5 records ---\n";
    Node* cur = list.head;
    for (int i = 0; i < 5 && cur; ++i, cur = cur->next)
        printResident(cur->resident);

    // show last 5 records - skip to (size-5)th node first
    std::cout << "\n--- Last 5 records ---\n";
    cur = list.head;
    int skip = list.size - 5;
    // Skip to the (size-5)th node; if size < 5, skip is negative and loop won't execute
    for (int i = 0; i < skip && cur; ++i)
        cur = cur->next;
    while (cur) {
        printResident(cur->resident);
        cur = cur->next;
    }

    // run linked list analysis functions
    analyzeEmissionsByAgeGroupList(list);
    std::cout << "\n";
    analyzeEmissionsByCityList(list);

    // sort linked list and measure performance
    // I create separate lists for each sort so they don't affect each other
    printHorizontalLine();
    printCentered("SORTING PERFORMANCE - LINKED LIST");
    printHorizontalLine();

    LinkedList listSortAge;
    for (int i = 0; i < 3; ++i)
        loadCSV(listSortAge, FILES[i], CITIES[i]);
    PerfMetrics listMetricsAge = sortLinkedListByFieldWithTiming(listSortAge, 1, "Age");

    LinkedList listSortEmission;
    for (int i = 0; i < 3; ++i)
        loadCSV(listSortEmission, FILES[i], CITIES[i]);
    PerfMetrics listMetricsEmission = sortLinkedListByFieldWithTiming(listSortEmission, 2, "Monthly Emission");

    LinkedList listSortDistance;
    for (int i = 0; i < 3; ++i)
        loadCSV(listSortDistance, FILES[i], CITIES[i]);
    PerfMetrics listMetricsDistance = sortLinkedListByFieldWithTiming(listSortDistance, 3, "Daily Distance");

    std::cout << "+---------------------+----------+----------+\n";
    std::cout << "| Sort By             | Time(us) | Memory(B)|\n";
    std::cout << "+---------------------+----------+----------+\n";
    printf("| Age                 | %8lld | %8lld |\n", listMetricsAge.executionTimeUs, listMetricsAge.memoryBytes);
    printf("| Monthly Emission    | %8lld | %8lld |\n", listMetricsEmission.executionTimeUs, listMetricsEmission.memoryBytes);
    printf("| Daily Distance      | %8lld | %8lld |\n", listMetricsDistance.executionTimeUs, listMetricsDistance.memoryBytes);
    std::cout << "+---------------------+----------+----------+\n";

    // search linked list and measure performance
    printHorizontalLine();
    printCentered("SEARCHING PERFORMANCE - LINKED LIST");
    printHorizontalLine();

    PerfMetrics listSearch1 = searchByAgeGroupList(list, "18-25");
    PerfMetrics listSearch2 = searchByModeList(list, "Bicycle");
    PerfMetrics listSearch3 = searchByDistanceThresholdList(list, 10.0);

    std::cout << "+----------------------------------+-------+-------+--------+\n";
    std::cout << "| Search Criteria                  | Found | Time  | Memory |\n";
    std::cout << "+----------------------------------+-------+-------+--------+\n";
    printf("| Age Group (18-25)                | %5d | %5lld | %6lld |\n", listSearch1.itemsProcessed, listSearch1.executionTimeUs, listSearch1.memoryBytes);
    printf("| Transport Mode (Bicycle)         | %5d | %5lld | %6lld |\n", listSearch2.itemsProcessed, listSearch2.executionTimeUs, listSearch2.memoryBytes);
    printf("| Distance > 10 km                 | %5d | %5lld | %6lld |\n", listSearch3.itemsProcessed, listSearch3.executionTimeUs, listSearch3.memoryBytes);
    std::cout << "+----------------------------------+-------+-------+--------+\n";

    // final comparison showing which data structure performed better
    printHorizontalLine();
    printCentered("PERFORMANCE COMPARISON: ARRAY vs LINKED LIST");
    printHorizontalLine();

    std::cout << "Searching by Age Group (26-45 for Array, 18-25 for List):\n";
    std::cout << "  Array:  Time=" << search1.executionTimeUs << " us  |  Memory=" << search1.memoryBytes << " bytes\n";
    std::cout << "  List:   Time=" << listSearch1.executionTimeUs << " us  |  Memory=" << listSearch1.memoryBytes << " bytes\n";

    // print which one was faster
    if (search1.executionTimeUs < listSearch1.executionTimeUs)
        std::cout << "  [OK] ARRAY is faster by " << (listSearch1.executionTimeUs - search1.executionTimeUs) << " us\n";
    else
        std::cout << "  [OK] LIST is faster by " << (search1.executionTimeUs - listSearch1.executionTimeUs) << " us\n";

    printHorizontalLine();

    return 0;
}
