// ============================================================
//  DSTR Assignment — Resident data analysis
//  Data structures: fixed-size array  +  singly linked list
//  Analysis: age-group breakdown, city & mode breakdown
// ============================================================

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <chrono>

// ─────────────────────────────────────────────────────────────
//  Resident struct
// ─────────────────────────────────────────────────────────────
struct Resident {
    std::string residentID;
    int         age;
    std::string modeOfTransport;
    double      dailyDistance;          // km
    double      carbonEmissionFactor;   // kg CO2 / km
    int         avgDaysPerMonth;
    std::string cityLabel;
    double      monthlyEmission;        // DailyDistance * CarbonEmissionFactor * AvgDaysPerMonth
};

// ─────────────────────────────────────────────────────────────
//  Data structure 1 — Fixed-size array (capacity 600)
// ─────────────────────────────────────────────────────────────
const int MAX_SIZE = 600;

struct ResidentArray {
    Resident data[MAX_SIZE];
    int      count;

    ResidentArray() : count(0) {}
};

// ─────────────────────────────────────────────────────────────
//  Data structure 2 — Singly linked list
// ─────────────────────────────────────────────────────────────
struct Node {
    Resident resident;
    Node*    next;

    Node(const Resident& r) : resident(r), next(nullptr) {}
};

struct LinkedList {
    Node* head;
    int   size;

    LinkedList() : head(nullptr), size(0) {}

    void append(const Resident& r) {
        Node* newNode = new Node(r);
        if (!head) {
            head = newNode;
        } else {
            Node* cur = head;
            while (cur->next) cur = cur->next;
            cur->next = newNode;
        }
        ++size;
    }

    // Deep copy — so we can sort independent copies
    void copyFrom(const LinkedList& other) {
        // clear existing
        Node* c = head;
        while (c) { Node* t = c->next; delete c; c = t; }
        head = nullptr; size = 0;
        const Node* src = other.head;
        while (src) { append(src->resident); src = src->next; }
    }

    ~LinkedList() {
        Node* cur = head;
        while (cur) {
            Node* tmp = cur->next;
            delete cur;
            cur = tmp;
        }
    }
};

// ─────────────────────────────────────────────────────────────
//  Helpers — trim, parse, load
// ─────────────────────────────────────────────────────────────
static std::string trim(const std::string& s) {
    const char* ws = " \t\r\n";
    std::size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    std::size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

static bool parseLine(const std::string& line,
                      const std::string& city,
                      Resident&          out)
{
    std::stringstream ss(line);
    std::string token;

    if (!std::getline(ss, token, ',')) return false;
    out.residentID = trim(token);
    if (out.residentID.empty()) return false;

    if (!std::getline(ss, token, ',')) return false;
    try { out.age = std::stoi(trim(token)); }
    catch (...) { return false; }

    if (!std::getline(ss, token, ',')) return false;
    out.modeOfTransport = trim(token);

    if (!std::getline(ss, token, ',')) return false;
    try { out.dailyDistance = std::stod(trim(token)); }
    catch (...) { return false; }

    if (!std::getline(ss, token, ',')) return false;
    try { out.carbonEmissionFactor = std::stod(trim(token)); }
    catch (...) { return false; }

    if (!std::getline(ss, token)) return false;
    try { out.avgDaysPerMonth = std::stoi(trim(token)); }
    catch (...) { return false; }

    out.cityLabel       = city;
    out.monthlyEmission = out.dailyDistance
                        * out.carbonEmissionFactor
                        * out.avgDaysPerMonth;
    return true;
}

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
    std::getline(file, line);  // skip header
    while (std::getline(file, line)) {
        if (trim(line).empty()) continue;
        if (arr.count >= MAX_SIZE) {
            std::cerr << "Warning: array full, remaining records skipped.\n";
            break;
        }
        Resident r;
        if (parseLine(line, city, r))
            arr.data[arr.count++] = r;
    }
    return true;
}

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
    std::getline(file, line);  // skip header
    while (std::getline(file, line)) {
        if (trim(line).empty()) continue;
        Resident r;
        if (parseLine(line, city, r))
            list.append(r);
    }
    return true;
}

// ─────────────────────────────────────────────────────────────
//  getAgeGroup — maps age to a descriptive label
// ─────────────────────────────────────────────────────────────
std::string getAgeGroup(int age) {
    if (age >= 6  && age <= 17)  return "Children & Teenagers";
    if (age >= 18 && age <= 25)  return "University Students / Young Adults";
    if (age >= 26 && age <= 45)  return "Working Adults (Early Career)";
    if (age >= 46 && age <= 60)  return "Working Adults (Late Career)";
    if (age >= 61 && age <= 100) return "Senior Citizens / Retirees";
    return "Unknown";
}

// Returns 0-4 index for the 5 groups, -1 if outside defined ranges
static int ageGroupIndex(int age) {
    if (age >=  6 && age <= 17)  return 0;
    if (age >= 18 && age <= 25)  return 1;
    if (age >= 26 && age <= 45)  return 2;
    if (age >= 46 && age <= 60)  return 3;
    if (age >= 61 && age <= 100) return 4;
    return -1;
}

// ─────────────────────────────────────────────────────────────
//  Accumulators (no STL containers)
// ─────────────────────────────────────────────────────────────
const int MAX_MODES  = 12;
const int NUM_GROUPS = 5;
const int NUM_CITIES = 3;

struct ModeStats {
    std::string mode;
    int         count;
    double      totalEmission;

    ModeStats() : count(0), totalEmission(0.0) {}
};

struct GroupData {
    ModeStats modes[MAX_MODES];
    int       modeCount;
    double    groupTotal;

    GroupData() : modeCount(0), groupTotal(0.0) {}

    void record(const std::string& mode, double emission) {
        for (int i = 0; i < modeCount; ++i) {
            if (modes[i].mode == mode) {
                modes[i].count++;
                modes[i].totalEmission += emission;
                groupTotal += emission;
                return;
            }
        }
        if (modeCount < MAX_MODES) {
            modes[modeCount].mode          = mode;
            modes[modeCount].count         = 1;
            modes[modeCount].totalEmission = emission;
            ++modeCount;
            groupTotal += emission;
        }
    }
};

struct CityData {
    std::string city;
    ModeStats   modes[MAX_MODES];
    int         modeCount;
    double      cityTotal;
    int         residentCount;

    CityData() : modeCount(0), cityTotal(0.0), residentCount(0) {}

    void record(const std::string& mode, double emission) {
        for (int i = 0; i < modeCount; ++i) {
            if (modes[i].mode == mode) {
                modes[i].count++;
                modes[i].totalEmission += emission;
                cityTotal += emission;
                ++residentCount;
                return;
            }
        }
        if (modeCount < MAX_MODES) {
            modes[modeCount].mode          = mode;
            modes[modeCount].count         = 1;
            modes[modeCount].totalEmission = emission;
            ++modeCount;
            cityTotal += emission;
            ++residentCount;
        }
    }
};

// ─────────────────────────────────────────────────────────────
//  Table printing helpers
// ─────────────────────────────────────────────────────────────
static const std::string AGE_GROUP_LABELS[NUM_GROUPS] = {
    "Children & Teenagers [6-17]",
    "University Students / Young Adults [18-25]",
    "Working Adults (Early Career) [26-45]",
    "Working Adults (Late Career) [46-60]",
    "Senior Citizens / Retirees [61-100]"
};

static void printTableSep() {
    std::cout << "  +-" << std::string(30, '-')
              << "-+-" << std::string(7, '-')
              << "-+-" << std::string(16, '-')
              << "-+-" << std::string(16, '-') << "-+\n";
}

static void printTableHeader() {
    printTableSep();
    std::cout << "  | " << std::left  << std::setw(30) << "Mode of Transport"
              << " | " << std::right << std::setw(7)  << "Count"
              << " | " << std::right << std::setw(16) << "Total CO2 (kg)"
              << " | " << std::right << std::setw(16) << "Avg CO2 (kg)"
              << " |\n";
    printTableSep();
}

static void printGroupTable(const GroupData& g) {
    printTableHeader();
    for (int i = 0; i < g.modeCount; ++i) {
        double avg = g.modes[i].count > 0
                   ? g.modes[i].totalEmission / g.modes[i].count
                   : 0.0;
        std::cout << "  | " << std::left  << std::setw(30) << g.modes[i].mode
                  << " | " << std::right << std::setw(7)  << g.modes[i].count
                  << " | " << std::right << std::setw(16) << std::fixed
                            << std::setprecision(2) << g.modes[i].totalEmission
                  << " | " << std::right << std::setw(16) << std::fixed
                            << std::setprecision(2) << avg
                  << " |\n";
    }
    printTableSep();

    // Most preferred mode (highest count)
    int bestIdx = 0;
    for (int i = 1; i < g.modeCount; ++i)
        if (g.modes[i].count > g.modes[bestIdx].count) bestIdx = i;

    if (g.modeCount > 0)
        std::cout << "  Most preferred mode  : " << g.modes[bestIdx].mode
                  << " (" << g.modes[bestIdx].count << " residents)\n";
    std::cout << "  Total group emission : " << std::fixed << std::setprecision(2)
              << g.groupTotal << " kg CO2/month\n";
}

// ─────────────────────────────────────────────────────────────
//  Core logic — shared between array and list versions
// ─────────────────────────────────────────────────────────────
static void recordResident(GroupData groups[NUM_GROUPS], const Resident& r) {
    int idx = ageGroupIndex(r.age);
    if (idx >= 0) groups[idx].record(r.modeOfTransport, r.monthlyEmission);
}

