 #include "profiler.hpp"

#include <iostream>

#include "time.hpp"


Profiler* Profiler::gProfiler = nullptr;

TimeRecordStart::TimeRecordStart(char const* sectionName, double secondsAtStart) : sectionName(sectionName), secondsAtStart(secondsAtStart){}

TimeRecordStop::TimeRecordStop(char const* sectionName, double elapsedTime, int lineNumber, const char* fileName, const char* functionName)
    : sectionName(sectionName), elapsedTime(elapsedTime), lineNumber(lineNumber), fileName(fileName), functionName(functionName) {}

TimeRecordStop::TimeRecordStop(char const* sectionName, double elapsedTime)
    : sectionName(sectionName), elapsedTime(elapsedTime), lineNumber(0), fileName(nullptr), functionName(nullptr) {}

TimeRecordStart::~TimeRecordStart() {}

TimeRecordStop::~TimeRecordStop() {}

Profiler::Profiler() {
    gProfiler = this;
    startTimes.reserve(100);
    elapsedTimes.reserve(1000000);
}
Profiler* Profiler::GetInstance() {
    if (gProfiler == nullptr) {
        gProfiler = new Profiler();
    }

    return gProfiler;
}
Profiler::~Profiler() {
    // for(auto& start: startTimes)
    // {
    //     delete start;
    // }
    // for(auto& elapsed: elapsedTimes)
    // {
    //     delete elapsed;
    // }
    for (auto& stat : stats) {
        delete stat.second;
    }
}

ProfilerScopeObject::ProfilerScopeObject(char const* sectionName) {
    Profiler::GetInstance()->EnterSection(sectionName);
}

ProfilerScopeObject::~ProfilerScopeObject() {
    Profiler::GetInstance()->ExitSection(sectionName);
}
void Profiler::EnterSection(char const* sectionName) {
    std::lock_guard<std::mutex> lock(mutex_);  // Lock the mutex for thread safety


    activeSections.push(sectionName);

    double secondsAtStart = GetCurrentTimeSeconds();

    TimeRecordStart start = TimeRecordStart(sectionName, secondsAtStart);

    startTimes.emplace_back(sectionName, secondsAtStart);
}

void Profiler::ExitSection(const char* sectionName) {
    std::lock_guard<std::mutex> lock(mutex_);  // Lock for thread safety

    double secondsAtStop = GetCurrentTimeSeconds();  // Get the current time
    bool found = false;

    // Iterate through startTimes to find the matching section
    for (auto it = startTimes.rbegin(); it != startTimes.rend(); ++it) {
        if (std::string(it->sectionName) == sectionName) {
            double elapsedTime = secondsAtStop - it->secondsAtStart;

            elapsedTimes.emplace_back(sectionName, elapsedTime);
            startTimes.erase((it + 1).base()); // Erase the found element
            found = true;
            activeSections.pop();

            break;
        }
    }

    if (!found) {
        std::cerr << "Warning: Attempted to exit a section '" << sectionName
                  << "' that was not found or already exited." << std::endl;
    }
    
}

// Update statistics for the section
// auto& statsEntry = stats[sectionName]; // Get or create stats for this section
// if (statsEntry) {
//     statsEntry->count++;
//     statsEntry->totalTime += elapsedTime;

//     // Update min and max
//     if (elapsedTime < statsEntry->minTime) {
//         statsEntry->minTime = elapsedTime; // Update minimum time
//     }
//     if (elapsedTime > statsEntry->maxTime) {
//         statsEntry->maxTime = elapsedTime; // Update maximum time
//     }

//     statsEntry->avgTime = statsEntry->totalTime / statsEntry->count; // Update average
// } else {
//     // If stats don't exist, create a new entry
//     statsEntry = new ProfilerStats(sectionName, __FILE__, __FUNCTION__, __LINE__);
//     statsEntry->count = 1;
//     statsEntry->totalTime = elapsedTime;
//     statsEntry->minTime = elapsedTime; // Set min to the first elapsed time
//     statsEntry->maxTime = elapsedTime; // Set max to the first elapsed time
//     statsEntry->avgTime = elapsedTime; // Set average to the first elapsed time
// }

// void Profiler::ExitSection(const char* sectionName) {
//     std::lock_guard<std::mutex> lock(mutex_);
//     double secondsAtStop = GetCurrentTimeSeconds();

//     // Ensure startTimes is not empty
//     if (startTimes.empty()) {
//         std::cerr << "Error: No sections to exit." << std::endl;
//         return; // Early return to avoid accessing an empty vector
//     }

//     // Retrieve the last entered section
//     TimeRecordStart const& currentSection = startTimes.back();

//     // Section name checking
//     if (strcmp(currentSection.sectionName, sectionName) != 0) {
//         std::cerr << "Warning: Exiting section '" << sectionName
//                   << "' does not match the last entered section '"
//                   << currentSection.sectionName << "'." << std::endl;
//     }

