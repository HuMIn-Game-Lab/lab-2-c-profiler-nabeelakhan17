#include "profiler.hpp"

#include "time.hpp"
#include <iostream>



Profiler* Profiler::gProfiler = nullptr; 

TimeRecordStart:: TimeRecordStart (char const* sectionName, double secondsAtStart):sectionName(sectionName), secondsAtStart(secondsAtStart){}


TimeRecordStop:: TimeRecordStop(char const* sectionName, double secondsAtStop, int lineNumber, const char* fileName, const char* functionName):sectionName(sectionName), secondsAtStop(secondsAtStop){}
TimeRecordStop:: TimeRecordStop(char const* sectionName, double secondsAtStop){} 

TimeRecordStart::~TimeRecordStart(){}
TimeRecordStop:: ~TimeRecordStop(){}


Profiler:: Profiler()
{
    gProfiler = this; 
    startTimes.reserve(100); 
    elapsedTimes.reserve(1000000);


}
Profiler* Profiler::GetInstance()
{
    if(gProfiler == nullptr)
    {
        gProfiler = new Profiler(); 
    }

    return gProfiler; 
}
Profiler::~Profiler()
{
    // for(auto& start: startTimes)
    // {
    //     delete start; 
    // }
    // for(auto& elapsed: elapsedTimes)
    // {
    //     delete elapsed;
    // }
    for (auto& stat: stats)
    {
        delete stat.second; 

    }
}

ProfilerScopeObject::ProfilerScopeObject(char const* sectionName){

    Profiler::GetInstance()->EnterSection(sectionName);

}

ProfilerScopeObject::~ProfilerScopeObject(){

    Profiler::GetInstance()->ExitSection(sectionName);

}
void Profiler::EnterSection(char const* sectionName)
{
    std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex for thread safety
        
    activeSections.push(sectionName);

    double secondsAtStart = GetCurrentTimeSeconds(); 
    startTimes.emplace_back(sectionName, secondsAtStart);

}

void Profiler::ExitSection(const char* sectionName) {
    std::lock_guard<std::mutex> lock(mutex_); // Lock for thread safety

    double secondsAtStop = GetCurrentTimeSeconds(); // Get the current time

    if (startTimes.empty()) {
        std::cerr << "Error: No sections to exit." << std::endl;
        return; // Prevent accessing an empty vector
    }

    TimeRecordStart const& currentSection = startTimes.back(); // Get the last section started
    double elapsedTime = secondsAtStop - currentSection.secondsAtStart; // Calculate elapsed time

    // Update the section's statistics
    ReportSectionTime(sectionName, elapsedTime); 
    startTimes.pop_back(); // Remove the last section from the stack

    // Check active sections to ensure proper matching
    if (!activeSections.empty()) {
        const char* lastSectionName = activeSections.top().c_str(); // Get the last entered section name

        // Check if the exiting section matches the last entered section
        if (strcmp(lastSectionName, sectionName) != 0) {
            std::cerr << "Error: Exiting section " << sectionName 
                      << " does not match the last entered section " << lastSectionName << "." << std::endl;
        } else {
            activeSections.pop(); // Remove the matched section from the stack
        }
    }

    // Update statistics for the section
    auto& statsEntry = stats[sectionName]; // Get or create stats for this section
    if (statsEntry) {
        statsEntry->count++;
        statsEntry->totalTime += elapsedTime;

        // Update min and max
        if (elapsedTime < statsEntry->minTime) {
            statsEntry->minTime = elapsedTime; // Update minimum time
        }
        if (elapsedTime > statsEntry->maxTime) {
            statsEntry->maxTime = elapsedTime; // Update maximum time
        }

        statsEntry->avgTime = statsEntry->totalTime / statsEntry->count; // Update average
    } else {
        // If stats don't exist, create a new entry
        statsEntry = new ProfilerStats(sectionName, __FILE__, __FUNCTION__, __LINE__);
        statsEntry->count = 1;
        statsEntry->totalTime = elapsedTime;
        statsEntry->minTime = elapsedTime; // Set min to the first elapsed time
        statsEntry->maxTime = elapsedTime; // Set max to the first elapsed time
        statsEntry->avgTime = elapsedTime; // Set average to the first elapsed time
    }
}

void Profiler::ExitSection(const char* sectionName, int lineNumber, const char* fileName, const char* functionName) {
    double secondsAtStop = GetCurrentTimeSeconds();
    if (startTimes.empty()) {
        std::cerr << "Error: No sections to exit." << std::endl;
        return; // Early return to avoid accessing an empty vector
    }
    TimeRecordStart const& currentSection = startTimes.back(); 

    double elapsedTime = secondsAtStop - currentSection.secondsAtStart;
    startTimes.pop_back(); // Remove the last section from the stack

       if (!activeSections.empty()) {
        const char* lastSectionName = activeSections.top().c_str(); // Get the last entered section name

        // Check if the exiting section matches the last entered section
        if (strcmp(lastSectionName, sectionName) != 0) {
            std::cerr << "Error: Exiting section " << sectionName 
                      << " does not match the last entered section " << lastSectionName << "." << std::endl;
        }

        activeSections.pop(); // Remove the last section from the stack
    }
    // Update stats
    auto& statsEntry = stats[sectionName]; // Get or create stats for this section
    if (statsEntry) {
        statsEntry->count++;
        statsEntry->totalTime += elapsedTime;

        // Update min and max
        if (elapsedTime < statsEntry->minTime) {
            statsEntry->minTime = elapsedTime; // Update minimum time
        }
        if (elapsedTime > statsEntry->maxTime) {
            statsEntry->maxTime = elapsedTime; // Update maximum time
        }
        
        statsEntry->avgTime = statsEntry->totalTime / statsEntry->count; // Update average
    } else {
        // If stats don't exist, create a new entry
        statsEntry = new ProfilerStats(sectionName, fileName, functionName, lineNumber);
        statsEntry->count = 1;
        statsEntry->totalTime = elapsedTime;
        statsEntry->minTime = elapsedTime; // Set min to the first elapsed time
        statsEntry->maxTime = elapsedTime; // Set max to the first elapsed time
        statsEntry->avgTime = elapsedTime; // Set average to the first elapsed time
    }
}