static void recordResidentCity(CityData  cities[NUM_CITIES],
                               ModeStats allModes[MAX_MODES], int& allModeCount,
                               double&   grandTotal,
                               const Resident& r)
{
    // find city slot
    for (int c = 0; c < NUM_CITIES; ++c) {
        if (cities[c].city == r.cityLabel) {
            cities[c].record(r.modeOfTransport, r.monthlyEmission);
            break;
        }
    }
    // global mode accumulator
    for (int i = 0; i < allModeCount; ++i) {
        if (allModes[i].mode == r.modeOfTransport) {
            allModes[i].count++;
            allModes[i].totalEmission += r.monthlyEmission;
            grandTotal += r.monthlyEmission;
            return;
        }
    }
    if (allModeCount < MAX_MODES) {
        allModes[allModeCount].mode          = r.modeOfTransport;
        allModes[allModeCount].count         = 1;
        allModes[allModeCount].totalEmission = r.monthlyEmission;
        ++allModeCount;
        grandTotal += r.monthlyEmission;
    }
}

static void printCityAnalysis(CityData         cities[NUM_CITIES],
                               ModeStats        allModes[MAX_MODES],
                               int              allModeCount,
                               double           grandTotal)
{
    // ── Per city ──────────────────────────────────────────────
    std::cout << "\n  +-" << std::string(10,'-') << "-+-" << std::string(10,'-')
              << "-+-" << std::string(16,'-') << "-+-" << std::string(16,'-')
              << "-+-" << std::string(18,'-') << "-+\n";
    std::cout << "  | " << std::left  << std::setw(10) << "City"
              << " | " << std::right << std::setw(10) << "Residents"
              << " | " << std::right << std::setw(16) << "Total CO2 (kg)"
              << " | " << std::right << std::setw(16) << "Avg CO2/resident"
              << " | " << std::right << std::setw(18) << "% of Grand Total"
              << " |\n";
    std::cout << "  +-" << std::string(10,'-') << "-+-" << std::string(10,'-')
              << "-+-" << std::string(16,'-') << "-+-" << std::string(16,'-')
              << "-+-" << std::string(18,'-') << "-+\n";

    for (int c = 0; c < NUM_CITIES; ++c) {
        double avg  = cities[c].residentCount > 0
                    ? cities[c].cityTotal / cities[c].residentCount : 0.0;
        double pct  = grandTotal > 0
                    ? 100.0 * cities[c].cityTotal / grandTotal : 0.0;
        std::cout << "  | " << std::left  << std::setw(10) << cities[c].city
                  << " | " << std::right << std::setw(10) << cities[c].residentCount
                  << " | " << std::right << std::setw(16) << std::fixed
                            << std::setprecision(2) << cities[c].cityTotal
                  << " | " << std::right << std::setw(16) << std::fixed
                            << std::setprecision(2) << avg
                  << " | " << std::right << std::setw(17) << std::fixed
                            << std::setprecision(1) << pct << "%"
                  << " |\n";
    }
    std::cout << "  +-" << std::string(10,'-') << "-+-" << std::string(10,'-')
              << "-+-" << std::string(16,'-') << "-+-" << std::string(16,'-')
              << "-+-" << std::string(18,'-') << "-+\n";

    // ── Per city: top mode ────────────────────────────────────
    std::cout << "\n  Top transport mode by total emission per city:\n";
    for (int c = 0; c < NUM_CITIES; ++c) {
        if (cities[c].modeCount == 0) continue;
        int best = 0;
        for (int i = 1; i < cities[c].modeCount; ++i)
            if (cities[c].modes[i].totalEmission > cities[c].modes[best].totalEmission)
                best = i;
        std::cout << "    " << std::left << std::setw(8) << cities[c].city
                  << " -> " << cities[c].modes[best].mode
                  << "  (" << std::fixed << std::setprecision(2)
                  << cities[c].modes[best].totalEmission << " kg)\n";
    }

    // ── Global: emission by transport mode ───────────────────
    std::cout << "\n  Emission by transport mode (all cities combined):\n";
    std::cout << "  +-" << std::string(30,'-') << "-+-" << std::string(10,'-')
              << "-+-" << std::string(16,'-') << "-+-" << std::string(18,'-') << "-+\n";
    std::cout << "  | " << std::left  << std::setw(30) << "Mode of Transport"
              << " | " << std::right << std::setw(10) << "Count"
              << " | " << std::right << std::setw(16) << "Total CO2 (kg)"
              << " | " << std::right << std::setw(18) << "% of Grand Total"
              << " |\n";
    std::cout << "  +-" << std::string(30,'-') << "-+-" << std::string(10,'-')
              << "-+-" << std::string(16,'-') << "-+-" << std::string(18,'-') << "-+\n";
    for (int i = 0; i < allModeCount; ++i) {
        double pct = grandTotal > 0
                   ? 100.0 * allModes[i].totalEmission / grandTotal : 0.0;
        std::cout << "  | " << std::left  << std::setw(30) << allModes[i].mode
                  << " | " << std::right << std::setw(10) << allModes[i].count
                  << " | " << std::right << std::setw(16) << std::fixed
                            << std::setprecision(2) << allModes[i].totalEmission
                  << " | " << std::right << std::setw(17) << std::fixed
                            << std::setprecision(1) << pct << "%"
                  << " |\n";
    }
    std::cout << "  +-" << std::string(30,'-') << "-+-" << std::string(10,'-')
              << "-+-" << std::string(16,'-') << "-+-" << std::string(18,'-') << "-+\n";
    std::cout << "  Grand total CO2 (all cities, all modes): " << std::fixed
              << std::setprecision(2) << grandTotal << " kg/month\n";
}

// ─────────────────────────────────────────────────────────────
//  analyseByAgeGroup — array version
// ─────────────────────────────────────────────────────────────
void analyseByAgeGroup(const ResidentArray& arr) {
    GroupData groups[NUM_GROUPS];

    for (int i = 0; i < arr.count; ++i)
        recordResident(groups, arr.data[i]);

    std::cout << "\n============================================================\n";
    std::cout << "  analyseByAgeGroup  [Data Structure: Fixed-Size Array]\n";
    std::cout << "============================================================\n";

    for (int g = 0; g < NUM_GROUPS; ++g) {
        std::cout << "\n  Age Group: " << AGE_GROUP_LABELS[g] << "\n";
        if (groups[g].modeCount == 0) {
            std::cout << "  (no residents in this group)\n";
            continue;
        }
        printGroupTable(groups[g]);
    }
}

// ─────────────────────────────────────────────────────────────
//  analyseByAgeGroup — linked list version
// ─────────────────────────────────────────────────────────────
void analyseByAgeGroup(const LinkedList& list) {
    GroupData groups[NUM_GROUPS];

    const Node* cur = list.head;
    while (cur) {
        recordResident(groups, cur->resident);
        cur = cur->next;
    }

    std::cout << "\n============================================================\n";
    std::cout << "  analyseByAgeGroup  [Data Structure: Singly Linked List]\n";
    std::cout << "============================================================\n";

    for (int g = 0; g < NUM_GROUPS; ++g) {
        std::cout << "\n  Age Group: " << AGE_GROUP_LABELS[g] << "\n";
        if (groups[g].modeCount == 0) {
            std::cout << "  (no residents in this group)\n";
            continue;
        }
        printGroupTable(groups[g]);
    }
}

// ─────────────────────────────────────────────────────────────
//  analyseByCity — array version
// ─────────────────────────────────────────────────────────────
void analyseByCity(const ResidentArray& arr) {
    const std::string cityNames[NUM_CITIES] = { "CityA", "CityB", "CityC" };
    CityData  cities[NUM_CITIES];
    for (int c = 0; c < NUM_CITIES; ++c) cities[c].city = cityNames[c];

    ModeStats allModes[MAX_MODES];
    int       allModeCount = 0;
    double    grandTotal   = 0.0;

    for (int i = 0; i < arr.count; ++i)
        recordResidentCity(cities, allModes, allModeCount, grandTotal, arr.data[i]);

    std::cout << "\n============================================================\n";
    std::cout << "  analyseByCity  [Data Structure: Fixed-Size Array]\n";
    std::cout << "============================================================\n";
    printCityAnalysis(cities, allModes, allModeCount, grandTotal);
}

// ─────────────────────────────────────────────────────────────
//  analyseByCity — linked list version
// ─────────────────────────────────────────────────────────────
void analyseByCity(const LinkedList& list) {
    const std::string cityNames[NUM_CITIES] = { "CityA", "CityB", "CityC" };
    CityData  cities[NUM_CITIES];
    for (int c = 0; c < NUM_CITIES; ++c) cities[c].city = cityNames[c];

    ModeStats allModes[MAX_MODES];
    int       allModeCount = 0;
    double    grandTotal   = 0.0;

    const Node* cur = list.head;
    while (cur) {
        recordResidentCity(cities, allModes, allModeCount, grandTotal, cur->resident);
        cur = cur->next;
    }

    std::cout << "\n============================================================\n";
    std::cout << "  analyseByCity  [Data Structure: Singly Linked List]\n";
    std::cout << "============================================================\n";
    printCityAnalysis(cities, allModes, allModeCount, grandTotal);
}

// ─────────────────────────────────────────────────────────────
//  Display helper
// ─────────────────────────────────────────────────────────────
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

// =============================================================
//  SORTING — Array versions
// =============================================================

// Bubble Sort — by Age ascending
void bubbleSortArray(ResidentArray& arr) {
    for (int i = 0; i < arr.count - 1; ++i)
        for (int j = 0; j < arr.count - 1 - i; ++j)
            if (arr.data[j].age > arr.data[j + 1].age) {
                Resident tmp  = arr.data[j];
                arr.data[j]   = arr.data[j + 1];
                arr.data[j + 1] = tmp;
            }
}

