#include "lib.h"

int PLRandInt(int max) {
  return (rand() % (int)(max));
}

int PLLookupIpv4IndexInContainer(ns3::Ipv4Address needle, ns3::Ipv4InterfaceContainer haystack, uint32_t count) {
  for(uint32_t idx = 0; idx < count; idx++) {
    ns3::Ipv4Address ip = haystack.GetAddress(idx, 0);
    if(ip.IsEqual(needle)) { return int(idx); }
  }
  return -1;
}
