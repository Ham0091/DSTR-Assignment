// ============================================================
//  DSTR Assignment — Two data structures for resident data
//  Structures: fixed-size array  +  singly linked list
// ============================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

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

    // Append a new node to the tail
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

    // Destructor — free all nodes
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
//  Helper — trim leading/trailing whitespace from a string
// ─────────────────────────────────────────────────────────────
static std::string trim(const std::string& s) {
    const char* ws = " \t\r\n";
    std::size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    std::size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

// ─────────────────────────────────────────────────────────────
//  Helper — parse one CSV data line into a Resident
//  Column order (all three files):
//    ResidentID, Age, ModeOfTransport, DailyDistance,
//    CarbonEmissionFactor, AvgDaysPerMonth
// ─────────────────────────────────────────────────────────────
static bool parseLine(const std::string& line,
                      const std::string& city,
                      Resident&          out)
{
    std::stringstream ss(line);
    std::string token;

    // ResidentID
    if (!std::getline(ss, token, ',')) return false;
    out.residentID = trim(token);
    if (out.residentID.empty()) return false;

    // Age
    if (!std::getline(ss, token, ',')) return false;
    try { out.age = std::stoi(trim(token)); }
    catch (...) { return false; }

    // ModeOfTransport  (may contain spaces, but no commas in these datasets)
    if (!std::getline(ss, token, ',')) return false;
    out.modeOfTransport = trim(token);

    // DailyDistance
    if (!std::getline(ss, token, ',')) return false;
    try { out.dailyDistance = std::stod(trim(token)); }
    catch (...) { return false; }

    // CarbonEmissionFactor
    if (!std::getline(ss, token, ',')) return false;
    try { out.carbonEmissionFactor = std::stod(trim(token)); }
    catch (...) { return false; }

    // AvgDaysPerMonth  (last column — read remainder of line)
    if (!std::getline(ss, token)) return false;
    try { out.avgDaysPerMonth = std::stoi(trim(token)); }
    catch (...) { return false; }

    out.cityLabel       = city;
    out.monthlyEmission = out.dailyDistance
                        * out.carbonEmissionFactor
                        * out.avgDaysPerMonth;
    return true;
}

// ─────────────────────────────────────────────────────────────
//  loadCSV — array version
// ─────────────────────────────────────────────────────────────
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
    std::getline(file, line);   // skip header row

    while (std::getline(file, line)) {
        if (trim(line).empty()) continue;

        if (arr.count >= MAX_SIZE) {
            std::cerr << "Warning: array capacity (" << MAX_SIZE
                      << ") reached. Remaining records skipped.\n";
            break;
        }

        Resident r;
        if (parseLine(line, city, r))
            arr.data[arr.count++] = r;
    }
    return true;
}

// ─────────────────────────────────────────────────────────────
//  loadCSV — linked list version
// ─────────────────────────────────────────────────────────────
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
    std::getline(file, line);   // skip header row

    while (std::getline(file, line)) {
        if (trim(line).empty()) continue;
        Resident r;
        if (parseLine(line, city, r))
            list.append(r);
    }
    return true;
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

// ─────────────────────────────────────────────────────────────
//  main
// ─────────────────────────────────────────────────────────────
int main() {
    // File paths are relative to the project root (run from there)
    const std::string FILES[3] = {
        "datasets/dataset1-cityA.csv",
        "datasets/dataset2-cityB.csv",
        "datasets/dataset3-cityC.csv"
    };
    const std::string CITIES[3] = { "CityA", "CityB", "CityC" };

    // ── 1. Fixed-size array ──────────────────────────────────
    ResidentArray arr;
    for (int i = 0; i < 3; ++i)
        loadCSV(arr, FILES[i], CITIES[i]);

    std::cout << "========================================\n";
    std::cout << " Data Structure 1 — Fixed-Size Array\n";
    std::cout << "========================================\n";
    std::cout << "Total residents loaded : " << arr.count << " / " << MAX_SIZE << "\n\n";

    std::cout << "--- First 5 records ---\n";
    for (int i = 0; i < 5 && i < arr.count; ++i)
        printResident(arr.data[i]);

    std::cout << "\n--- Last 5 records ---\n";
    int start = arr.count - 5 < 0 ? 0 : arr.count - 5;
    for (int i = start; i < arr.count; ++i)
        printResident(arr.data[i]);

    // ── 2. Singly linked list ────────────────────────────────
    LinkedList list;
    for (int i = 0; i < 3; ++i)
        loadCSV(list, FILES[i], CITIES[i]);

    std::cout << "\n========================================\n";
    std::cout << " Data Structure 2 — Singly Linked List\n";
    std::cout << "========================================\n";
    std::cout << "Total residents loaded : " << list.size << "\n\n";

    std::cout << "--- First 5 records ---\n";
    Node* cur = list.head;
    for (int i = 0; i < 5 && cur; ++i, cur = cur->next)
        printResident(cur->resident);

    std::cout << "\n--- Last 5 records ---\n";
    cur = list.head;
    int skip = list.size - 5;
    for (int i = 0; i < skip && cur; ++i)
        cur = cur->next;
    while (cur) {
        printResident(cur->resident);
        cur = cur->next;
    }

    return 0;
}