// Selection Sort — by DailyDistance ascending
void selectionSortArray(ResidentArray& arr) {
    for (int i = 0; i < arr.count - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < arr.count; ++j)
            if (arr.data[j].dailyDistance < arr.data[minIdx].dailyDistance)
                minIdx = j;
        if (minIdx != i) {
            Resident tmp       = arr.data[i];
            arr.data[i]        = arr.data[minIdx];
            arr.data[minIdx]   = tmp;
        }
    }
}

// Insertion Sort — by monthlyEmission descending
void insertionSortArray(ResidentArray& arr) {
    for (int i = 1; i < arr.count; ++i) {
        Resident key = arr.data[i];
        int j = i - 1;
        while (j >= 0 && arr.data[j].monthlyEmission < key.monthlyEmission) {
            arr.data[j + 1] = arr.data[j];
            --j;
        }
        arr.data[j + 1] = key;
    }
}

// =============================================================
//  SORTING — Linked-list versions (swap node DATA, not pointers)
// =============================================================

// Bubble Sort — by Age ascending
void bubbleSortList(LinkedList& list) {
    if (list.size <= 1) return;
    for (int i = 0; i < list.size - 1; ++i) {
        Node* cur = list.head;
        while (cur->next) {
            if (cur->resident.age > cur->next->resident.age) {
                Resident tmp       = cur->resident;
                cur->resident      = cur->next->resident;
                cur->next->resident = tmp;
            }
            cur = cur->next;
        }
    }
}

// Selection Sort — by DailyDistance ascending
void selectionSortList(LinkedList& list) {
    for (Node* i = list.head; i; i = i->next) {
        Node* minNode = i;
        for (Node* j = i->next; j; j = j->next)
            if (j->resident.dailyDistance < minNode->resident.dailyDistance)
                minNode = j;
        if (minNode != i) {
            Resident tmp        = i->resident;
            i->resident         = minNode->resident;
            minNode->resident   = tmp;
        }
    }
}

// Insertion Sort — by monthlyEmission descending
// (Walk sorted portion from head to current; shift via swaps)
void insertionSortList(LinkedList& list) {
    if (!list.head || !list.head->next) return;

    Node* sorted = list.head;          // last node of sorted portion
    while (sorted->next) {
        Node* toInsert = sorted->next;
        if (toInsert->resident.monthlyEmission > sorted->resident.monthlyEmission) {
            // Detach toInsert
            sorted->next = toInsert->next;

            // Find correct position in sorted portion
            if (toInsert->resident.monthlyEmission >= list.head->resident.monthlyEmission) {
                // Insert before head
                toInsert->next = list.head;
                list.head = toInsert;
            } else {
                Node* prev = list.head;
                while (prev->next != sorted->next &&
                       prev->next->resident.monthlyEmission > toInsert->resident.monthlyEmission)
                    prev = prev->next;
                toInsert->next = prev->next;
                prev->next = toInsert;
            }
        } else {
            sorted = sorted->next;
        }
    }
}

// =============================================================
//  Sorted-table printer (first N records)
// =============================================================
static void printSortedTableSep() {
    std::cout << "  +-" << std::string(12, '-')
              << "-+-" << std::string(5, '-')
              << "-+-" << std::string(20, '-')
              << "-+-" << std::string(16, '-')
              << "-+-" << std::string(16, '-') << "-+\n";
}

static void printSortedHeader() {
    printSortedTableSep();
    std::cout << "  | " << std::left  << std::setw(12) << "ResidentID"
              << " | " << std::right << std::setw(5)  << "Age"
              << " | " << std::left  << std::setw(20) << "Mode"
              << " | " << std::right << std::setw(16) << "Distance (km)"
              << " | " << std::right << std::setw(16) << "CO2/month (kg)"
              << " |\n";
    printSortedTableSep();
}

static void printSortedRow(const Resident& r) {
    std::cout << "  | " << std::left  << std::setw(12) << r.residentID
              << " | " << std::right << std::setw(5)  << r.age
              << " | " << std::left  << std::setw(20) << r.modeOfTransport
              << " | " << std::right << std::setw(16) << std::fixed
                        << std::setprecision(2) << r.dailyDistance
              << " | " << std::right << std::setw(16) << std::fixed
                        << std::setprecision(2) << r.monthlyEmission
              << " |\n";
}

static void printSortedArray(const ResidentArray& arr, int n) {
    printSortedHeader();
    for (int i = 0; i < n && i < arr.count; ++i)
        printSortedRow(arr.data[i]);
    printSortedTableSep();
}

static void printSortedList(const LinkedList& list, int n) {
    printSortedHeader();
    const Node* cur = list.head;
    for (int i = 0; i < n && cur; ++i, cur = cur->next)
        printSortedRow(cur->resident);
    printSortedTableSep();
}

// =============================================================
//  Performance tracking
// =============================================================
const int MAX_PERF = 6;  // 3 algorithms x 2 data structures

struct PerfRecord {
    std::string algorithm;
    std::string dataStructure;
    long long   timeMicroseconds;
    std::size_t memoryBytes;
};

static void printPerfTable(PerfRecord perf[], int count) {
    std::cout << "\n============================================================\n";
    std::cout << "  Sorting Performance Comparison\n";
    std::cout << "============================================================\n";
    std::cout << "  +-" << std::string(22, '-')
              << "-+-" << std::string(22, '-')
              << "-+-" << std::string(14, '-')
              << "-+-" << std::string(14, '-') << "-+\n";
    std::cout << "  | " << std::left  << std::setw(22) << "Algorithm"
              << " | " << std::left  << std::setw(22) << "Data Structure"
              << " | " << std::right << std::setw(14) << "Time (us)"
              << " | " << std::right << std::setw(14) << "Memory (bytes)"
              << " |\n";
    std::cout << "  +-" << std::string(22, '-')
              << "-+-" << std::string(22, '-')
              << "-+-" << std::string(14, '-')
              << "-+-" << std::string(14, '-') << "-+\n";
    for (int i = 0; i < count; ++i)
        std::cout << "  | " << std::left  << std::setw(22) << perf[i].algorithm
                  << " | " << std::left  << std::setw(22) << perf[i].dataStructure
                  << " | " << std::right << std::setw(14) << perf[i].timeMicroseconds
                  << " | " << std::right << std::setw(14) << perf[i].memoryBytes
                  << " |\n";
    std::cout << "  +-" << std::string(22, '-')
              << "-+-" << std::string(22, '-')
              << "-+-" << std::string(14, '-')
              << "-+-" << std::string(14, '-') << "-+\n";
}

// =============================================================
//  runSortingDemo — sorts copies, prints tables, records perf
// =============================================================
void runSortingDemo(const ResidentArray& original, const LinkedList& originalList) {
    PerfRecord perf[MAX_PERF];
    int pi = 0;
    const int SHOW = 20;
    std::size_t arrMem  = sizeof(Resident) * original.count;
    std::size_t listMem = sizeof(Resident) * originalList.size;

    // ── 1. Bubble Sort by Age (ascending) ────────────────────
    {
        ResidentArray a;
        for (int i = 0; i < original.count; ++i) a.data[i] = original.data[i];
        a.count = original.count;

        auto t0 = std::chrono::high_resolution_clock::now();
        bubbleSortArray(a);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

        std::cout << "\n============================================================\n";
        std::cout << "  Bubble Sort by Age (ascending)  [Array]\n";
        std::cout << "  Time: " << us << " us   Memory: " << arrMem << " bytes\n";
        std::cout << "============================================================\n";
        printSortedArray(a, SHOW);
        perf[pi++] = {"Bubble Sort (Age)", "Array", us, arrMem};
    }
    {
        LinkedList ll;
        ll.copyFrom(originalList);

        auto t0 = std::chrono::high_resolution_clock::now();
        bubbleSortList(ll);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

        std::cout << "\n============================================================\n";
        std::cout << "  Bubble Sort by Age (ascending)  [Linked List]\n";
        std::cout << "  Time: " << us << " us   Memory: " << listMem << " bytes\n";
        std::cout << "============================================================\n";
        printSortedList(ll, SHOW);
        perf[pi++] = {"Bubble Sort (Age)", "Linked List", us, listMem};
    }

    // ── 2. Selection Sort by DailyDistance (ascending) ───────
    {
        ResidentArray a;
        for (int i = 0; i < original.count; ++i) a.data[i] = original.data[i];
        a.count = original.count;

        auto t0 = std::chrono::high_resolution_clock::now();
        selectionSortArray(a);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

        std::cout << "\n============================================================\n";
        std::cout << "  Selection Sort by DailyDistance (ascending)  [Array]\n";
        std::cout << "  Time: " << us << " us   Memory: " << arrMem << " bytes\n";
        std::cout << "============================================================\n";
        printSortedArray(a, SHOW);
        perf[pi++] = {"Selection Sort (Dist)", "Array", us, arrMem};
    }
    {
        LinkedList ll;
        ll.copyFrom(originalList);

        auto t0 = std::chrono::high_resolution_clock::now();
        selectionSortList(ll);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

        std::cout << "\n============================================================\n";
        std::cout << "  Selection Sort by DailyDistance (ascending)  [Linked List]\n";
        std::cout << "  Time: " << us << " us   Memory: " << listMem << " bytes\n";
        std::cout << "============================================================\n";
        printSortedList(ll, SHOW);
        perf[pi++] = {"Selection Sort (Dist)", "Linked List", us, listMem};
    }

    // ── 3. Insertion Sort by monthlyEmission (descending) ────
    {
        ResidentArray a;
        for (int i = 0; i < original.count; ++i) a.data[i] = original.data[i];
        a.count = original.count;

        auto t0 = std::chrono::high_resolution_clock::now();
        insertionSortArray(a);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

        std::cout << "\n============================================================\n";
        std::cout << "  Insertion Sort by monthlyEmission (descending)  [Array]\n";
        std::cout << "  Time: " << us << " us   Memory: " << arrMem << " bytes\n";
        std::cout << "============================================================\n";
        printSortedArray(a, SHOW);
        perf[pi++] = {"Insertion Sort (CO2)", "Array", us, arrMem};
    }
    {
        LinkedList ll;
        ll.copyFrom(originalList);

        auto t0 = std::chrono::high_resolution_clock::now();
        insertionSortList(ll);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

        std::cout << "\n============================================================\n";
        std::cout << "  Insertion Sort by monthlyEmission (descending)  [Linked List]\n";
        std::cout << "  Time: " << us << " us   Memory: " << listMem << " bytes\n";
        std::cout << "============================================================\n";
        printSortedList(ll, SHOW);
        perf[pi++] = {"Insertion Sort (CO2)", "Linked List", us, listMem};
    }

    // ── Performance comparison ───────────────────────────────
    printPerfTable(perf, pi);
}

