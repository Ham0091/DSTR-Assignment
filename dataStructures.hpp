#ifndef DATASTRUCTURES_HPP
#define DATASTRUCTURES_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

// ============================================================
// SECTION 1: Resident Struct
// ============================================================
// This struct stores all information for one resident.
// monthlyEmission is pre-calculated to avoid recalculation.
struct Resident {
    std::string residentID;
    int         age;
    std::string modeOfTransport;
    double      dailyDistance;          // in km
    double      carbonEmissionFactor;   // kg CO2 / km
    int         avgDaysPerMonth;
    std::string cityLabel;
    double      monthlyEmission;        // pre-calculated: dailyDistance × carbonEmissionFactor × avgDaysPerMonth
};

// ============================================================
// SECTION 2: ResidentArray Struct (Fixed-Size Array)
// ============================================================
// This struct wraps a fixed array of Resident objects.
// count tracks how many residents are actually loaded.
// MAX_SIZE is 600 (enough for ~200 residents per dataset × 3 datasets)
const int MAX_SIZE = 600;

struct ResidentArray {
    Resident data[MAX_SIZE];
    int      count;

    // Constructor: Initialize count to 0
    ResidentArray() : count(0) {}
};

// ============================================================
// SECTION 3: Node Struct for Linked List
// ============================================================
// Each node holds a Resident and a pointer to the next node.
// next is nullptr for the last node in the list.
struct Node {
    Resident resident;
    Node*    next;

    // Constructor: Initialize with a Resident and set next to nullptr
    Node(const Resident& r) : resident(r), next(nullptr) {}
};

// ============================================================
// SECTION 4: LinkedList Struct
// ============================================================
// Singly linked list for dynamic Resident storage.
// Unlike arrays, nodes are created dynamically as needed.
struct LinkedList {
    Node* head;
    int   size;

    // Constructor: Initialize empty list
    LinkedList() : head(nullptr), size(0) {}

    // Destructor: Delete all nodes to prevent memory leaks
    ~LinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
        size = 0;
    }

    // Method: Add a new resident at the end of the list
    void append(const Resident& r) {
        Node* newNode = new Node(r);
        
        if (!head) {
            // List is empty, new node becomes the head
            head = newNode;
        } else {
            // Traverse to the last node and attach new node
            Node* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
        size++;
    }
};

// ============================================================
// SECTION 5: File Loading Functions (Overloaded)
// ============================================================
// These functions load CSV files into either array or linked list.
// Function overloading allows both to be named "loadCSV".

// Load CSV into fixed-size array
void loadCSV(ResidentArray& arr, const std::string& filename, const std::string& cityLabel);

// Load CSV into linked list
void loadCSV(LinkedList& list, const std::string& filename, const std::string& cityLabel);

// ============================================================
// SECTION 6: Helper/Display Functions
// ============================================================
// Print a single resident in formatted table row
void printResident(const Resident& r);

// Print horizontal line for table borders
void printHorizontalLine();

// Center text in console output
void printCentered(const std::string& text);

#endif // DATASTRUCTURES_HPP
