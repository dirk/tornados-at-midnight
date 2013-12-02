#include "lib-all.h"

// using namespace ns3;

void ListSamples(){
  using namespace std;
  cerr << "Samples:\n";
  cerr << "  wifi-mesh\n";
}

int main(int argc, char *argv[]){
  // NS_LOG_UNCOND("Scratch Simulator");
  //PLLog("Test\n");
  
  std::string sample = "";
  
  ns3::CommandLine cmd;
  cmd.Usage ("CSCI 440 Project runner.\n\nInvokes samples and scripts for the project.");
  cmd.AddValue ("sample",  "Sample to run ('list' to list all samples)", sample);
  // cmd.AddValue ("cbArg",   "a string via callback", MakeCallback (SetCbArg));
  cmd.Parse(argc, argv);
  if(sample == ""){
    cmd.PrintHelp(std::cerr);
    return 0;
  }
  if(sample == "list") {
    ListSamples();
    return 0;
  }
  if(sample == "wifi-mesh") {
    return PSWifiMesh();
  }
  
  std::cout << "Sample: " << sample << "\n";
}
