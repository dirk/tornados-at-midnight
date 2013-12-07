#include "ns3/core-module.h"
#include "ns3/command-line.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"

#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"

#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"

#define PLLogWrite(expr) std::cout << expr; std::cout << std::endl;

// Speed of light through air (The Speed of Magic).
#define C 299704644.53915

void PLLog(std::string s);

int PSWifiMesh();
ns3::Ipv4Header GetIpv4Header(ns3::Ptr<const ns3::Packet> p);

// lib-util
int PLRandInt(int max);
int PLLookupIpv4IndexInContainer(ns3::Ipv4Address needle, ns3::Ipv4InterfaceContainer haystack, uint32_t count);
double VectorDistance(ns3::Vector v1, ns3::Vector v2);

// lib-ns3
ns3::WifiHelper PLCreateWifiHelper(std::string rate);
ns3::YansWifiPhyHelper PLCreateWifiPhysicalHelper();
ns3::Vector GetNodePosition(ns3::Ptr<ns3::Node> node, ns3::NodeContainer container);
