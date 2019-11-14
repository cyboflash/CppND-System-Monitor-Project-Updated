#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
   Basic class for Process representation
   It contains relevant attributes as shown below
   */
class Process {
    public:
        int Pid();
        std::string User();
        std::string Command();
        float CpuUtilization() const;
        std::string Ram();
        long int UpTime() const;
        bool operator<(Process const& a) const;
        void Pid(int pid);

        long LastRam() const;

    private:
        int pid;
        std::string cmd{""};
        std::string user{""};
        long ram{0};
        
};

#endif
