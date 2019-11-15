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
    return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() 
{ 
    return LinuxParser::OperatingSystem();
}

int System::RunningProcesses() 
{ 
    return LinuxParser::RunningProcesses();
}

int System::TotalProcesses() 
{ 
    return LinuxParser::TotalProcesses();
}

long int System::UpTime() 
{ 
    return LinuxParser::UpTime();
}
