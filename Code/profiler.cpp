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
ProfilerStats::~ProfilerStats()
{
    // Destructor implementation (if needed, otherwise it can be empty)
}
ProfilerScopeObject::ProfilerScopeObject(char const* sectionName){

    Profiler::GetInstance()->EnterSection(sectionName);

}

ProfilerScopeObject::~ProfilerScopeObject(){

    Profiler::GetInstance()->ExitSection(sectionName);

}
void Profiler::EnterSection(char const* sectionName)
{
    double secondsAtStart = GetCurrentTimeSeconds(); 
    startTimes.emplace_back(sectionName, secondsAtStart);

}

void Profiler :: ExitSection(char const* sectionName)
{
    double secondsAtStop = GetCurrentTimeSeconds();
    if (startTimes.empty()) {
        std::cerr << "Error: No sections to exit." << std::endl;
        return; // Early return to avoid accessing an empty vector
    }
    TimeRecordStart const& currentSection = startTimes.back(); 

    // #if defined (DEBUG PROFILER)
    //     //verify the stack isnt empty 


    //     //verify the currentSection matches the sectionName
    // #endif
    double elapsedTime = secondsAtStop - currentSection.secondsAtStart;
    ReportSectionTime(sectionName, elapsedTime);
    //elapsedTimes.emplace_back(sectionName, elapsedTime);
    startTimes.pop_back();
}
void Profiler::ExitSection(char const* sectionName, int lineNumber, const char* fileName, const char* functionName)
{
    double secondsAtStop = GetCurrentTimeSeconds();
    const TimeRecordStart& currentSection = startTimes.back();

    double elapsedTime = secondsAtStop - currentSection.secondsAtStart;
    ReportSectionTime(sectionName, elapsedTime, lineNumber, fileName, functionName);
    
    startTimes.pop_back();
}

void Profiler::ReportSectionTime(char const* sectionName, double elapsedTime)
{
    elapsedTimes.emplace_back(sectionName, elapsedTime, __LINE__, __FILE__, __FUNCTION__);
}

void Profiler::ReportSectionTime(char const* sectionName, double elapsedTime, int lineNumber, const char* fileName, const char* functionName)
{
    elapsedTimes.emplace_back(sectionName, elapsedTime, lineNumber, fileName, functionName );
}