// =============================================================
//  SEARCH INFRASTRUCTURE
// =============================================================

// Fixed-capacity result collection (no STL)
const int MAX_RESULTS = 600;

struct SearchResult {
    const Resident* ptr;   // points into the original data
};

struct ResultSet {
    SearchResult items[MAX_RESULTS];
    int          count;
    ResultSet() : count(0) {}

    void add(const Resident* r) {
        if (count < MAX_RESULTS) items[count++].ptr = r;
    }
};

// ── Result table printer ──────────────────────────────────────
static void printResultSep() {
    std::cout << "  +-" << std::string(12, '-')
              << "-+-" << std::string(5,  '-')
              << "-+-" << std::string(20, '-')
              << "-+-" << std::string(14, '-')
              << "-+-" << std::string(14, '-') << "-+\n";
}
static void printResultHeader() {
    printResultSep();
    std::cout << "  | " << std::left  << std::setw(12) << "ResidentID"
              << " | " << std::right << std::setw(5)  << "Age"
              << " | " << std::left  << std::setw(20) << "Mode"
              << " | " << std::right << std::setw(14) << "Distance (km)"
              << " | " << std::right << std::setw(14) << "CO2/mo (kg)"
              << " |\n";
    printResultSep();
}
static void printResultSet(const ResultSet& rs) {
    printResultHeader();
    for (int i = 0; i < rs.count; ++i) {
        const Resident& r = *rs.items[i].ptr;
        std::cout << "  | " << std::left  << std::setw(12) << r.residentID
                  << " | " << std::right << std::setw(5)  << r.age
                  << " | " << std::left  << std::setw(20) << r.modeOfTransport
                  << " | " << std::right << std::setw(14) << std::fixed
                            << std::setprecision(2) << r.dailyDistance
                  << " | " << std::right << std::setw(14) << std::fixed
                            << std::setprecision(2) << r.monthlyEmission
                  << " |\n";
    }
    printResultSep();
}

// ── Search performance tracker ────────────────────────────────
const int MAX_SEARCH_PERF = 10;

struct SearchPerfRecord {
    std::string searchType;
    std::string dataStructure;
    std::string criteria;
    int         resultsFound;
    long long   timeMicroseconds;
};

static void printSearchPerfTable(SearchPerfRecord perf[], int count) {
    std::cout << "\n============================================================\n";
    std::cout << "  Search Performance Comparison\n";
    std::cout << "============================================================\n";
    std::cout << "  +-" << std::string(16, '-')
              << "-+-" << std::string(14, '-')
              << "-+-" << std::string(32, '-')
              << "-+-" << std::string(8,  '-')
              << "-+-" << std::string(10, '-') << "-+\n";
    std::cout << "  | " << std::left  << std::setw(16) << "Search Type"
              << " | " << std::left  << std::setw(14) << "Data Structure"
              << " | " << std::left  << std::setw(32) << "Criteria"
              << " | " << std::right << std::setw(8)  << "Found"
              << " | " << std::right << std::setw(10) << "Time (us)"
              << " |\n";
    std::cout << "  +-" << std::string(16, '-')
              << "-+-" << std::string(14, '-')
              << "-+-" << std::string(32, '-')
              << "-+-" << std::string(8,  '-')
              << "-+-" << std::string(10, '-') << "-+\n";
    for (int i = 0; i < count; ++i)
        std::cout << "  | " << std::left  << std::setw(16) << perf[i].searchType
                  << " | " << std::left  << std::setw(14) << perf[i].dataStructure
                  << " | " << std::left  << std::setw(32) << perf[i].criteria
                  << " | " << std::right << std::setw(8)  << perf[i].resultsFound
                  << " | " << std::right << std::setw(10) << perf[i].timeMicroseconds
                  << " |\n";
    std::cout << "  +-" << std::string(16, '-')
              << "-+-" << std::string(14, '-')
              << "-+-" << std::string(32, '-')
              << "-+-" << std::string(8,  '-')
              << "-+-" << std::string(10, '-') << "-+\n";
}

// =============================================================
//  LINEAR SEARCH — Array
// =============================================================

// 1a. By age range [lo, hi]
ResultSet linearSearchAgeRange(const ResidentArray& arr, int lo, int hi) {
    ResultSet rs;
    for (int i = 0; i < arr.count; ++i)
        if (arr.data[i].age >= lo && arr.data[i].age <= hi)
            rs.add(&arr.data[i]);
    return rs;
}

// 1b. By mode of transport (exact, case-sensitive)
ResultSet linearSearchMode(const ResidentArray& arr, const std::string& mode) {
    ResultSet rs;
    for (int i = 0; i < arr.count; ++i)
        if (arr.data[i].modeOfTransport == mode)
            rs.add(&arr.data[i]);
    return rs;
}

// 1c. By daily distance above threshold
ResultSet linearSearchDistance(const ResidentArray& arr, double threshold) {
    ResultSet rs;
    for (int i = 0; i < arr.count; ++i)
        if (arr.data[i].dailyDistance > threshold)
            rs.add(&arr.data[i]);
    return rs;
}

// =============================================================
//  LINEAR SEARCH — Linked List
// =============================================================

ResultSet linearSearchAgeRange(const LinkedList& list, int lo, int hi) {
    ResultSet rs;
    const Node* cur = list.head;
    while (cur) {
        if (cur->resident.age >= lo && cur->resident.age <= hi)
            rs.add(&cur->resident);
        cur = cur->next;
    }
    return rs;
}

ResultSet linearSearchMode(const LinkedList& list, const std::string& mode) {
    ResultSet rs;
    const Node* cur = list.head;
    while (cur) {
        if (cur->resident.modeOfTransport == mode)
            rs.add(&cur->resident);
        cur = cur->next;
    }
    return rs;
}

ResultSet linearSearchDistance(const LinkedList& list, double threshold) {
    ResultSet rs;
    const Node* cur = list.head;
    while (cur) {
        if (cur->resident.dailyDistance > threshold)
            rs.add(&cur->resident);
        cur = cur->next;
    }
    return rs;
}

// =============================================================
//  BINARY SEARCH — requires data sorted ascending by Age
//  Finds all records whose age falls within [lo, hi].
//  Works on array by index; on linked list by building a
//  temporary pointer index (no STL).
// =============================================================

ResultSet binarySearchAgeRange(const ResidentArray& arr, int lo, int hi) {
    ResultSet rs;
    // Find leftmost index where age >= lo
    int left = 0, right = arr.count - 1, start = arr.count;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr.data[mid].age >= lo) { start = mid; right = mid - 1; }
        else                          left  = mid + 1;
    }
    // Scan forward while age <= hi
    for (int i = start; i < arr.count && arr.data[i].age <= hi; ++i)
        rs.add(&arr.data[i]);
    return rs;
}

// Linked-list binary search: build a flat pointer array, then apply
// the same binary search logic without touching the list's pointers.
ResultSet binarySearchAgeRange(const LinkedList& list, int lo, int hi) {
    // Build flat index
    const Resident* idx[MAX_SIZE];
    int n = 0;
    const Node* cur = list.head;
    while (cur && n < MAX_SIZE) { idx[n++] = &cur->resident; cur = cur->next; }

    ResultSet rs;
    // Find leftmost position where age >= lo
    int left = 0, right = n - 1, start = n;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (idx[mid]->age >= lo) { start = mid; right = mid - 1; }
        else                      left  = mid + 1;
    }
    for (int i = start; i < n && idx[i]->age <= hi; ++i)
        rs.add(idx[i]);
    return rs;
}

