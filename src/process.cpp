#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() 
{ 
    return pid; 
}

void Process::Pid(int pid) 
{ 
    this->pid = pid;
}

float Process::CpuUtilization() 
{ 
    float uptime = 0;
    std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kUptimeFilename);
    if (filestream.is_open())
    {
        string s;
        filestream >> s;
        filestream.close();
        uptime = std::stof(s);
    }

    long int utime = 0, stime = 0, cutime = 0, cstime = 0, starttime = 0;
    filestream.open(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
    if (filestream.is_open())
    {
        // #14 utime - CPU time spent in user code, measured in clock ticks
        // #15 stime - CPU time spent in kernel code, measured in clock ticks
        // #16 cutime - Waited-for children's CPU time spent in user code (in clock ticks)
        // #17 cstime - Waited-for children's CPU time spent in kernel code (in clock ticks)
        // #22 starttime - Time when the process started, measured in clock ticks
        string s{""};
        for (unsigned i = 1; i <= 22; i++)
        {
            filestream >> s;
            switch(i)
            {
                case 14: utime     = stol(s); break;
                case 15: stime     = stol(s); break;
                case 16: cutime    = stol(s); break;
                case 17: cstime    = stol(s); break;
                case 22: starttime = stol(s); break;
                default: break;
            }
        }
    }

    // Calculation
    //     First we determine the total time spent for the process:
    //     
    //     total_time = utime + stime
    long int totalTime = utime + stime;

    // We also have to decide whether we want to include the time from children processes. If we do, then we add those values to total_time:
    //     total_time = total_time + cutime + cstime
    totalTime += cutime + cstime;

    //     Next we get the total elapsed time in seconds since the process started:
    //     seconds = uptime - (starttime / Hertz)
    float seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));

    //     Finally we calculate the CPU usage percentage:
    //     cpu_usage = 100 * ((total_time / Hertz) / seconds)
    
    return 100 * ((totalTime / sysconf(_SC_CLK_TCK)) / seconds); 
}

string Process::Command() 
{ 
    string cmd{""};
    std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kCmdlineFilename);
    if (filestream.is_open())
    {
        filestream >> cmd;
        filestream.close();
    }
    return cmd;
}

string Process::Ram() 
{ 
    string vmsize{""};
    std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatusFilename);

    if (filestream.is_open())
    {
        string line, key;
        while(std::getline(filestream, line))
        {
            std::istringstream linestream(line);
            linestream >> key >> vmsize;
            if ("VmSize:" == key)
            {
                filestream.close();
                vmsize = to_string(std::stof(vmsize)/1000);
                break;
            }
        }
    }

    return vmsize;
}

string Process::User() 
{ 
    string user{""};
    std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatusFilename);

    if (filestream.is_open())
    {
        filestream >> user >> user;
        filestream.close();
    }
    return user;
}

long int Process::UpTime() 
{ 
    long int uptime = 0;
    std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
    if (filestream.is_open())
    {
        string s{""};
        // 22nd item in a line is uptime in clock ticks
        for (unsigned i = 0; i < 22; i++)
            filestream >> s;

        filestream.close();

        // convert uptime from clock ticks to seconds
        uptime = (std::stol(s))/sysconf(_SC_CLK_TCK);
    }
    return uptime; 
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }
