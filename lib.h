#include "ns3/core-module.h"
#include "ns3/command-line.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"

#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"

#include "ns3/wifi-module.h"

#define PLLogWrite(expr) std::cout << expr; std::cout << std::endl;

void PLLog(std::string s);

int PSWifiMesh();
ns3::Ipv4Header GetIpv4Header(ns3::Ptr<const ns3::Packet> p);
int PLRandInt(int max);

int PLLookupIpv4IndexInContainer(ns3::Ipv4Address needle, ns3::Ipv4InterfaceContainer haystack, uint32_t count);

ns3::WifiHelper PLCreateWifiHelper(std::string rate);
ns3::YansWifiPhyHelper PLCreateWifiPhysicalHelper();