void Profiler::ReportSectionTime(char const* sectionName, double elapsedTime)
{
    elapsedTimes.emplace_back(sectionName, elapsedTime, __LINE__, __FILE__, __FUNCTION__);
}

void Profiler::ReportSectionTime(char const* sectionName, double elapsedTime, int lineNumber, const char* fileName, const char* functionName)
{
    elapsedTimes.emplace_back(sectionName, elapsedTime, lineNumber, fileName, functionName );
}

void Profiler::printStats() {
    for (auto iter = stats.begin(); iter != stats.end(); ++iter) {
        const char* sectionName = iter->first;
        ProfilerStats* stats = iter->second;

        // Calculate average time if count is not zero to avoid division by zero
        double averageSeconds = (stats->count > 0) ? (stats->totalTime / stats->count) : 0.0;

        printf("Section \"%s\" had %i calls for %.06fms; avg=%.06fms, min=%.06fms, max=%.06fms\n",
               sectionName, stats->count,
               1000.0 * stats->totalTime,  // Convert total time to milliseconds
               1000.0 * averageSeconds,
               1000.0 * stats->minTime,
               1000.0 * stats->maxTime);
    }
}

void Profiler::printStatsToCSV(const char* fileName) {
    std::ofstream file(fileName);
    if (file.is_open()) {
        file << "{\n";
        file << "  \"sections\": [\n";

        // Iterator for statistics map
        for (auto iter = stats.begin(); iter != stats.end(); ++iter) {
            ProfilerStats* s = iter->second;
            double average = (s->count > 0) ? (s->totalTime / s->count) : 0.0;

            // Write the JSON object for this section
            file << "    {\n"
                 << "      \"sectionName\": \"" << s->sectionName << "\",\n"
                 << "      \"count\": " << s->count << ",\n"
                 << "      \"totalTime\": " << (1000.0 * s->totalTime) << ",\n"
                 << "      \"minTime\": " << (1000.0 * s->minTime) << ",\n"
                 << "      \"maxTime\": " << (1000.0 * s->maxTime) << ",\n"
                 << "      \"averageTime\": " << (1000.0 * average) << ",\n" // Convert to milliseconds
                 << "      \"fileName\": \"" << (s->fileName ? s->fileName : "N/A") << "\",\n"
                 << "      \"functionName\": \"" << (s->functionName ? s->functionName : "N/A") << "\",\n"
                 << "      \"lineNumber\": " << s->lineNumber << "\n"
                 << "    }";
            
            // Check if this is the last section to avoid a trailing comma
            if (std::next(iter) != stats.end()) {
                file << ",";
            }
            file << "\n";
        }

        file << "  ]\n"; // Close the sections array
        file << "}\n";   // Close the JSON object
        file.close();    // Close the file

        std::cout << "JSON statistics successfully written to " << fileName << std::endl;
    } else {
        std::cerr << "Error: Unable to open file " << fileName << std::endl;
    }
}

void Profiler::printStatsToJSON(const char* fileName) {
    std::ofstream file(fileName);
    if (file.is_open()) {
        file << "{\n";
        file << "  \"sections\": [\n";

        // Iterator for statistics map
        for (auto iter = stats.begin(); iter != stats.end(); ++iter) {
            ProfilerStats* s = iter->second;
            double average = (s->count > 0) ? (s->totalTime / s->count) : 0.0;

            // Write the JSON object for this section
            file << "    {\n"
                 << "      \"sectionName\": \"" << s->sectionName << "\",\n"
                 << "      \"count\": " << s->count << ",\n"
                 << "      \"totalTime\": " << (1000.0 * s->totalTime) << ",\n" // Convert to milliseconds
                 << "      \"minTime\": " << (1000.0 * s->minTime) << ",\n"
                 << "      \"maxTime\": " << (1000.0 * s->maxTime) << ",\n"
                 << "      \"averageTime\": " << (1000.0 * average) << ",\n"
                 << "      \"fileName\": \"" << (s->fileName ? s->fileName : "N/A") << "\",\n"
                 << "      \"functionName\": \"" << (s->functionName ? s->functionName : "N/A") << "\",\n"
                 << "      \"lineNumber\": " << s->lineNumber << "\n"
                 << "    }";

            // Check if this is the last section to avoid a trailing comma
            if (std::next(iter) != stats.end()) {
                file << ",";
            }
            file << "\n";
        }

        file << "  ]\n"; // Close the sections array
        file << "}\n";   // Close the JSON object
        file.close();    // Close the file

        std::cout << "JSON statistics successfully written to " << fileName << std::endl;
    } else {
        std::cerr << "Error: Unable to open file " << fileName << std::endl;
    }
}
