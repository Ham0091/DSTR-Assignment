#ifndef DATASTRUCTURES_HPP
#define DATASTRUCTURES_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

// ---- core record ----
// holds all the fields for one resident
// monthlyEmission is stored so we don't recompute it every time
struct Resident {
    std::string residentID;
    int         age;
    std::string modeOfTransport;
    double      dailyDistance;          // km per day
    double      carbonEmissionFactor;   // kg CO2 / km
    int         avgDaysPerMonth;
    std::string cityLabel;
    double      monthlyEmission;        // pre-calc: distance * factor * avgDaysPerMonth
};

// ---- fixed-size array wrapper ----
// count = how many slots are actually used
// MAX_SIZE is 600 (3x datasets, give or take)
const int MAX_SIZE = 600;

struct ResidentArray {
    Resident data[MAX_SIZE];
    int      count;

    // ctor: start empty
    ResidentArray() : count(0) {}
};

// ---- linked list node ----
// one Resident + pointer to next (nullptr at the end)
struct Node {
    Resident resident;
    Node*    next;

    // ctor: stash resident, next starts null
    Node(const Resident& r) : resident(r), next(nullptr) {}
};

// ---- linked list container ----
// dynamic nodes, no fixed max size
struct LinkedList {
    Node* head;
    Node* tail;  // keep tail so append stays O(1)
    int   size;

    // ctor: empty list
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    // dtor: clean up nodes
    ~LinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    // append at end (O(1) with tail)
    void append(const Resident& r) {
        Node* newNode = new Node(r);
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }
};

// ---- CSV loaders ----
// same name, different target container

// load CSV into fixed-size array
void loadCSV(ResidentArray& arr, const std::string& filename, const std::string& cityLabel);

// load CSV into linked list
void loadCSV(LinkedList& list, const std::string& filename, const std::string& cityLabel);

// ---- small output helpers ----
// print one resident row
void printResident(const Resident& r);

// print table divider line
void printHorizontalLine();

// center text in console output
void printCentered(const std::string& text);

#endif // datastructures.hpp guard
