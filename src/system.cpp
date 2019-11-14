#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <fstream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() 
{ 
    return cpu_; 
}

vector<Process>& System::Processes() 
{ 
    for (auto pid : LinuxParser::Pids())
    {
        Process p;
        p.Pid(pid);
        processes_.push_back(p);
    }
    std::sort(processes_.begin(), processes_.end()); 
    return processes_;
}

std::string System::Kernel() 
{ 
    return LinuxParser::Kernel();
}

float System::MemoryUtilization() 
{ 
    float utilization = 0;
    std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kMeminfoFilename);

    if (filestream.is_open())
    {
        string discard, value;

        filestream >> discard >> value >> discard;
        float total = std::stof(value);

        filestream >> discard >> value >> discard;
        float used = std::stof(value);

        utilization = (total - used) / total;
    }

    return utilization;
}

std::string System::OperatingSystem() 
{ 
    return LinuxParser::OperatingSystem();
}

int System::RunningProcesses() 
{ 
    return 200; 
}

int System::TotalProcesses() 
{ 
    return LinuxParser::Pids().size();
}

long int System::UpTime() 
{ 
    float uptime = 0;
    std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kUptimeFilename);
    if (filestream.is_open())
    {
        filestream >> uptime;
        filestream.close();
    }

    return uptime; 
}