//     // Calculate elapsed time
//     double elapsedTime = secondsAtStop - currentSection.secondsAtStart;
//     ReportSectionTime(currentSection.sectionName, elapsedTime);

//     // Remove the last entered section
//     startTimes.pop_back();
// }

// void Profiler::ExitSection(const char* sectionName, int lineNumber, const char* fileName, const char* functionName) {
//     double secondsAtStop = GetCurrentTimeSeconds();
//     if (startTimes.empty()) {
//         std::cerr << "Error: No sections to exit." << std::endl;
//         return; // Early return to avoid accessing an empty vector
//     }
//     TimeRecordStart const& currentSection = startTimes.back();

//     double elapsedTime = secondsAtStop - currentSection.secondsAtStart;
//     startTimes.pop_back(); // Remove the last section from the stack

//     //    if (!activeSections.empty()) {
//     //     const char* lastSectionName = activeSections.top().c_str(); // Get the last entered section name

//     //     // Check if the exiting section matches the last entered section
//     //     if (strcmp(lastSectionName, sectionName) != 0) {
//     //         std::cerr << "Error: Exiting section " << sectionName
//     //                   << " does not match the last entered section " << lastSectionName << "." << std::endl;
//     //     }

//     //     activeSections.pop(); // Remove the last section from the stack
//     // }
//     // // Update stats
//     // auto& statsEntry = stats[sectionName]; // Get or create stats for this section
//     // if (statsEntry) {
//     //     statsEntry->count++;
//     //     statsEntry->totalTime += elapsedTime;

//     //     // Update min and max
//     //     if (elapsedTime < statsEntry->minTime) {
//     //         statsEntry->minTime = elapsedTime; // Update minimum time
//     //     }
//     //     if (elapsedTime > statsEntry->maxTime) {
//     //         statsEntry->maxTime = elapsedTime; // Update maximum time
//     //     }

//     //     statsEntry->avgTime = statsEntry->totalTime / statsEntry->count; // Update average
//     // } else {
//     //     // If stats don't exist, create a new entry
//     //     statsEntry = new ProfilerStats(sectionName, fileName, functionName, lineNumber);
//     //     statsEntry->count = 1;
//     //     statsEntry->totalTime = elapsedTime;
//     //     statsEntry->minTime = elapsedTime; // Set min to the first elapsed time
//     //     statsEntry->maxTime = elapsedTime; // Set max to the first elapsed time
//     //     statsEntry->avgTime = elapsedTime; // Set average to the first elapsed time
//     // }
// }

void Profiler::ExitSection(const char* sectionName, int lineNumber, const char* fileName, const char* functionName) {
    std::lock_guard<std::mutex> lock(mutex_);  // Ensure thread safety

     double secondsAtStop = GetCurrentTimeSeconds();
    bool found = false;

    // Iterate through startTimes to find the matching section
    for (auto it = startTimes.rbegin(); it != startTimes.rend(); ++it) {
        if (std::string(it->sectionName) == sectionName) {
            double elapsedTime = secondsAtStop - it->secondsAtStart;

            ReportSectionTime(sectionName, elapsedTime,  lineNumber, fileName, functionName);
            startTimes.erase((it + 1).base()); // Erase the found element
            found = true;
            activeSections.pop();

            break;
        }
    }
}

void Profiler::ReportSectionTime(char const* sectionName, double elapsedTime) {
    elapsedTimes.emplace_back(sectionName,elapsedTime, __LINE__, __FILE__, __FUNCTION__);
}

// profiler.cpp

void Profiler::ReportSectionTime(const char* sectionName, double elapsedTime,  int lineNumber, const char* fileName, const char* functionName) {
    elapsedTimes.emplace_back(sectionName, elapsedTime,  lineNumber, fileName, functionName);
}


void Profiler::calculateStats() {
    for (auto const& record : elapsedTimes) {
        // Calculate stats for each section
        if (stats.find(record.sectionName) == stats.end()) {
            stats[record.sectionName] = new ProfilerStats(
                record.sectionName,
                record.fileName ? record.fileName : "N/A",
                record.functionName ? record.functionName : "N/A",
                record.lineNumber);
        }
        ProfilerStats* sectionStats = stats[record.sectionName];
        sectionStats->count++;
        sectionStats->totalTime += record.elapsedTime;  // Use elapsedTime instead of secondsAtStop
        sectionStats->avgTime = sectionStats->totalTime / static_cast<double>(sectionStats->count);

        if (record.elapsedTime < sectionStats->minTime) {
            sectionStats->minTime = record.elapsedTime;
        }
        if (record.elapsedTime > sectionStats->maxTime) {
            sectionStats->maxTime = record.elapsedTime;
        }

       

        // Update file and function information
        sectionStats->fileName = record.fileName ? record.fileName : "N/A";
        sectionStats->functionName = record.functionName ? record.functionName : "N/A";
        sectionStats->lineNumber = record.lineNumber;
    }
}