// =============================================================
//  runSearchDemo
// =============================================================
void runSearchDemo(const ResidentArray& arr, const LinkedList& list) {
    SearchPerfRecord perf[MAX_SEARCH_PERF];
    int pi = 0;

    // We need age-sorted copies for binary search
    ResidentArray sortedArr;
    for (int i = 0; i < arr.count; ++i) sortedArr.data[i] = arr.data[i];
    sortedArr.count = arr.count;
    bubbleSortArray(sortedArr);   // reuse existing sort

    LinkedList sortedList;
    sortedList.copyFrom(list);
    bubbleSortList(sortedList);

    std::cout << "\n============================================================\n";
    std::cout << "  SEARCH DEMONSTRATIONS\n";
    std::cout << "============================================================\n";

    // ── Linear: age range 26-45 ──────────────────────────────
    {
        auto t0 = std::chrono::high_resolution_clock::now();
        ResultSet rs = linearSearchAgeRange(arr, 26, 45);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();

        std::cout << "\n  Linear Search | Age range 26-45 | Array\n";
        std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
        printResultSet(rs);
        perf[pi++] = {"Linear", "Array", "Age range 26-45", rs.count, us};
    }
    {
        auto t0 = std::chrono::high_resolution_clock::now();
        ResultSet rs = linearSearchAgeRange(list, 26, 45);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();

        std::cout << "\n  Linear Search | Age range 26-45 | Linked List\n";
        std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
        printResultSet(rs);
        perf[pi++] = {"Linear", "Linked List", "Age range 26-45", rs.count, us};
    }

    // ── Linear: mode = "Car" ─────────────────────────────────
    {
        auto t0 = std::chrono::high_resolution_clock::now();
        ResultSet rs = linearSearchMode(arr, "Car");
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();

        std::cout << "\n  Linear Search | Mode = \"Car\" | Array\n";
        std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
        printResultSet(rs);
        perf[pi++] = {"Linear", "Array", "Mode = Car", rs.count, us};
    }
    {
        auto t0 = std::chrono::high_resolution_clock::now();
        ResultSet rs = linearSearchMode(list, "Car");
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();

        std::cout << "\n  Linear Search | Mode = \"Car\" | Linked List\n";
        std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
        printResultSet(rs);
        perf[pi++] = {"Linear", "Linked List", "Mode = Car", rs.count, us};
    }

    // ── Linear: distance > 15 ────────────────────────────────
    {
        auto t0 = std::chrono::high_resolution_clock::now();
        ResultSet rs = linearSearchDistance(arr, 15.0);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();

        std::cout << "\n  Linear Search | DailyDistance > 15 km | Array\n";
        std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
        printResultSet(rs);
        perf[pi++] = {"Linear", "Array", "Distance > 15 km", rs.count, us};
    }
    {
        auto t0 = std::chrono::high_resolution_clock::now();
        ResultSet rs = linearSearchDistance(list, 15.0);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();

        std::cout << "\n  Linear Search | DailyDistance > 15 km | Linked List\n";
        std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
        printResultSet(rs);
        perf[pi++] = {"Linear", "Linked List", "Distance > 15 km", rs.count, us};
    }

    // ── Binary: age range 26-45 (on age-sorted copy) ─────────
    {
        auto t0 = std::chrono::high_resolution_clock::now();
        ResultSet rs = binarySearchAgeRange(sortedArr, 26, 45);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();

        std::cout << "\n  Binary Search | Age range 26-45 (sorted) | Array\n";
        std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
        printResultSet(rs);
        perf[pi++] = {"Binary", "Array (sorted)", "Age range 26-45", rs.count, us};
    }
    {
        auto t0 = std::chrono::high_resolution_clock::now();
        ResultSet rs = binarySearchAgeRange(sortedList, 26, 45);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();

        std::cout << "\n  Binary Search | Age range 26-45 (sorted) | Linked List\n";
        std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
        printResultSet(rs);
        perf[pi++] = {"Binary", "List (sorted)", "Age range 26-45", rs.count, us};
    }

    printSearchPerfTable(perf, pi);
}

// =============================================================
//  SESSION PERFORMANCE ACCUMULATOR
// =============================================================
const int MAX_SESSION_PERF = 60;

struct SessionPerf {
    PerfRecord       sorts[MAX_SESSION_PERF];
    int              sortCount;
    SearchPerfRecord searches[MAX_SESSION_PERF];
    int              searchCount;

    SessionPerf() : sortCount(0), searchCount(0) {}

    void addSort(const std::string& algo, const std::string& ds,
                 long long us, std::size_t mem) {
        if (sortCount < MAX_SESSION_PERF)
            sorts[sortCount++] = {algo, ds, us, mem};
    }
    void addSearch(const std::string& type, const std::string& ds,
                   const std::string& criteria, int found, long long us) {
        if (searchCount < MAX_SESSION_PERF)
            searches[searchCount++] = {type, ds, criteria, found, us};
    }
};

// =============================================================
//  MENU HELPERS
// =============================================================
static void menuLine(char c = '=', int w = 60) {
    std::cout << "  " << std::string(w, c) << "\n";
}
static void menuTitle(const std::string& t) {
    menuLine();
    std::cout << "  " << t << "\n";
    menuLine();
}
static int readInt(const std::string& prompt) {
    int v;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) { std::cin.ignore(1000, '\n'); return v; }
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "  Invalid input. Please enter a number.\n";
    }
}
static double readDouble(const std::string& prompt) {
    double v;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) { std::cin.ignore(1000, '\n'); return v; }
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "  Invalid input. Please enter a number.\n";
    }
}
static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return trim(s);
}

// ── Print first 5 and last 5 records of current data ─────────
static void printDataPreview(const ResidentArray& arr) {
    std::cout << "\n  First 5 records:\n";
    for (int i = 0; i < 5 && i < arr.count; ++i) printResident(arr.data[i]);
    std::cout << "  Last 5 records:\n";
    int s = arr.count - 5 < 0 ? 0 : arr.count - 5;
    for (int i = s; i < arr.count; ++i) printResident(arr.data[i]);
}
static void printDataPreview(const LinkedList& list) {
    std::cout << "\n  First 5 records:\n";
    const Node* cur = list.head;
    for (int i = 0; cur && i < 5; ++i, cur = cur->next) printResident(cur->resident);
    std::cout << "  Last 5 records:\n";
    cur = list.head;
    int skip = list.size - 5;
    for (int i = 0; cur && i < skip; ++i) cur = cur->next;
    while (cur) { printResident(cur->resident); cur = cur->next; }
}

// ── Performance summary ───────────────────────────────────────
static void showPerfSummary(const SessionPerf& sp) {
    menuTitle("  PERFORMANCE SUMMARY");
    if (sp.sortCount == 0 && sp.searchCount == 0) {
        std::cout << "  No operations recorded yet.\n";
        return;
    }
    if (sp.sortCount > 0) {
        std::cout << "\n  -- Sorting Results --\n";
        printPerfTable(const_cast<PerfRecord*>(sp.sorts), sp.sortCount);
    }
    if (sp.searchCount > 0) {
        std::cout << "\n  -- Search Results --\n";
        printSearchPerfTable(const_cast<SearchPerfRecord*>(sp.searches), sp.searchCount);
    }
}

// =============================================================
//  SORT SUB-MENUS
// =============================================================
static void doSortArray(ResidentArray& arr, SessionPerf& sp) {
    std::size_t mem = sizeof(Resident) * arr.count;
    const int SHOW = 20;
    while (true) {
        menuTitle("  SORTING — Array");
        std::cout << "  1. Bubble Sort    (by Age, ascending)\n";
        std::cout << "  2. Selection Sort (by Daily Distance, ascending)\n";
        std::cout << "  3. Insertion Sort (by Monthly Emission, descending)\n";
        std::cout << "  4. Run All Three\n";
        std::cout << "  0. Back\n";
        int ch = readInt("  Choice: ");
        if (ch == 0) break;
        if (ch < 1 || ch > 4) { std::cout << "  Invalid choice.\n"; continue; }

        auto run = [&](int which) {
            ResidentArray copy;
            for (int i = 0; i < arr.count; ++i) copy.data[i] = arr.data[i];
            copy.count = arr.count;

            std::string algo;
            auto t0 = std::chrono::high_resolution_clock::now();
            if (which == 1) { bubbleSortArray(copy);    algo = "Bubble Sort (Age)"; }
            if (which == 2) { selectionSortArray(copy); algo = "Selection Sort (Dist)"; }
            if (which == 3) { insertionSortArray(copy); algo = "Insertion Sort (CO2)"; }
            auto t1 = std::chrono::high_resolution_clock::now();
            long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();

            menuLine('-');
            std::cout << "  " << algo << "  [Array]\n";
            std::cout << "  Time: " << us << " us   |   Memory: " << mem << " bytes\n";
            menuLine('-');
            printSortedArray(copy, SHOW);
            sp.addSort(algo, "Array", us, mem);
        };

        if (ch == 4) { run(1); run(2); run(3); }
        else          run(ch);
    }
}

static void doSortList(LinkedList& list, SessionPerf& sp) {
    std::size_t mem = sizeof(Resident) * list.size;
    const int SHOW = 20;
    while (true) {
        menuTitle("  SORTING — Linked List");
        std::cout << "  1. Bubble Sort    (by Age, ascending)\n";
        std::cout << "  2. Selection Sort (by Daily Distance, ascending)\n";
        std::cout << "  3. Insertion Sort (by Monthly Emission, descending)\n";
        std::cout << "  4. Run All Three\n";
        std::cout << "  0. Back\n";
        int ch = readInt("  Choice: ");
        if (ch == 0) break;
        if (ch < 1 || ch > 4) { std::cout << "  Invalid choice.\n"; continue; }

        auto run = [&](int which) {
            LinkedList copy;
            copy.copyFrom(list);

            std::string algo;
            auto t0 = std::chrono::high_resolution_clock::now();
            if (which == 1) { bubbleSortList(copy);    algo = "Bubble Sort (Age)"; }
            if (which == 2) { selectionSortList(copy); algo = "Selection Sort (Dist)"; }
            if (which == 3) { insertionSortList(copy); algo = "Insertion Sort (CO2)"; }
            auto t1 = std::chrono::high_resolution_clock::now();
            long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();

            menuLine('-');
            std::cout << "  " << algo << "  [Linked List]\n";
            std::cout << "  Time: " << us << " us   |   Memory: " << mem << " bytes\n";
            menuLine('-');
            printSortedList(copy, SHOW);
            sp.addSort(algo, "Linked List", us, mem);
        };

        if (ch == 4) { run(1); run(2); run(3); }
        else          run(ch);
    }
}

// =============================================================
//  SEARCH SUB-MENUS
// =============================================================

