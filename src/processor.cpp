#include <fstream>

#include "linux_parser.h"
#include "processor.h"

float Processor::Utilization() {
  return std::stof(LinuxParser::CpuUtilization()[0]);
}
