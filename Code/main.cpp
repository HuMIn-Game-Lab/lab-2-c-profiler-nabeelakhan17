// main.cpp
#include "profiler.hpp"
#include <iostream> 
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>      // For std::vector
#include <algorithm>   // For std::sort
#include <thread>   // For std::this_thread::sleep_for
#include <chrono>   // For std::chrono::seconds
#include <list>

// Initialize the global profiler pointer
Profiler* profiler = nullptr; 

constexpr float DEGREES_TO_RADIANS = (3.1415926535897932384626433f / 180.0f);

// Function Prototypes
void Test1();
void Test2();
void Test3();
void RunInterleavedTest();
void RunTest();
void InsertionSortTest(); 
void InsertionSortTest2();   // New Insertion Sort Test
void BinarySearchTest();     // New Binary Search Test

// Existing Test1 Function (Refactored to Use Macros)'
void startStreamlitServer() {
    #ifdef _WIN32
    // Start Streamlit in the background and wait for it to start
    system("start /B streamlit run Code/combinedGraphs.py");
    std::this_thread::sleep_for(std::chrono::seconds(5));  // Increased wait time for server to start

    // Open the Streamlit app in the default browser
    system("start http://localhost:8501");

    #elif __APPLE__
    // For macOS, run Streamlit and open the browser
    system("streamlit run Code/combinedGraphs.py &");
    std::this_thread::sleep_for(std::chrono::seconds(5));  // Increased wait time
    system("open http://localhost:8501");

    #else // For Linux or other OS
    // For Linux, run Streamlit and open the browser
    system("streamlit run Code/combinedGraphs.py &");
    std::this_thread::sleep_for(std::chrono::seconds(5));  // Increased wait time
    system("xdg-open http://localhost:8501");

    #endif
}

void stopStreamlitServer() {
    #ifdef _WIN32
    system("taskkill /F /IM python.exe > nul 2>&1");
    #else
    system("pkill -f 'streamlit' > /dev/null 2>&1");
    #endif
}


void RunInterleavedTest() {
    // Start profiling for Task A
    PROFILER_ENTER("Task A");
    
    // Some operations for Task A
    for (int i = 0; i < 5; ++i) {
        PROFILER_ENTER("Task B");  // Start Task B
        // Some operations for Task B
        PROFILER_EXIT("Task B");   // End Task B
    }

    // Finalize Task A
    PROFILER_EXIT("Task A");
}

// New Insertion Sort Test Function
void InsertionSortTest() {
    PROFILER_ENTER("Insertion Sort Test");

    constexpr int SORT_TEST_SIZE = 100000; // Size of the array to sort
    std::vector<int> data(SORT_TEST_SIZE);

    // Populate the array with random numbers
    for(auto &num : data) {
        num = rand();
    }

    // Perform Insertion Sort
    for(int i = 1; i < SORT_TEST_SIZE; ++i) {
        int key = data[i];
        int j = i - 1;

        // Move elements of data[0..i-1], that are greater than key,
        // to one position ahead of their current position
        while(j >= 0 && data[j] > key) {
            data[j + 1] = data[j];
            --j;
        }
        data[j + 1] = key;
    }

    PROFILER_EXIT("Insertion Sort Test");

    // Verify sorting
    bool sorted = true;
    for(int i =1; i < SORT_TEST_SIZE; ++i) {
        if(data[i -1] > data[i]) {
            sorted = false;
            break;
        }
    }

    if(sorted) {
        std::cout << "Insertion Sort successfully sorted the array." << std::endl;
    }
    else {
        std::cout << "Insertion Sort failed to sort the array." << std::endl;
    }
}

