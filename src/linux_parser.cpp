#include <dirent.h>
#include <unistd.h>
#include <fstream>
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

float LinuxParser::MemoryUtilization() {
  float utilization = 0;
  std::ifstream filestream(LinuxParser::kProcDirectory +
                           LinuxParser::kMeminfoFilename);

  if (filestream.is_open()) {
    string discard, value;

    filestream >> discard >> value >> discard;
    float total = std::stof(value);

    filestream >> discard >> value >> discard;
    float used = std::stof(value);

    utilization = (total - used) / total;
  }

  return utilization;
}

long LinuxParser::UpTime() {
  float uptime = 0;
  std::ifstream filestream(LinuxParser::kProcDirectory +
                           LinuxParser::kUptimeFilename);
  if (filestream.is_open()) {
    filestream >> uptime;
    filestream.close();
  }
  return uptime;
}

long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

long LinuxParser::ActiveJiffies(int pid) {
  // Based on https://stackoverflow.com/a/16736599
  long int utime = 0, stime = 0, cutime = 0, cstime = 0;

  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) +
                           LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    // #14 utime - CPU time spent in user code, measured in clock ticks
    // #15 stime - CPU time spent in kernel code, measured in clock ticks
    // #16 cutime - Waited-for children's CPU time spent in user code (in clock
    // ticks) #17 cstime - Waited-for children's CPU time spent in kernel code
    // (in clock ticks)
    string s{""};
    for (unsigned i = 1; i <= 22; i++) {
      filestream >> s;
      switch (i) {
        case 14:
          utime = stol(s);
          break;
        case 15:
          stime = stol(s);
          break;
        case 16:
          cutime = stol(s);
          break;
        case 17:
          cstime = stol(s);
          break;
        default: /* do nothing */
          break;
      }
    }
  }

  // Calculation
  //     First we determine the total time spent for the process:
  //
  //     total_time = utime + stime
  long int totalTime = utime + stime;

  // We also have to decide whether we want to include the time from children
  // processes. If we do, then we add those values to total_time:
  //     total_time = total_time + cutime + cstime
  totalTime += cutime + cstime;

  return totalTime;
}

long LinuxParser::ActiveJiffies() {
  std::string discard, user{"0"}, nice{"0"}, system{"0"}, irq{"0"},
      softirq{"0"}, steal{"0"};
  std::ifstream filestream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    filestream >> discard;
    filestream >> user;
    filestream >> nice;
    filestream >> system;
    filestream >> discard;
    filestream >> discard;
    filestream >> irq;
    filestream >> softirq;
    filestream >> steal;
  }

  return std::stol(user) + std::stol(nice) + std::stol(system) +
         std::stol(irq) + std::stol(softirq) + std::stol(steal);
}

long LinuxParser::IdleJiffies() {
  std::string idle{"0"}, iowait{"0"}, discard;
  std::ifstream filestream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);

  if (filestream.is_open()) {
    filestream >> discard;
    filestream >> discard;
    filestream >> discard;
    filestream >> discard;
    filestream >> idle;
    filestream >> iowait;
    filestream.close();
  }

  return std::stol(idle) + std::stol(iowait);
}

vector<string> LinuxParser::CpuUtilization() {
  static long prevIdle{0}, prevTotal{0};
  long idle = LinuxParser::IdleJiffies();

  long nonIdle = LinuxParser::ActiveJiffies();

  // Total = Idle + NonIdle
  long total = idle + nonIdle;

  // differentiate: actual value minus the previous one
  // totald = Total - PrevTotal
  // idled = Idle - PrevIdle
  long totald = total - prevTotal;
  long idled = idle - prevIdle;

  // Remember current values for next calculation
  prevIdle = idle;
  prevTotal = total;

  // CPU_Percentage = (totald - idled)/totald
  float percentage = (static_cast<float>(totald) - static_cast<float>(idled)) /
                     static_cast<float>(totald);
  return vector<string>{std::to_string(percentage)};
}

int LinuxParser::TotalProcesses() {
  string processes{"0"};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string line{""};
    for (auto i = 0; i < 6; i++) std::getline(filestream, line);

    std::istringstream linestream(line);
    linestream >> processes >> processes;
    filestream.close();
  }
  return std::stoi(processes);
}

int LinuxParser::RunningProcesses() {
  string processes{"0"};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string line{""};
    for (auto i = 0; i < 7; i++) std::getline(filestream, line);

    std::istringstream linestream(line);
    linestream >> processes >> processes;
    filestream.close();
  }
  return std::stoi(processes);
}

string LinuxParser::Command(int pid) {
  string cmd{""};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    filestream >> cmd;
    filestream.close();
  }
  return cmd;
}

string LinuxParser::Ram(int pid) {
  string vmsize{""};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  if (filestream.is_open()) {
    string line, key;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> vmsize;
      if ("VmSize:" == key) {
        filestream.close();
        vmsize = to_string(std::stoi(vmsize) / 1000);
        break;
      }
    }
  }
  return vmsize;
}

string LinuxParser::Uid(int pid) {
  string uid{""};

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    string line{""};
    for (auto i = 0; i < 9; i++) std::getline(filestream, line);

    std::istringstream linestream(line);

    linestream >> uid >> uid;
    filestream.close();
  }

  return uid;
}

string LinuxParser::User(int pid) {
  string user{""};
  string uid{LinuxParser::Uid(pid)};
  std::ifstream filestream(kPasswordPath);

  if (filestream.is_open()) {
    string line;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string _user, _uid;
      std::getline(linestream, _user, ':');
      std::getline(linestream, _uid, ':');
      std::getline(linestream, _uid, ':');

      if (uid == _uid) {
        user = _user;
        break;
      }
    }
  }
  return user;
}

long LinuxParser::UpTime(int pid) {
  long uptime = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    string s{""};
    // 22nd item in a line is uptime in clock ticks
    for (unsigned i = 0; i < 22; i++) filestream >> s;

    filestream.close();

    uptime = std::stol(s);
  }
  return uptime;
}
