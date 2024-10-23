// main.cpp
#include "profiler.hpp"
#include <iostream> 
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>      // For std::vector
#include <algorithm>   // For std::sort

// Initialize the global profiler pointer
Profiler* profiler = nullptr; 

constexpr float DEGREES_TO_RADIANS = (3.1415926535897932384626433f / 180.0f);

// Function Prototypes
void Test1();
void Test2();
void Test3();
void RunInterleavedTest();
void RunTest();
void InsertionSortTest();    // New Insertion Sort Test
void BinarySearchTest();     // New Binary Search Test

// Existing Test1 Function (Refactored to Use Macros)
void Test1()
{
    // Start profiling for the entire test
    PROFILER_ENTER("Trig Speed Test");
    
    constexpr int TRIG_TEST_NUM_ENTRIES = 100000; // Number of entries for testing
    std::vector<float> randomYawDegreeTable(TRIG_TEST_NUM_ENTRIES, 0.0f); // Use std::vector for safety
    
    // Enter section for generating random angles
    PROFILER_ENTER("Random Angle Generation");
    for (int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++)
    {
        randomYawDegreeTable[i] = 360.0f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX); // Fill the array with random angles
    }
    // Exit the angle generation section
    PROFILER_EXIT("Random Angle Generation");
    
    float biggestSoFar = 0.f; // Variable to track the largest sum of cos and sin values
    
    // Enter section for total cosine and sine computations
    PROFILER_ENTER("Total Cos and Sin Compute");
    for (int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++)
    {
        // Enter section for each cosine and sine computation
        PROFILER_ENTER("Cos and Sine Compute");
        float yawDegrees = randomYawDegreeTable[i]; // Get the current angle
        float cosDegrees = cosf(yawDegrees * DEGREES_TO_RADIANS); // Compute cosine
        float sinDegrees = sinf(yawDegrees * DEGREES_TO_RADIANS); // Compute sine

        // Exit the individual computation section
        PROFILER_EXIT("Cos and Sine Compute");
        
        // Update the biggest value found
        if (cosDegrees + sinDegrees > biggestSoFar)
        {
            biggestSoFar = cosDegrees + sinDegrees; // Update if the new value is larger
        }
    }
    // Exit the total computation section
    PROFILER_EXIT("Total Cos and Sin Compute");

    // Finally, exit the main test section
    PROFILER_EXIT("Trig Speed Test");

    // Output the result to the console
    std::cout << "Biggest cos+sin = " << biggestSoFar << std::endl; 
}

