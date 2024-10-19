
#pragma once

#include<cfloat>
#include<vector>
#include<map>
#include <string>


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

class ProfilerStats{
    public: 
        ProfilerStats(char const* sectionName);
        ~ProfilerStats(); 

        char const* sectionName; 
        int count; 
        double totalTime; 
        double minTime; 
        double maxTime; 
        double avgTime; 
        const char* functionName; 
        int lineNumber; 
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

        static Profiler* gProfiler;
        static Profiler* GetInstance();


    private: 
        void ReportSectionTime(char const* sectionName, double elapsedTime);
        void ReportSectionTime(char const* sectionName, double elapsedTime, int lineNumber, const char* fileName, const char* functionName );
        std::map<char const*, ProfilerStats*> stats; 
        std::vector<TimeRecordStart> startTimes; 
        std::vector<TimeRecordStop> elapsedTimes; 
        
};

class ProfilerScopeObject{

    public:

        ProfilerScopeObject(char const* sectionName);

        ~ProfilerScopeObject();

        char const* sectionName;

};