// Helper: build sorted array copy for binary search
static void makeSortedArrayCopy(const ResidentArray& src, ResidentArray& dst) {
    for (int i = 0; i < src.count; ++i) dst.data[i] = src.data[i];
    dst.count = src.count;
    bubbleSortArray(dst);
}

static void doSearchArray(const ResidentArray& arr, SessionPerf& sp) {
    while (true) {
        menuTitle("  SEARCHING — Array");
        std::cout << "  1. Linear Search  — by Age Group (enter range)\n";
        std::cout << "  2. Linear Search  — by Mode of Transport\n";
        std::cout << "  3. Linear Search  — by Daily Distance above threshold\n";
        std::cout << "  4. Binary Search  — by Age Range (sorted copy)\n";
        std::cout << "  5. Run All (defaults: age 26-45, mode Car, dist>15)\n";
        std::cout << "  0. Back\n";
        int ch = readInt("  Choice: ");
        if (ch == 0) break;
        if (ch < 1 || ch > 5) { std::cout << "  Invalid choice.\n"; continue; }

        if (ch == 1 || ch == 5) {
            int lo = 26, hi = 45;
            if (ch == 1) {
                lo = readInt("  Age range — lower bound: ");
                hi = readInt("  Age range — upper bound: ");
            }
            auto t0 = std::chrono::high_resolution_clock::now();
            ResultSet rs = linearSearchAgeRange(arr, lo, hi);
            auto t1 = std::chrono::high_resolution_clock::now();
            long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
            std::string crit = "Age " + std::to_string(lo) + "-" + std::to_string(hi);
            std::cout << "\n  Linear Search | " << crit << " | Array\n";
            std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
            printResultSet(rs);
            sp.addSearch("Linear", "Array", crit, rs.count, us);
        }
        if (ch == 2 || ch == 5) {
            std::string mode = "Car";
            if (ch == 2) mode = readLine("  Mode of transport: ");
            auto t0 = std::chrono::high_resolution_clock::now();
            ResultSet rs = linearSearchMode(arr, mode);
            auto t1 = std::chrono::high_resolution_clock::now();
            long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
            std::string crit = "Mode = " + mode;
            std::cout << "\n  Linear Search | " << crit << " | Array\n";
            std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
            printResultSet(rs);
            sp.addSearch("Linear", "Array", crit, rs.count, us);
        }
        if (ch == 3 || ch == 5) {
            double thr = 15.0;
            if (ch == 3) thr = readDouble("  Distance threshold (records above X km): ");
            auto t0 = std::chrono::high_resolution_clock::now();
            ResultSet rs = linearSearchDistance(arr, thr);
            auto t1 = std::chrono::high_resolution_clock::now();
            long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
            std::string crit = "Distance > " + std::to_string((int)thr) + " km";
            std::cout << "\n  Linear Search | " << crit << " | Array\n";
            std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
            printResultSet(rs);
            sp.addSearch("Linear", "Array", crit, rs.count, us);
        }
        if (ch == 4 || ch == 5) {
            int lo = 26, hi = 45;
            if (ch == 4) {
                lo = readInt("  Age range — lower bound: ");
                hi = readInt("  Age range — upper bound: ");
            }
            std::cout << "  (Sorting array copy by age for binary search...)\n";
            ResidentArray sorted;
            makeSortedArrayCopy(arr, sorted);
            auto t0 = std::chrono::high_resolution_clock::now();
            ResultSet rs = binarySearchAgeRange(sorted, lo, hi);
            auto t1 = std::chrono::high_resolution_clock::now();
            long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
            std::string crit = "Age " + std::to_string(lo) + "-" + std::to_string(hi);
            std::cout << "\n  Binary Search | " << crit << " (sorted) | Array\n";
            std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
            printResultSet(rs);
            sp.addSearch("Binary", "Array (sorted)", crit, rs.count, us);
        }
    }
}

static void doSearchList(const LinkedList& list, SessionPerf& sp) {
    while (true) {
        menuTitle("  SEARCHING — Linked List");
        std::cout << "  1. Linear Search  — by Age Group (enter range)\n";
        std::cout << "  2. Linear Search  — by Mode of Transport\n";
        std::cout << "  3. Linear Search  — by Daily Distance above threshold\n";
        std::cout << "  4. Binary Search  — by Age Range (sorted copy)\n";
        std::cout << "  5. Run All (defaults: age 26-45, mode Car, dist>15)\n";
        std::cout << "  0. Back\n";
        int ch = readInt("  Choice: ");
        if (ch == 0) break;
        if (ch < 1 || ch > 5) { std::cout << "  Invalid choice.\n"; continue; }

        if (ch == 1 || ch == 5) {
            int lo = 26, hi = 45;
            if (ch == 1) {
                lo = readInt("  Age range — lower bound: ");
                hi = readInt("  Age range — upper bound: ");
            }
            auto t0 = std::chrono::high_resolution_clock::now();
            ResultSet rs = linearSearchAgeRange(list, lo, hi);
            auto t1 = std::chrono::high_resolution_clock::now();
            long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
            std::string crit = "Age " + std::to_string(lo) + "-" + std::to_string(hi);
            std::cout << "\n  Linear Search | " << crit << " | Linked List\n";
            std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
            printResultSet(rs);
            sp.addSearch("Linear", "Linked List", crit, rs.count, us);
        }
        if (ch == 2 || ch == 5) {
            std::string mode = "Car";
            if (ch == 2) mode = readLine("  Mode of transport: ");
            auto t0 = std::chrono::high_resolution_clock::now();
            ResultSet rs = linearSearchMode(list, mode);
            auto t1 = std::chrono::high_resolution_clock::now();
            long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
            std::string crit = "Mode = " + mode;
            std::cout << "\n  Linear Search | " << crit << " | Linked List\n";
            std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
            printResultSet(rs);
            sp.addSearch("Linear", "Linked List", crit, rs.count, us);
        }
        if (ch == 3 || ch == 5) {
            double thr = 15.0;
            if (ch == 3) thr = readDouble("  Distance threshold (records above X km): ");
            auto t0 = std::chrono::high_resolution_clock::now();
            ResultSet rs = linearSearchDistance(list, thr);
            auto t1 = std::chrono::high_resolution_clock::now();
            long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
            std::string crit = "Distance > " + std::to_string((int)thr) + " km";
            std::cout << "\n  Linear Search | " << crit << " | Linked List\n";
            std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
            printResultSet(rs);
            sp.addSearch("Linear", "Linked List", crit, rs.count, us);
        }
        if (ch == 4 || ch == 5) {
            int lo = 26, hi = 45;
            if (ch == 4) {
                lo = readInt("  Age range — lower bound: ");
                hi = readInt("  Age range — upper bound: ");
            }
            std::cout << "  (Sorting list copy by age for binary search...)\n";
            LinkedList sorted;
            sorted.copyFrom(list);
            bubbleSortList(sorted);
            auto t0 = std::chrono::high_resolution_clock::now();
            ResultSet rs = binarySearchAgeRange(sorted, lo, hi);
            auto t1 = std::chrono::high_resolution_clock::now();
            long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
            std::string crit = "Age " + std::to_string(lo) + "-" + std::to_string(hi);
            std::cout << "\n  Binary Search | " << crit << " (sorted) | Linked List\n";
            std::cout << "  Found: " << rs.count << " records  |  Time: " << us << " us\n";
            printResultSet(rs);
            sp.addSearch("Binary", "List (sorted)", crit, rs.count, us);
        }
    }
}

// =============================================================
//  INSIGHTS & RECOMMENDATIONS
// =============================================================

struct InsightData {
    // per age group
    double groupEmission[NUM_GROUPS];
    int    groupCount[NUM_GROUPS];
    int    groupBicycleCount[NUM_GROUPS];
    int    groupCarCount[NUM_GROUPS];
    int    groupWalkCount[NUM_GROUPS];
    // per city (index 0=CityA, 1=CityB, 2=CityC)
    double cityEmission[NUM_CITIES];
    int    cityCount[NUM_CITIES];
    // global
    double grandTotal;
    int    totalCount;

    InsightData() : grandTotal(0.0), totalCount(0) {
        for (int i = 0; i < NUM_GROUPS; ++i) {
            groupEmission[i] = 0.0;
            groupCount[i] = groupBicycleCount[i] = groupCarCount[i] = groupWalkCount[i] = 0;
        }
        for (int i = 0; i < NUM_CITIES; ++i) {
            cityEmission[i] = 0.0;
            cityCount[i] = 0;
        }
    }

    void record(const Resident& r) {
        int gi = ageGroupIndex(r.age);
        if (gi >= 0) {
            groupEmission[gi] += r.monthlyEmission;
            ++groupCount[gi];
            if (r.modeOfTransport == "Bicycle") ++groupBicycleCount[gi];
            if (r.modeOfTransport == "Car")     ++groupCarCount[gi];
            if (r.modeOfTransport == "Walking") ++groupWalkCount[gi];
        }
        // city index
        const std::string cNames[NUM_CITIES] = {"CityA", "CityB", "CityC"};
        for (int c = 0; c < NUM_CITIES; ++c) {
            if (r.cityLabel == cNames[c]) {
                cityEmission[c] += r.monthlyEmission;
                ++cityCount[c];
                break;
            }
        }
        grandTotal += r.monthlyEmission;
        ++totalCount;
    }
};

static InsightData buildInsights(const ResidentArray& arr) {
    InsightData d;
    for (int i = 0; i < arr.count; ++i) d.record(arr.data[i]);
    return d;
}

static InsightData buildInsights(const LinkedList& list) {
    InsightData d;
    const Node* cur = list.head;
    while (cur) { d.record(cur->resident); cur = cur->next; }
    return d;
}

