#include "lib.h"

// Return random int (instead of double/float) between 0 and max.
int PLRandInt(int max) {
  return (rand() % (int)(max));
}

// Give a needle, haystack, and size of the haystack, find the index of
// the needle in the `Ipv4InterfaceContainer` haystack.
int PLLookupIpv4IndexInContainer(ns3::Ipv4Address needle, ns3::Ipv4InterfaceContainer haystack, uint32_t count) {
  for(uint32_t idx = 0; idx < count; idx++) {
    ns3::Ipv4Address ip = haystack.GetAddress(idx, 0);
    if(ip.IsEqual(needle)) { return int(idx); }
  }
  return -1;
}

// Compute the distance between two vectors.
double VectorDistance(ns3::Vector v1, ns3::Vector v2) {
  double dx = v1.x - v2.x;
  double dy = v1.y - v2.y;
  double d  = sqrt((dx * dx) + (dy * dy));
  return d;
}
