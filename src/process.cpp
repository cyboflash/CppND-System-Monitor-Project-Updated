#include <unistd.h>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return pid; }

void Process::Pid(int pid) { this->pid = pid; }

float Process::CpuUtilization() const {
  // Based on https://stackoverflow.com/a/16736599
  float seconds =
      static_cast<float>(LinuxParser::UpTime() - LinuxParser::UpTime(pid)) /
      sysconf(_SC_CLK_TCK);

  return static_cast<float>(LinuxParser::ActiveJiffies(pid) /
                            sysconf(_SC_CLK_TCK)) /
         seconds;
}

string Process::Command() {
  string cmd{""};
  if (cmd.empty()) {
    cmd = LinuxParser::Command(pid);
  }
  return cmd;
}

string Process::Ram() {
  string vmsize{LinuxParser::Ram(pid)};
  ram = std::stol(vmsize);
  return vmsize;
}

long Process::LastRam() const { return ram; }

string Process::User() {
  if (user.empty()) {
    user = LinuxParser::User(pid);
  }
  return user;
}

long int Process::UpTime() const { return LinuxParser::UpTime(pid); }

bool Process::operator<(Process const& a) const {
  return LastRam() > a.LastRam();
}
