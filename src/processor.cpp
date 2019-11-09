#include <fstream>

#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() 
{ 
    Processor::UseBreakdown breakdown = getUseBreakdown();

    // PrevIdle = previdle + previowait
    // Idle = idle + iowait
    unsigned prevIdle = prevBreakdown.idle + prevBreakdown.iowait;
    unsigned idle = breakdown.idle + breakdown.iowait;

    // PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
    // NonIdle = user + nice + system + irq + softirq + steal
    unsigned prevNonIdle = prevBreakdown.user + prevBreakdown.nice + prevBreakdown.system +
                           prevBreakdown.irq + prevBreakdown.softirq + prevBreakdown.steal;
    unsigned nonIdle = breakdown.user + breakdown.nice + breakdown.system + breakdown.irq + 
                       breakdown.softirq + breakdown.steal;


    // PrevTotal = PrevIdle + PrevNonIdle
    // Total = Idle + NonIdle
    unsigned prevTotal = prevIdle + prevNonIdle;
    unsigned total = idle + nonIdle;

    // differentiate: actual value minus the previous one
    // totald = Total - PrevTotal
    // idled = Idle - PrevIdle
    unsigned totald = total - prevTotal;
    unsigned idled = idle - prevIdle;

    // Remember current breakdown for next calculation
    prevBreakdown = breakdown;

    // CPU_Percentage = (totald - idled)/totald
    return (static_cast<float>(totald) - static_cast<float>(idled))/static_cast<float>(totald); 
}

bool Processor::isBreakdownZero(const Processor::UseBreakdown& breakdown)
{
    return (0 == breakdown.user)    &&
           (0 == breakdown.nice)    &&
           (0 == breakdown.system)  &&
           (0 == breakdown.idle)    &&
           (0 == breakdown.iowait)  &&
           (0 == breakdown.irq)     &&
           (0 == breakdown.softirq) &&
           (0 == breakdown.steal)   &&
           (0 == breakdown.guest)   &&
           (0 == breakdown.guestNice);

}

Processor::UseBreakdown Processor::getUseBreakdown()
{
    Processor::UseBreakdown breakdown{0,0,0,0,0,0,0,0,0,0};

    std::string line,
                discard;
    std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> discard;
        linestream >> breakdown.user;
        linestream >> breakdown.nice;
        linestream >> breakdown.system;
        linestream >> breakdown.idle;
        linestream >> breakdown.iowait;
        linestream >> breakdown.irq;
        linestream >> breakdown.softirq;
        linestream >> breakdown.steal;
        linestream >> breakdown.guest;
        linestream >> breakdown.guestNice;
    }
    return breakdown;
}