void Profiler::printStats() {
    std::cout<< "Profiler Stats"<<std::endl;
    std::cout<< "--------------------------------"<<std::endl;
    for(auto const& pair : stats){
        ProfilerStats* sectionStats = pair.second;
        std::cout<< "Section Name: "<< sectionStats->sectionName<<std::endl;
        std::cout<< "Count: "<< sectionStats->count<<std::endl;
        std::cout<< "Total Time: "<< (sectionStats->totalTime)*1000<<std::endl;
        std::cout<< "Average Time: "<< (sectionStats->avgTime)*1000<<std::endl;
        std::cout<< "Min Time: "<< (sectionStats->minTime)*1000<<std::endl;
        std::cout<< "Max Time: "<< (sectionStats->maxTime)*1000<<std::endl;
        std::cout<< "Line Number: "<< sectionStats->lineNumber<<std::endl;
        std::cout<< "File Name: "<< sectionStats->fileName<<std::endl;
        std::cout<< "Function Name: "<< sectionStats->functionName<<std::endl;

        std::cout<< "--------------------------------"<<std::endl;
    }
}

#include <iomanip> // Ensure this is included for setting precision

void Profiler::printStatsToCSV(const char* fileName) {
    std::ofstream file(fileName);
    if (file.is_open()) {
        // Set fixed-point notation and precision for floating-point numbers
        file << std::fixed << std::setprecision(6);

        // Write CSV headers
        file << "sectionName,count,totalTime(ms),minTime(ms),maxTime(ms),averageTime(ms),fileName,functionName,lineNumber\n";

        // Iterate over the stats map and write each entry as a CSV row
        for (const auto& pair : stats) {
            ProfilerStats* s = pair.second;
            double average = (s->count > 0) ? (s->totalTime / s->count) : 0.0;

            // Write each field, enclosing strings in quotes and separating with commas
            file << "\"" << s->sectionName << "\","         // Section name
                 << s->count << ","                          // Count
                 << (1000.0 * s->totalTime) << ","           // Total time in ms
                 << (1000.0 * s->minTime) << ","             // Min time in ms
                 << (1000.0 * s->maxTime) << ","             // Max time in ms
                 << (1000.0 * average) << ","                // Average time in ms
                 << "\"" << (s->fileName ? s->fileName : "N/A") << "\","  // File name
                 << "\"" << (s->functionName ? s->functionName : "N/A") << "\","  // Function name
                 << s->lineNumber << "\n";                     
        }

        file.close();
        std::cout << "CSV statistics successfully written to " << fileName << std::endl;
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
                 << "      \"totalTime\": " << (1000.0 * s->totalTime) << ",\n"  // Convert to milliseconds
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

        file << "  ]\n";  // Close the sections array
        file << "}\n";    // Close the JSON object
        file.close();     // Close the file

        std::cout << "JSON statistics successfully written to " << fileName << std::endl;
    } else {
        std::cerr << "Error: Unable to open file " << fileName << std::endl;
    }
}

// In profiler.cpp

// Print Detailed Statistics to CSV
void Profiler::printStartAndStopToCSV(const char* fileName) {
    std::ofstream file(fileName);
    if (file.is_open()) {
        // Set fixed-point notation and precision for floating-point numbers
        file << std::fixed << std::setprecision(6);

        // Write CSV headers
        file << "sectionName,elapsedTime(ms),lineNumber,fileName,functionName\n";

        // Iterate over the elapsedTimes and write each entry
        for (const auto& record : elapsedTimes) {
            file << "\"" << record.sectionName << "\","
                 << (record.elapsedTime * 1000.0) << ","
                 << record.lineNumber << ","
                 << "\"" << (record.fileName ? record.fileName : "N/A") << "\","
                 << "\"" << (record.functionName ? record.functionName : "N/A") << "\"\n";
        }

        file.close();
        std::cout << "Detailed CSV statistics successfully written to " << fileName << std::endl;
    } else {
        std::cerr << "Error: Unable to open file " << fileName << std::endl;
    }
}

// void Profiler:: printAllRuns(const char* fileName)
// {
//     std::ofstream file(fileName);
//     if (file.is_open()) {
//         // Set fixed-point notation and precision for floating-point numbers
//         file << std::fixed << std::setprecision(6);

//         // Write CSV headers
//         file << "sectionName,elapsedTime(ms)\n";

//         // Iterate over the elapsedTimes and write each entry
//         for (const auto& record : elapsedTimes) {
//             file << "\"" << record.sectionName << "\","
//                  << (record.elapsedTime * 1000.0) <<"\"\n";
//         }

//         file.close();
//         std::cout << "Detailed CSV statistics successfully written to " << fileName << std::endl;
//     } else {
//         std::cerr << "Error: Unable to open file " << fileName << std::endl;
//     }
// }

// // 