//interleaved Insertion Sort: 
void InsertionSortTest2() {
    PROFILER_ENTER("Insertion Sort Test1");

    constexpr int SORT_TEST_SIZE = 100;  // Size of the array to sort
       std::vector<int> data = {
        857,  23,  546,  19,  678,  345,  789,  12,  456,  89,
        234,  567,  90,  123,  456,  789,  234,  56,  789,  123,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12
    };


    // // Populate the array with random numbers
    // PROFILER_ENTER("Random Number Generation");
    // for (auto &num : data) {
    //     num = rand();
    // }
    // PROFILER_EXIT("Random Number Generation");

    // Perform Insertion Sort
    for (int i = 1; i < SORT_TEST_SIZE; ++i) {
        PROFILER_ENTER("Outer Loop1");  // Start profiling the outer loop

        int key = data[i];
        int j = i - 1;

        // Interleaving the shifting process (Inner loop)
        while (j >= 0 && data[j] > key) {
            PROFILER_ENTER("Inner Loop (Shifting Elements)1");  // Start profiling inner loop

            data[j + 1] = data[j];  // Shift element

            PROFILER_EXIT("Inner Loop (Shifting Elements)1");   // End profiling inner loop
            --j;
        }

        // Place the key in its correct position
        PROFILER_ENTER("Key Assignment1");
        data[j + 1] = key;
        PROFILER_EXIT("Key Assignment1");

        PROFILER_EXIT("Outer Loop1");  // End profiling the outer loop
    }

    PROFILER_EXIT("Insertion Sort Test1");

    // Verify sorting
    bool sorted = true;
    for (int i = 1; i < SORT_TEST_SIZE; ++i) {
        if (data[i - 1] > data[i]) {
            sorted = false;
            break;
        }
    }

    if (sorted) {
        std::cout << "Insertion Sort successfully sorted the array." << std::endl;
    } else {
        std::cout << "Insertion Sort failed to sort the array." << std::endl;
    }
}

void InsertionSortTest2_Modified() {
    PROFILER_ENTER("Insertion Sort Test2");

    constexpr int SORT_TEST_SIZE = 100;  // Reduced size for testing
       std::vector<int> data = {
        857,  23,  546,  19,  678,  345,  789,  12,  456,  89,
        234,  567,  90,  123,  456,  789,  234,  56,  789,  123,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12
    };


    // Populate the array with random numbers
    // PROFILER_ENTER("Random Number Generation");
    // for (auto &num : data) {
    //     num = rand();
    // }
    // PROFILER_EXIT("Random Number Generation");

    // Perform Insertion Sort
    for (int i = 1; i < SORT_TEST_SIZE; ++i) {
        PROFILER_ENTER("Outer Loop2");  // Start profiling the outer loop

        int key = data[i];
        int j = i - 1;

        // Shift elements to make space for the key without profiling each shift
        while (j >= 0 && data[j] > key) {
            PROFILER_ENTER("Inner Loop (Shifting Elements)2");  // Start profiling inner loop

            data[j + 1] = data[j];  // Shift element

            PROFILER_EXIT("Inner Loop (Shifting Elements)2");   // End profiling inner loop
            --j;
        }

        // Place the key in its correct position
        PROFILER_ENTER("Key Assignment2");
        data[j + 1] = key;
        PROFILER_EXIT("Key Assignment2");

        PROFILER_EXIT("Outer Loop2");  // End profiling the outer loop
    }

    PROFILER_EXIT("Insertion Sort Test2");

    // Verify sorting
    bool sorted = true;
    for (int i = 1; i < SORT_TEST_SIZE; ++i) {
        if (data[i -1] > data[i]) {
            sorted = false;
            break;
        }
    }

    if (sorted) {
        std::cout << "Insertion Sort successfully sorted the array." << std::endl;
    }
    else {
        std::cout << "Insertion Sort failed to sort the array." << std::endl;
    }
}

