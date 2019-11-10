#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

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

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() 
{ 
    for (auto pid : LinuxParser::Pids())
    {
        Process p;
        p.Pid(pid);
        processes_.push_back(p);
    }
    return processes_; 
}

std::string System::Kernel() 
{ 
    return LinuxParser::Kernel();
}

// TODO: Return the 
float System::MemoryUtilization() { return 0.0; }

std::string System::OperatingSystem() 
{ 
    return LinuxParser::OperatingSystem();
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return 0; }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return 0; }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return 0; }
