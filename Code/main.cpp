#include "profiler.hpp"
#include <iostream> 
#include <cstdlib>
#include <cstdio>
#include <cmath>

Profiler* profiler = nullptr; 
constexpr float DEGREES_TO_RADIANS = (3.1415926535897932384626433f / 180.0f);

void Test1()
{
    // Start profiling for the entire test
    profiler->EnterSection("Trig Speed Test");
    
    constexpr int TRIG_TEST_NUM_ENTRIES = 100000; // Number of entries for testing
    float randomYawDegreeTable[TRIG_TEST_NUM_ENTRIES] = {}; // Array to hold random angles
    
    // Enter section for generating random angles
    profiler->EnterSection("Random Angle Generation");
    for (int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++)
    {
        randomYawDegreeTable[i] = 360.0f * float(rand()) / float(RAND_MAX); // Fill the array with random angles
    }
    // Exit the angle generation section
    profiler->ExitSection("Random Angle Generation", __LINE__, __FILE__, __FUNCTION__);
    
    float biggestSoFar = 0.f; // Variable to track the largest sum of cos and sin values
    
    // Enter section for total cosine and sine computations
    profiler->EnterSection("Total Cos and Sin Compute");
    for (int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++)
    {
        // Enter section for each cosine and sine computation
        profiler->EnterSection("Cos and Sine Compute");
        float yawDegrees = randomYawDegreeTable[i]; // Get the current angle
        float cosDegrees = cosf(yawDegrees * DEGREES_TO_RADIANS); // Compute cosine
        float sinDegrees = sinf(yawDegrees * DEGREES_TO_RADIANS); // Compute sine

        // Exit the individual computation section
        profiler->ExitSection("Cos and Sine Compute", __LINE__, __FILE__, __FUNCTION__);
        
        // Update the biggest value found
        if (cosDegrees + sinDegrees > biggestSoFar)
        {
            biggestSoFar = cosDegrees + sinDegrees; // Update if the new value is larger
        }
    }
    // Exit the total computation section
    profiler->ExitSection("Total Cos and Sin Compute", __LINE__, __FILE__, __FUNCTION__);

    // Finally, exit the main test section
    profiler->ExitSection("Trig Speed Test", __LINE__, __FILE__, __FUNCTION__);

    // Output the result to the console
    std::cout << "Biggest cos+sin = " << biggestSoFar << std::endl; 
}

void Test2()
{
    PROFILER_ENTER("Trig Speed Test");
    constexpr int TRIG_TEST_NUM_ENTRIES = 100000;
    float randomYawDegreeTable[TRIG_TEST_NUM_ENTRIES] = {};
    
    // Enter section for generating random angles
    PROFILER_ENTER("Random Angle Generation");
    for(int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++)
    {
        randomYawDegreeTable[i] = 360.0f * float(rand()) / float(RAND_MAX);
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

 void Test3()
{
    ProfilerScopeObject myObject("Trig Speed Test"); // Automatically enters the section
    // PROFILER_ENTER("Trig Speed Test"); // No need for this as ProfilerScopeObject handles it
    
    constexpr int TRIG_TEST_NUM_ENTRIES = 100'000;
    float randomYawDegreeTable[TRIG_TEST_NUM_ENTRIES] = {};
    
    // Enter section for generating random angles
    PROFILER_ENTER("Random Angle Generation");
    for(int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++)
    {
        randomYawDegreeTable[i] = 360.0f * float(rand()) / float(RAND_MAX);
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

    // The ProfilerScopeObject will handle the exit of "Trig Speed Test" automatically
    // Output the result
    std::cout << "Biggest cos+sin = " << biggestSoFar << std::endl; 
}
void RunInterleavedTest() {
    // Start profiling for Task A
    profiler->EnterSection("Task A");
    
    // Some operations for Task A
    for (int i = 0; i < 5; ++i) {
        profiler->EnterSection("Task B");  // Start Task B
        // Some operations for Task B
        profiler->ExitSection("Task B");   // End Task B
        
        // Possibly some other operations in Task A
    }

    // Finalize Task A
    profiler->ExitSection("Task A");
}
void RunTest() {
    RunInterleavedTest(); // Call the interleaved profiling 
    Test1();
    Test2();
}

int main(int argc, char** argv)
{
    profiler = Profiler::GetInstance();

    RunTest();

    profiler->printStatsToCSV("profiler_stats.csv"); // Output statistics to a CSV file
    profiler->printStatsToJSON("profiler_stats.json"); // Output statistics to a JSON file
    profiler->printStats(); // Print stats to console

    // Clean up
    delete profiler; 
    profiler = nullptr; 
    return 0; 

}