void InsertionSortTest2_Optimized() {
    PROFILER_ENTER("Insertion Sort Test (Optimized)3");

    constexpr int SORT_TEST_SIZE = 100;  // Reduced size for testing
       std::vector<int> data = {
        857,  23,  546,  19,  678,  345,  789,  12,  456,  89,
        234,  567,  90,  123,  456,  789,  234,  56,  789,  123,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12
    };
    

    // Perform Insertion Sort with Binary Search and optimized shifting
    for (int i = 1; i < SORT_TEST_SIZE; ++i) {
        PROFILER_ENTER("Outer Loop (Optimized)3");  // Start profiling the outer loop

        int key = data[i];

        // Find the insertion point using binary search
        auto insertion_point = std::upper_bound(data.begin(), data.begin() + i, key);

        // Shift elements to make space for the key using move_backward
        PROFILER_ENTER("Shifting Elements (Optimized)3");
        std::move_backward(insertion_point, data.begin() + i, data.begin() + i + 1);
        PROFILER_EXIT("Shifting Elements (Optimized)3");

        // Place the key in its correct position
        PROFILER_ENTER("Key Assignment (Optimized)3");
        *insertion_point = key;
        PROFILER_EXIT("Key Assignment (Optimized)3");

        PROFILER_EXIT("Outer Loop (Optimized)3");  // End profiling the outer loop
    }

    PROFILER_EXIT("Insertion Sort Test (Optimized)3");

    // Verify sorting
    bool sorted = true;
    for (int i = 1; i < SORT_TEST_SIZE; ++i) {
        if (data[i - 1] > data[i]) {
            sorted = false;
            break;
        }
    }

    if (sorted) {
        std::cout << "Optimized Insertion Sort successfully sorted the array." << std::endl;
    } else {
        std::cout << "Optimized Insertion Sort failed to sort the array." << std::endl;
    }
}
void InsertionSortLinkedList() {
    PROFILER_ENTER("Insertion Sort Test (Linked List)4");

    constexpr int SORT_TEST_SIZE = 100;  // Reduced size for testing
       std::vector<int> data = {
        857,  23,  546,  19,  678,  345,  789,  12,  456,  89,
        234,  567,  90,  123,  456,  789,  234,  56,  789,  123,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12,
        345,  678,  90,  234,  567,  890,  123,  456,  789,  12
    };
    // Perform Insertion Sort on linked list
    for (auto it = std::next(data.begin()); it != data.end(); ++it) {
        PROFILER_ENTER("Outer Loop4");  // Start profiling the outer loop

        int key = *it;
        auto j = it;

        // Shift elements in the list to make space for the key
        while (j != data.begin() && *std::prev(j) > key) {
            PROFILER_ENTER("Inner Loop (Shifting Elements)4");  // Start profiling inner loop

            *j = *std::prev(j);  // Shift element
            --j;

            PROFILER_EXIT("Inner Loop (Shifting Elements)4");   // End profiling inner loop
        }

        // Place the key in its correct position
        PROFILER_ENTER("Key Assignment4");
        *j = key;
        PROFILER_EXIT("Key Assignment4");

        PROFILER_EXIT("Outer Loop4");  // End profiling the outer loop
    }

    PROFILER_EXIT("Insertion Sort Test (Linked List)4");

    // Verify sorting
    bool sorted = true;
    auto prev = data.begin();
    for (auto it = std::next(data.begin()); it != data.end(); ++it) {
        if (*prev > *it) {
            sorted = false;
            break;
        }
        prev = it;
    }

    if (sorted) {
        std::cout << "Insertion Sort successfully sorted the list." << std::endl;
    } else {
        std::cout << "Insertion Sort failed to sort the list." << std::endl;
    }
}
// Updated RunTest Function to Include New Tests
void RunTest() {
    // RunInterleavedTest(); // Existing interleaved profiling
    
      InsertionSortTest2();
      InsertionSortTest2_Modified();
      InsertionSortTest2_Optimized();
      InsertionSortLinkedList();
}

int main(int argc, char** argv)
{
    // Initialize the profiler instance
    profiler = Profiler::GetInstance();
    if (!profiler) {
        std::cerr << "Failed to initialize Profiler." << std::endl;
        return 1;
    }

    // Run all tests
    RunTest();

    // // Calculate statistics based on recorded profiling data
    profiler->calculateStats();

     // Print statistics to the console
    profiler->printStats();

    // Output statistics to a CSV file
    profiler->printStatsToCSV("Data/profiler_stats.csv"); // Ensure the 'Data' directory exists

    // Output statistics to a JSON file
    profiler->printStatsToJSON("Data/profiler_stats.json"); // Ensure the 'Data' directory exists

    profiler->printStartAndStopToCSV("Data/detailedStats.csv");

    std::cout<<"Starting the stream lit server..."<< std::endl; 

    startStreamlitServer();

    std::cout<<"Stopping the server..."<<std::endl; 
    
    //stopStreamlitServer();

    // // Clean up
    // // If Profiler uses a Singleton with a static instance, **do not** delete it manually
    delete profiler; 
    // profiler = nullptr; 
    return 0; 
}