static void printInsights(const InsightData& d, const std::string& ds) {
    const std::string cNames[NUM_CITIES] = {"CityA", "CityB", "CityC"};

    // ── Derive findings ───────────────────────────────────────
    // 1. Highest-emission age group
    int topGroup = 0;
    for (int i = 1; i < NUM_GROUPS; ++i)
        if (d.groupEmission[i] > d.groupEmission[topGroup]) topGroup = i;

    // 2. Age group with highest bicycle preference (bike% among all residents in group)
    int topBikeGroup = 0;
    double topBikePct = 0.0;
    for (int i = 0; i < NUM_GROUPS; ++i) {
        if (d.groupCount[i] == 0) continue;
        double pct = 100.0 * d.groupBicycleCount[i] / d.groupCount[i];
        if (pct > topBikePct) { topBikePct = pct; topBikeGroup = i; }
    }

    // 3. Age group most dependent on cars (highest car%)
    int topCarGroup = 0;
    double topCarPct = 0.0;
    for (int i = 0; i < NUM_GROUPS; ++i) {
        if (d.groupCount[i] == 0) continue;
        double pct = 100.0 * d.groupCarCount[i] / d.groupCount[i];
        if (pct > topCarPct) { topCarPct = pct; topCarGroup = i; }
    }

    // 4. Highest-emission city
    int topCity = 0;
    for (int c = 1; c < NUM_CITIES; ++c)
        if (d.cityEmission[c] > d.cityEmission[topCity]) topCity = c;

    // 5. Age group with highest walking proportion
    int topWalkGroup = 0;
    double topWalkPct = 0.0;
    for (int i = 0; i < NUM_GROUPS; ++i) {
        if (d.groupCount[i] == 0) continue;
        double pct = 100.0 * d.groupWalkCount[i] / d.groupCount[i];
        if (pct > topWalkPct) { topWalkPct = pct; topWalkGroup = i; }
    }

    // ── Print ─────────────────────────────────────────────────
    std::cout << "\n";
    menuLine('=', 64);
    std::cout << "  INSIGHTS & RECOMMENDATIONS  [" << ds << "]\n";
    menuLine('=', 64);

    // Finding 1 — age group emissions
    std::cout << "\n  FINDING 1 — Age Group with Highest Total Emissions\n";
    menuLine('-', 64);
    std::cout << "  +-" << std::string(42,'-') << "-+-" << std::string(16,'-') << "-+-" << std::string(7,'-') << "-+\n";
    std::cout << "  | " << std::left  << std::setw(42) << "Age Group"
              << " | " << std::right << std::setw(16) << "Total CO2 (kg)"
              << " | " << std::right << std::setw(7)  << "Count"
              << " |\n";
    std::cout << "  +-" << std::string(42,'-') << "-+-" << std::string(16,'-') << "-+-" << std::string(7,'-') << "-+\n";
    for (int i = 0; i < NUM_GROUPS; ++i) {
        std::string label = AGE_GROUP_LABELS[i];
        if (i == topGroup) label += " ***";
        std::cout << "  | " << std::left  << std::setw(42) << label
                  << " | " << std::right << std::setw(16) << std::fixed << std::setprecision(2) << d.groupEmission[i]
                  << " | " << std::right << std::setw(7)  << d.groupCount[i]
                  << " |\n";
    }
    std::cout << "  +-" << std::string(42,'-') << "-+-" << std::string(16,'-') << "-+-" << std::string(7,'-') << "-+\n";
    std::cout << "  >> " << AGE_GROUP_LABELS[topGroup] << " produces the highest total CO2: "
              << std::fixed << std::setprecision(2) << d.groupEmission[topGroup] << " kg/month ("
              << std::setprecision(1) << (d.grandTotal > 0 ? 100.0*d.groupEmission[topGroup]/d.grandTotal : 0)
              << "% of all emissions).\n";

    // Finding 2 — bicycle vs car by age group
    std::cout << "\n  FINDING 2 — Bicycle vs Car Usage by Age Group\n";
    menuLine('-', 64);
    std::cout << "  +-" << std::string(42,'-') << "-+-" << std::string(8,'-') << "-+-" << std::string(8,'-') << "-+-" << std::string(9,'-') << "-+-" << std::string(9,'-') << "-+\n";
    std::cout << "  | " << std::left  << std::setw(42) << "Age Group"
              << " | " << std::right << std::setw(8)  << "Bike %"
              << " | " << std::right << std::setw(8)  << "Car %"
              << " | " << std::right << std::setw(9)  << "Bikes"
              << " | " << std::right << std::setw(9)  << "Cars"
              << " |\n";
    std::cout << "  +-" << std::string(42,'-') << "-+-" << std::string(8,'-') << "-+-" << std::string(8,'-') << "-+-" << std::string(9,'-') << "-+-" << std::string(9,'-') << "-+\n";
    for (int i = 0; i < NUM_GROUPS; ++i) {
        if (d.groupCount[i] == 0) continue;
        double bpct = 100.0 * d.groupBicycleCount[i] / d.groupCount[i];
        double cpct = 100.0 * d.groupCarCount[i]     / d.groupCount[i];
        std::string label = AGE_GROUP_LABELS[i];
        if (i == topBikeGroup) label += " (top bike)";
        if (i == topCarGroup)  label += " (top car)";
        std::cout << "  | " << std::left  << std::setw(42) << label
                  << " | " << std::right << std::setw(7) << std::fixed << std::setprecision(1) << bpct << "%"
                  << " | " << std::right << std::setw(7) << std::fixed << std::setprecision(1) << cpct << "%"
                  << " | " << std::right << std::setw(9) << d.groupBicycleCount[i]
                  << " | " << std::right << std::setw(9) << d.groupCarCount[i]
                  << " |\n";
    }
    std::cout << "  +-" << std::string(42,'-') << "-+-" << std::string(8,'-') << "-+-" << std::string(8,'-') << "-+-" << std::string(9,'-') << "-+-" << std::string(9,'-') << "-+\n";
    std::cout << "  >> Highest bicycle use: " << AGE_GROUP_LABELS[topBikeGroup]
              << " (" << std::fixed << std::setprecision(1) << topBikePct << "% ride bikes).\n";
    std::cout << "  >> Highest car dependency: " << AGE_GROUP_LABELS[topCarGroup]
              << " (" << std::fixed << std::setprecision(1) << topCarPct << "% drive).\n";

    // Finding 3 — city emissions
    std::cout << "\n  FINDING 3 — City with Highest Total Emissions\n";
    menuLine('-', 64);
    std::cout << "  +-" << std::string(8,'-') << "-+-" << std::string(10,'-') << "-+-" << std::string(16,'-') << "-+-" << std::string(16,'-') << "-+-" << std::string(9,'-') << "-+\n";
    std::cout << "  | " << std::left  << std::setw(8)  << "City"
              << " | " << std::right << std::setw(10) << "Residents"
              << " | " << std::right << std::setw(16) << "Total CO2 (kg)"
              << " | " << std::right << std::setw(16) << "Avg CO2/person"
              << " | " << std::right << std::setw(9)  << "% Total"
              << " |\n";
    std::cout << "  +-" << std::string(8,'-') << "-+-" << std::string(10,'-') << "-+-" << std::string(16,'-') << "-+-" << std::string(16,'-') << "-+-" << std::string(9,'-') << "-+\n";
    for (int c = 0; c < NUM_CITIES; ++c) {
        double avg = d.cityCount[c] > 0 ? d.cityEmission[c]/d.cityCount[c] : 0.0;
        double pct = d.grandTotal > 0 ? 100.0*d.cityEmission[c]/d.grandTotal : 0.0;
        std::string label = cNames[c];
        if (c == topCity) label += " ***";
        std::cout << "  | " << std::left  << std::setw(8)  << label
                  << " | " << std::right << std::setw(10) << d.cityCount[c]
                  << " | " << std::right << std::setw(16) << std::fixed << std::setprecision(2) << d.cityEmission[c]
                  << " | " << std::right << std::setw(16) << std::fixed << std::setprecision(2) << avg
                  << " | " << std::right << std::setw(8)  << std::fixed << std::setprecision(1) << pct << "%"
                  << " |\n";
    }
    std::cout << "  +-" << std::string(8,'-') << "-+-" << std::string(10,'-') << "-+-" << std::string(16,'-') << "-+-" << std::string(16,'-') << "-+-" << std::string(9,'-') << "-+\n";
    std::cout << "  >> " << cNames[topCity] << " has the highest total emissions: "
              << std::fixed << std::setprecision(2) << d.cityEmission[topCity] << " kg CO2/month.\n";

    // ── Recommendations ───────────────────────────────────────
    std::cout << "\n";
    menuLine('=', 64);
    std::cout << "  RECOMMENDATIONS FOR CITY PLANNERS\n";
    menuLine('=', 64);

    // Rec 1 — target the high-emission age group
    std::cout << "\n  [1] Target Subsidised Public Transport for "
              << AGE_GROUP_LABELS[topCarGroup] << "\n";
    menuLine('-', 64);
    std::cout << "  This group drives the most (" << std::fixed << std::setprecision(1)
              << topCarPct << "% car users) and generates the bulk of\n"
              << "  transport-related CO2. Offering subsidised bus or carpool passes\n"
              << "  to working-age residents aged 26-60 could reduce total city\n"
              << "  emissions significantly.\n";

    // Rec 2 — leverage existing bike culture
    std::cout << "\n  [2] Expand Cycling Infrastructure for "
              << AGE_GROUP_LABELS[topBikeGroup] << "\n";
    menuLine('-', 64);
    std::cout << "  " << std::fixed << std::setprecision(1) << topBikePct
              << "% of this group already cycle. Protected bike lanes, secure\n"
              << "  parking, and bike-sharing schemes would reinforce this low-carbon\n"
              << "  behaviour and encourage neighbouring age groups to adopt it.\n";

    // Rec 3 — focus on the highest-emission city
    std::cout << "\n  [3] Prioritise Emission Reduction in " << cNames[topCity] << "\n";
    menuLine('-', 64);
    double topCityAvg = d.cityCount[topCity] > 0
                      ? d.cityEmission[topCity]/d.cityCount[topCity] : 0.0;
    std::cout << "  " << cNames[topCity] << " accounts for "
              << std::setprecision(1) << (d.grandTotal>0 ? 100.0*d.cityEmission[topCity]/d.grandTotal : 0.0)
              << "% of combined emissions (avg "
              << std::setprecision(2) << topCityAvg << " kg CO2/person/month).\n"
              << "  High-frequency, low-cost public transit routes and park-and-ride\n"
              << "  facilities in this city should be the first capital investment.\n";

    // Rec 4 — carpooling incentives
    std::cout << "\n  [4] Institutionalise Employer-Led Carpooling Schemes\n";
    menuLine('-', 64);
    std::cout << "  Carpool trips emit 50-80% less CO2 per person than solo car trips.\n"
              << "  City councils can mandate or incentivise employers with >50 staff\n"
              << "  to operate shared-ride programmes, directly targeting the dominant\n"
              << "  working-adult commuter population.\n";

    // Rec 5 — walking-friendly urban design
    std::cout << "\n  [5] Promote 15-Minute Neighbourhood Design for Short Trips\n";
    menuLine('-', 64);
    std::cout << "  " << std::fixed << std::setprecision(1) << topWalkPct
              << "% of " << AGE_GROUP_LABELS[topWalkGroup] << " already walk.\n"
              << "  Rezoning residential areas to place workplaces, schools, and\n"
              << "  shops within 2-3 km eliminates the need for motorised transport\n"
              << "  for short trips, yielding zero-emission journeys at no ongoing cost.\n";

    menuLine('=', 64);
    std::cout << "\n";
}

