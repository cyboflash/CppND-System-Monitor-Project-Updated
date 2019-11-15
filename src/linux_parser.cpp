#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() 
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

long LinuxParser::UpTime() 
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

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

int LinuxParser::TotalProcesses() 
{ 
    string processes{"0"};
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open())
    {
        string line{""};
        for (auto i = 0; i < 6; i++)
            std::getline(filestream, line);

        std::istringstream linestream(line);
        linestream >> processes >> processes;
        filestream.close();
    }
    return std::stoi(processes); 
}

int LinuxParser::RunningProcesses() 
{ 
    string processes{"0"};
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open())
    {
        string line{""};
        for (auto i = 0; i < 7; i++)
            std::getline(filestream, line);

        std::istringstream linestream(line);
        linestream >> processes >> processes;
        filestream.close();
    }
    return std::stoi(processes); 
}

string LinuxParser::Command(int pid) 
{ 
    string cmd{""};
    std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
    if (filestream.is_open())
    {
        filestream >> cmd;
        filestream.close();
    }
    return cmd; 
}

string LinuxParser::Ram(int pid) 
{ 
    string vmsize{""};
    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

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
                vmsize = to_string(std::stoi(vmsize)/1000);
                break;
            }
        }
    }
    return vmsize; 
}

string LinuxParser::Uid(int pid) 
{ 
    string uid{""};

    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
    if (filestream.is_open())
    {
        string line{""};
        for (auto i = 0; i < 9; i++)
            std::getline(filestream, line);

        std::istringstream linestream(line);

        linestream >> uid >> uid;
        filestream.close();
    }

    return uid; 
}

string LinuxParser::User(int pid) 
{ 
    string user{""};
    string uid{LinuxParser::Uid(pid)};
    std::ifstream filestream(kPasswordPath);

    if (filestream.is_open())
    {
        string line;
        while(std::getline(filestream, line))
        {
            std::istringstream linestream(line);
            string _user, _uid;
            std::getline(linestream, _user, ':');
            std::getline(linestream, _uid, ':');
            std::getline(linestream, _uid, ':');

            if (uid == _uid)
            {
                user = _user;
                break;
            }
        }
    }
    return user; 
}

long LinuxParser::UpTime(int pid) 
{ 
    long int uptime = 0;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
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
