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

double VectorDistance(ns3::Vector v1, ns3::Vector v2) {
  double dx = v1.x - v2.x;
  double dy = v1.y - v2.y;
  double d  = sqrt((dx * dx) + (dy * dy));
  return d;
}