// ── Wrappers callable from menu ───────────────────────────────
static void showInsights(const ResidentArray& arr) {
    InsightData d = buildInsights(arr);
    printInsights(d, "Fixed-Size Array");
}
static void showInsights(const LinkedList& list) {
    InsightData d = buildInsights(list);
    printInsights(d, "Singly Linked List");
}

// =============================================================
//  ARRAY SUB-MENU
// =============================================================
static void menuArray(ResidentArray& arr, LinkedList& list,
                      bool& loaded, SessionPerf& sp) {
    const std::string FILES[3] = {
        "datasets/dataset1-cityA.csv",
        "datasets/dataset2-cityB.csv",
        "datasets/dataset3-cityC.csv"
    };
    const std::string CITIES[3] = { "CityA", "CityB", "CityC" };

    while (true) {
        menuTitle("  ARRAY-BASED PROGRAM");
        if (loaded) std::cout << "  Data loaded: " << arr.count << " residents\n";
        else        std::cout << "  Data not loaded yet\n";
        menuLine('-');
        std::cout << "  1. Load Data from CSV Files\n";
        std::cout << "  2. Carbon Emission Analysis\n";
        std::cout << "     (2a) By Age Group   (2b) By City\n";
        std::cout << "  3. Sorting Experiments\n";
        std::cout << "  4. Searching Experiments\n";
        std::cout << "  5. Performance Summary\n";
        std::cout << "  6. Insights & Recommendations\n";
        std::cout << "  0. Back to Main Menu\n";
        menuLine('-');
        int ch = readInt("  Choice: ");

        switch (ch) {
        case 0:
            return;

        case 1:
            arr.count = 0;
            for (int i = 0; i < 3; ++i) loadCSV(arr, FILES[i], CITIES[i]);
            // also reload list so binary search / copy operations stay consistent
            { Node* c = list.head; while (c) { Node* t = c->next; delete c; c = t; } list.head = nullptr; list.size = 0; }
            for (int i = 0; i < 3; ++i) loadCSV(list, FILES[i], CITIES[i]);
            loaded = true;
            menuLine('-');
            std::cout << "  Loaded " << arr.count << " residents into Array.\n";
            std::cout << "  Loaded " << list.size << " residents into Linked List.\n";
            menuLine('-');
            printDataPreview(arr);
            break;

        case 2: {
            if (!loaded) { std::cout << "  Please load data first (Option 1).\n"; break; }
            menuTitle("  CARBON EMISSION ANALYSIS — Array");
            std::cout << "  a. By Age Group\n";
            std::cout << "  b. By City\n";
            std::cout << "  c. Both\n";
            std::cout << "  0. Back\n";
            int sub = readInt("  Choice: ");
            if (sub == 1 || sub == 3) analyseByAgeGroup(arr);
            if (sub == 2 || sub == 3) analyseByCity(arr);
            if (sub != 0 && sub != 1 && sub != 2 && sub != 3)
                std::cout << "  Invalid choice.\n";
            break;
        }

        case 3:
            if (!loaded) { std::cout << "  Please load data first (Option 1).\n"; break; }
            doSortArray(arr, sp);
            break;

        case 4:
            if (!loaded) { std::cout << "  Please load data first (Option 1).\n"; break; }
            doSearchArray(arr, sp);
            break;

        case 5:
            showPerfSummary(sp);
            break;

        case 6:
            if (!loaded) { std::cout << "  Please load data first (Option 1).\n"; break; }
            showInsights(arr);
            break;

        default:
            std::cout << "  Invalid choice.\n";
        }
    }
}

// =============================================================
//  LINKED LIST SUB-MENU
// =============================================================
static void menuList(ResidentArray& arr, LinkedList& list,
                     bool& loaded, SessionPerf& sp) {
    const std::string FILES[3] = {
        "datasets/dataset1-cityA.csv",
        "datasets/dataset2-cityB.csv",
        "datasets/dataset3-cityC.csv"
    };
    const std::string CITIES[3] = { "CityA", "CityB", "CityC" };

    while (true) {
        menuTitle("  LINKED LIST-BASED PROGRAM");
        if (loaded) std::cout << "  Data loaded: " << list.size << " residents\n";
        else        std::cout << "  Data not loaded yet\n";
        menuLine('-');
        std::cout << "  1. Load Data from CSV Files\n";
        std::cout << "  2. Carbon Emission Analysis\n";
        std::cout << "     (2a) By Age Group   (2b) By City\n";
        std::cout << "  3. Sorting Experiments\n";
        std::cout << "  4. Searching Experiments\n";
        std::cout << "  5. Performance Summary\n";
        std::cout << "  6. Insights & Recommendations\n";
        std::cout << "  0. Back to Main Menu\n";
        menuLine('-');
        int ch = readInt("  Choice: ");

        switch (ch) {
        case 0:
            return;

        case 1:
            arr.count = 0;
            { Node* c = list.head; while (c) { Node* t = c->next; delete c; c = t; } list.head = nullptr; list.size = 0; }
            for (int i = 0; i < 3; ++i) loadCSV(arr, FILES[i], CITIES[i]);
            for (int i = 0; i < 3; ++i) loadCSV(list, FILES[i], CITIES[i]);
            loaded = true;
            menuLine('-');
            std::cout << "  Loaded " << arr.count << " residents into Array.\n";
            std::cout << "  Loaded " << list.size << " residents into Linked List.\n";
            menuLine('-');
            printDataPreview(list);
            break;

        case 2: {
            if (!loaded) { std::cout << "  Please load data first (Option 1).\n"; break; }
            menuTitle("  CARBON EMISSION ANALYSIS — Linked List");
            std::cout << "  1. By Age Group\n";
            std::cout << "  2. By City\n";
            std::cout << "  3. Both\n";
            std::cout << "  0. Back\n";
            int sub = readInt("  Choice: ");
            if (sub == 1 || sub == 3) analyseByAgeGroup(list);
            if (sub == 2 || sub == 3) analyseByCity(list);
            if (sub != 0 && sub != 1 && sub != 2 && sub != 3)
                std::cout << "  Invalid choice.\n";
            break;
        }

        case 3:
            if (!loaded) { std::cout << "  Please load data first (Option 1).\n"; break; }
            doSortList(list, sp);
            break;

        case 4:
            if (!loaded) { std::cout << "  Please load data first (Option 1).\n"; break; }
            doSearchList(list, sp);
            break;

        case 5:
            showPerfSummary(sp);
            break;

        case 6:
            if (!loaded) { std::cout << "  Please load data first (Option 1).\n"; break; }
            showInsights(list);
            break;

        default:
            std::cout << "  Invalid choice.\n";
        }
    }
}

// =============================================================
//  main
// =============================================================
int main() {
    ResidentArray arr;
    LinkedList    list;
    bool          loaded = false;
    SessionPerf   sp;

    while (true) {
        std::cout << "\n";
        menuLine('=', 60);
        std::cout << "    DSTR Assignment — Carbon Emission Analyser\n";
        menuLine('=', 60);
        std::cout << "  1. Array-based Program\n";
        std::cout << "  2. Linked List-based Program\n";
        std::cout << "  0. Exit\n";
        menuLine('-', 60);
        int ch = readInt("  Choice: ");

        if (ch == 0) {
            std::cout << "  Goodbye.\n";
            break;
        }
        if (ch == 1) { menuArray(arr, list, loaded, sp); continue; }
        if (ch == 2) { menuList (arr, list, loaded, sp); continue; }
        std::cout << "  Invalid choice. Please enter 0, 1, or 2.\n";
    }
    return 0;
}
