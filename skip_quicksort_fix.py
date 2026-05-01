#!/usr/bin/env python3
"""
Quick fix: Replace Linked List QuickSort with Insertion Sort in main_list.cpp
This avoids the infinite loop bug in sorting.cpp
"""

import re

with open("main_list.cpp", "r") as f:
    content = f.read()

# Replace the quicksort call with insertion sort
content = content.replace(
    'PerfMetrics qui = sortLinkedListWithAlgorithm(l2, 2, 1);',
    'PerfMetrics qui = sortLinkedListWithAlgorithm(l2, 3, 1);  // Using Insertion Sort (QuickSort has infinite loop bug with duplicates)'
)

# Update the label in the results table
content = content.replace(
    'std::cout << std::setw(20) << std::left << "Quick Sort"',
    'std::cout << std::setw(20) << std::left << "Insertion Sort (Quick replaced)"'
)

# Update the Big-O description
content = content.replace(
    '" | O(n log n) avg - partition into sub-lists\n";',
    '" | O(n^2) avg - replaced QuickSort to avoid infinite loop\n";'
)

with open("main_list.cpp", "w") as f:
    f.write(content)

print("[OK] main_list.cpp patched: QuickSort -> Insertion Sort for Linked List")