// Existing Test2 Function (Already Uses Macros)
void Test2()
{
    PROFILER_ENTER("Trig Speed Test");
    constexpr int TRIG_TEST_NUM_ENTRIES = 100000;
    std::vector<float> randomYawDegreeTable(TRIG_TEST_NUM_ENTRIES, 0.0f);
    
    // Enter section for generating random angles
    PROFILER_ENTER("Random Angle Generation");
    for(int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++)
    {
        randomYawDegreeTable[i] = 360.0f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
    // Exit the angle generation section
    PROFILER_EXIT("Random Angle Generation");

    float biggestSoFar = 0.f; 
    
    // Enter section for total cosine and sine computations
    PROFILER_ENTER("Total Cos and Sin Compute");
    for(int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++)
    {
        // Enter section for each cosine and sine computation
        PROFILER_ENTER("Cos and Sine Compute");
        float yawDegrees = randomYawDegreeTable[i];
        float cosDegrees = cosf(yawDegrees * DEGREES_TO_RADIANS);
        float sinDegrees = sinf(yawDegrees * DEGREES_TO_RADIANS);
        
        // Exit the individual computation section
        PROFILER_EXIT("Cos and Sine Compute");
        
        // Update the biggest value found
        if(cosDegrees + sinDegrees > biggestSoFar)
        {
            biggestSoFar = cosDegrees + sinDegrees;
        }
    }
    // Exit the total computation section
    PROFILER_EXIT("Total Cos and Sin Compute");

    // Exit the main test section
    PROFILER_EXIT("Trig Speed Test");

    // Output the result
    std::cout << "Biggest cos+sin = " << biggestSoFar << std::endl; 
}

// Existing Test3 Function (Refactored to Use Macros)
void Test3()
{
    PROFILER_ENTER("Trig Speed Test"); // Removed ProfilerScopeObject
    
    constexpr int TRIG_TEST_NUM_ENTRIES = 100000;
    std::vector<float> randomYawDegreeTable(TRIG_TEST_NUM_ENTRIES, 0.0f);
    
    // Enter section for generating random angles
    PROFILER_ENTER("Random Angle Generation");
    for(int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++)
    {
        randomYawDegreeTable[i] = 360.0f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
    // Exit the angle generation section
    PROFILER_EXIT("Random Angle Generation");

    float biggestSoFar = 0.f; 
    
    // Enter section for total cosine and sine computations
    PROFILER_ENTER("Total Cos and Sin Compute");
    for(int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++)
    {
        // Enter section for each cosine and sine computation
        PROFILER_ENTER("Cos and Sine Compute");
        float yawDegrees = randomYawDegreeTable[i];
        float cosDegrees = cosf(yawDegrees * DEGREES_TO_RADIANS);
        float sinDegrees = sinf(yawDegrees * DEGREES_TO_RADIANS);

        // Exit the individual computation section
        PROFILER_EXIT("Cos and Sine Compute");

        // Update the biggest value found
        if(cosDegrees + sinDegrees > biggestSoFar)
        {
            biggestSoFar = cosDegrees + sinDegrees;
        }
    }
    // Exit the total computation section
    PROFILER_EXIT("Total Cos and Sin Compute");

    // Exit the main test section
    PROFILER_EXIT("Trig Speed Test");

    // Output the result
    std::cout << "Biggest cos+sin = " << biggestSoFar << std::endl; 
}

// Existing RunInterleavedTest Function (Already Uses Macros)
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

// New Binary Search Test Function
void BinarySearchTest() {
    PROFILER_ENTER("Binary Search Test");

    constexpr int SEARCH_TEST_SIZE = 100000; // Size of the array to search
    std::vector<int> data(SEARCH_TEST_SIZE);

    // Populate the array with random numbers
    for(auto &num : data) {
        num = rand();
    }

    // Sort the array to prepare for Binary Search
    std::sort(data.begin(), data.end());

    // Choose a target element (existing)
    int target = data[rand() % SEARCH_TEST_SIZE]; // Ensures the target exists

    // Alternatively, for testing non-existent elements, use:
    // int target = -1; // Assuming all random numbers are positive

    // Perform Binary Search
    int left = 0;
    int right = SEARCH_TEST_SIZE -1;
    int foundIndex = -1;

    while(left <= right) {
        int mid = left + (right - left) / 2;

        if(data[mid] == target) {
            foundIndex = mid;
            break;
        }
        else if(data[mid] < target) {
            left = mid + 1;
        }
        else {
            right = mid -1;
        }
    }

    PROFILER_EXIT("Binary Search Test");

    if(foundIndex != -1) {
        std::cout << "Binary Search: Element " << target << " found at index " << foundIndex << "." << std::endl;
    }
    else {
        std::cout << "Binary Search: Element " << target << " not found in the array." << std::endl;
    }
}

// Updated RunTest Function to Include New Tests
void RunTest() {
    // RunInterleavedTest(); // Existing interleaved profiling
    // Test1();               // Refactored Test1
    // Test2();               // Existing Test2
    //Test3();               // Refactored Test3
    for(int i = 0; i< 10; i++)
    {
        std::cout<<"Run: " << i << std::endl;
        InsertionSortTest();   // New Insertion Sort Test
        BinarySearchTest(); 
    }
       // New Binary Search Test
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

    // Calculate statistics based on recorded profiling data
    profiler->calculateStats();

    // Print statistics to the console
    profiler->printStats();

    // Output statistics to a CSV file
    profiler->printStatsToCSV("Data/profiler_stats.csv"); // Ensure the 'Data' directory exists

    // Output statistics to a JSON file
    profiler->printStatsToJSON("Data/profiler_stats.json"); // Ensure the 'Data' directory exists

    profiler->printStartAndStopToCSV("Data/detailedStats.csv");
    // profiler->printAllRuns("Data/allruns.csv");
    // Print statistics again to the console (optional)
    // profiler->printStats(); // Print stats to console

    // Clean up
    // If Profiler uses a Singleton with a static instance, **do not** delete it manually
    delete profiler; 
    // profiler = nullptr; 
    return 0; 
}
