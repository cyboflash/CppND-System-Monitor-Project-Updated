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

float Process::CpuUtilization() const
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

    long int utime = 0,
             stime = 0,
             cutime = 0,
             cstime = 0,
             starttime = 0;
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
                default: /* do nothing */     break;
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
    float seconds = uptime - (static_cast<float>(starttime) / sysconf(_SC_CLK_TCK));

    //     Finally we calculate the CPU usage percentage:
    //     cpu_usage = 100 * ((total_time / Hertz) / seconds)
    return 100*static_cast<float>(totalTime / sysconf(_SC_CLK_TCK)) / seconds; 
}

string Process::Command()
{ 
    string cmd{""};
    if (cmd.empty())
    {
        cmd = LinuxParser::Command(pid);
    }
    return cmd;
}

string Process::Ram()
{ 
    string vmsize{LinuxParser::Ram(pid)};
    ram = std::stol(vmsize);
    return vmsize;
}

long Process::LastRam() const
{
    return ram;
}

string Process::User()
{ 
    if (user.empty())
    {
        user = LinuxParser::User(pid);
    }
    return user;
}

long int Process::UpTime() const
{
    return LinuxParser::UpTime(pid);
}

bool Process::operator<(Process const& a) const 
{ 
    return LastRam() > a.LastRam(); 
}
