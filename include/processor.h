#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  struct UseBreakdown
  {
      unsigned user;    
      unsigned nice;   
      unsigned system;  
      unsigned idle;      
      unsigned iowait; 
      unsigned irq;   
      unsigned softirq;  
      unsigned steal;  
      unsigned guest;  
      unsigned guestNice;
  };


  // TODO: Declare any necessary private members
 private:
  UseBreakdown prevBreakdown{0,0,0,0,0,0,0,0,0,0};
  UseBreakdown getUseBreakdown();
  bool isBreakdownZero(const Processor::UseBreakdown& breakdown);
  
};

#endif
