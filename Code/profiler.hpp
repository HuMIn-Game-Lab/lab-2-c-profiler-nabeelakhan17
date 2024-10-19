
#pragma once

#include<cfloat>
#include<vector>
#include<map>
#include <string>
#include<fstream>
#include<stack>
#include <mutex>
#include<cstring>

#define PROFILER_EXIT(sectionName) Profiler::GetInstance()->ExitSection(sectionName, __LINE__, __FILE__, __FUNCTION__)
#define PROFILER_ENTER(sectionName) Profiler::GetInstance()->EnterSection(sectionName)


class TimeRecordStart{
    public: 
        TimeRecordStart(char const* sectionName, double secondsAtStart);
        ~TimeRecordStart();

        char const* sectionName; 
        double secondsAtStart; 


};

class TimeRecordStop{
    public: 
        TimeRecordStop(char const* sectionName, double secondsAtStop, int lineNumber, const char* fileName, const char* functionName);
        TimeRecordStop(char const* sectionName, double secondsAtStop);
        ~TimeRecordStop();

        char const* sectionName; 
        double elapsedTime; 
        double secondsAtStop; 
        int lineNumber; 
        const char* fileName; 
        const char* functionName; 

};

class ProfilerStats {
public:
    const char* sectionName;     // Name of the section
    int count;                   // Number of times the section was called
    double totalTime;            // Total time spent in the section
    double minTime;              // Minimum time taken for a call
    double maxTime;              // Maximum time taken for a call
    double avgTime;              // Average time taken per call
    const char* fileName;        // Name of the file where the section is defined
    const char* functionName;    // Name of the function where the section is defined
    int lineNumber;              // Line number where the section begins

    // Constructor that initializes all fields
    ProfilerStats(const char* name, const char* file, const char* function, int line)
        : sectionName(name), count(0), totalTime(0.0), minTime(DBL_MAX),
          maxTime(DBL_MIN), avgTime(0.0), fileName(file), functionName(function), lineNumber(line) {}

    ~ProfilerStats() {} // Destructor
};



class Profiler{
    public: 
        Profiler(); 
        ~Profiler();

        void EnterSection(char const* sectionName);
        void ExitSection(char const* sectionName);
        void ExitSection(char const* sectionName,int lineNumber, const char* fileName, const char* functionName);

        void calculateStats(); 
        void printStats();
        void printStatsToCSV(const char* fileName);
        void printStatsToJSON(const char* fileName);

        static Profiler* gProfiler;
        static Profiler* GetInstance();


    private: 
        void ReportSectionTime(char const* sectionName, double elapsedTime);
        void ReportSectionTime(char const* sectionName, double elapsedTime, int lineNumber, const char* fileName, const char* functionName );
        std::map<char const*, ProfilerStats*> stats; 
        std::vector<TimeRecordStart> startTimes; 
        std::vector<TimeRecordStop> elapsedTimes; 
        std::mutex mutex_; 
        std::stack<std::string> activeSections; // Stack to track active sections

        
};


class ProfilerScopeObject{

    public:

        ProfilerScopeObject(char const* sectionName);

        ~ProfilerScopeObject();

        char const* sectionName;

};