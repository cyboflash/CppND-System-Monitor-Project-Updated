#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) 
{ 
    string elapsedTime{""};

    long hours = seconds / 3600;
    seconds -= hours*3600;

    long minutes = seconds / 60;
    seconds -= minutes*60;


    if (hours > 99) 
    {
        elapsedTime += "99";
    }
    else 
    {
        if ((hours > 0) && (hours < 10))
        {
            elapsedTime += "0";
        }
        elapsedTime += std::to_string(hours);
    }
    elapsedTime += ":";

    if ((minutes > 0) && (minutes < 10))
    {
        elapsedTime += "0";
    }
    elapsedTime += std::to_string(minutes);
    elapsedTime += ":";

    if ((seconds > 0) && (seconds < 10))
    {
        elapsedTime += "0";
    }
    elapsedTime += std::to_string(seconds);

    return elapsedTime;
}
