#include "lib.h"

#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/olsr-header.h"

#include <iostream>
#include <fstream>

using namespace ns3;

// All the nodes that we'll be using.
const uint32_t nodeCount = 5;
Ptr<Socket> sockets[nodeCount];

// UDP port to send-receive on.
const int port = 80;
// Container for nodes in network.
static NodeContainer container;
// Container for IPv4-to-node mappings.
static Ipv4InterfaceContainer ipv4container;

// Network layer tracing ------------------------------------------------------
void MacSendTrace(std::string context, Ptr<const Packet> p) {
  Ipv4Header header = GetIpv4Header(p);
  std::ostringstream log;
  int ri = PLLookupIpv4IndexInContainer(header.GetDestination(), ipv4container, nodeCount);
  int si = PLLookupIpv4IndexInContainer(header.GetSource(),      ipv4container, nodeCount);
  log << Simulator::Now().GetMicroSeconds() << ",mac-send," << si << "," << ri;
  PLLogWrite(log.str());
}
void MacRecvTrace(std::string context, Ptr<const Packet> p) {
  Ipv4Header header = GetIpv4Header(p);
  std::ostringstream log;
  int ri = PLLookupIpv4IndexInContainer(header.GetDestination(), ipv4container, nodeCount);
  int si = PLLookupIpv4IndexInContainer(header.GetSource(),      ipv4container, nodeCount);
  log << Simulator::Now().GetMicroSeconds() << ",mac-recv," << si << "," << ri;
  PLLogWrite(log.str());
}

// Physical layer tracing -----------------------------------------------------
void PhySendTrace(std::string context, Ptr<const Packet> p) {
  Ipv4Header header = GetIpv4Header(p);
  std::ostringstream log;
  int ri = PLLookupIpv4IndexInContainer(header.GetDestination(), ipv4container, nodeCount);
  int si = PLLookupIpv4IndexInContainer(header.GetSource(),      ipv4container, nodeCount);
  log << Simulator::Now().GetMicroSeconds() << ",phy-send," << si << "," << ri;
  PLLogWrite(log.str());
}
void PhyRecvTrace(std::string context, Ptr<const Packet> p) {
  Ipv4Header header = GetIpv4Header(p);
  std::ostringstream log;
  int ri = PLLookupIpv4IndexInContainer(header.GetDestination(), ipv4container, nodeCount);
  int si = PLLookupIpv4IndexInContainer(header.GetSource(),      ipv4container, nodeCount);
  log << Simulator::Now().GetMicroSeconds() << ",phy-recv," << si << "," << ri;
  PLLogWrite(log.str());
}

// Application layer tracing --------------------------------------------------
void UDPReceivePacket(Ptr<Socket> socket) {
  Ptr<Packet> packet;
  while ((packet = socket->Recv())) {
    bool found;
    SocketAddressTag tag;
    found = packet->PeekPacketTag(tag);
    
    std::ostringstream log;
    if(found) {
      Address addr;
      socket->GetSockName(addr);
      InetSocketAddress recvAddress = InetSocketAddress::ConvertFrom(addr);
      InetSocketAddress sendAddress = InetSocketAddress::ConvertFrom(tag.GetAddress());
      
      int ri = PLLookupIpv4IndexInContainer(recvAddress.GetIpv4(), ipv4container, nodeCount);
      int si = PLLookupIpv4IndexInContainer(sendAddress.GetIpv4(), ipv4container, nodeCount);
      log << Simulator::Now().GetMicroSeconds() << ",udp-recv," << si << "," << ri;
    } else {
      log << "err,Packet recieved without tags";
    }
    PLLogWrite(log.str());
  }
}
void UDPSendPacket(uint32_t size, uint32_t count, Time interval) {
  if(count > 0) {
    // Pick a random sender and destination for a packet.
    int si = PLRandInt(nodeCount);
    int di = PLRandInt(nodeCount); while(di == si) { di = PLRandInt(nodeCount); }
    
    // Create sending socket.
    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    Ptr<Socket> socket = Socket::CreateSocket(container.Get(si), tid);
    // Get address for receiver.
    Ipv4Address       destAddress       = ipv4container.GetAddress(di, 0);
    InetSocketAddress destSocketAddress = InetSocketAddress(destAddress, port);
    // Connect and send the packet.
    socket->Connect(destSocketAddress);
    socket->Send(Create<Packet>(size));
    socket->Close();
    // Log the send then wait `interval` time before sending another random packet.
    PLLogWrite(Simulator::Now().GetMicroSeconds() << ",udp-send," << si << "," << di);
    Simulator::Schedule(interval, &UDPSendPacket, size, count - 1, interval);
  } else {
    Simulator::Stop();
  }
}

int PSWifiMesh() {
  Packet::EnablePrinting();
  Packet::EnableChecking();
  Time::SetResolution(Time::NS);
  
  // CONFIGURATION ------------------------------------------------------------
  
  // Distance between nodes in meters.
  double rectWidth = 500;
  double rectDepth = 500;
  // Time between transmissions.
  double interval = 1.0; // Seconds
  Time   packetInterval = Seconds(interval);
  // Size of packets.
  uint32_t packetSize = 500; // Bytes
  // Number of packets to generate.
  uint32_t numPackets = 100;
  std::string rate = "1Mbps";
  
  // RUNNING ------------------------------------------------------------------
  
  // Holds all the network nodes.
  container.Create(nodeCount);
  // Set up overall wifi structure.
  WifiHelper wifi = PLCreateWifiHelper("DsssRate" + rate);
  // Then build the physical network.
  YansWifiPhyHelper phys = PLCreateWifiPhysicalHelper();
  NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
  mac.SetType("ns3::AdhocWifiMac");
  // Install the physical and network layer Wifi helpers.
  NetDeviceContainer devices = wifi.Install(phys, mac, container);
  
  // Use the mobility helper to set up the grid.
  MobilityHelper mobility;
    std::ostringstream sw; sw << rectWidth;
    std::ostringstream sd; sd << rectDepth;
    mobility.SetPositionAllocator(
      "ns3::RandomRectanglePositionAllocator",
      "X", StringValue("ns3::UniformRandomVariable[Min=0.0|Max="+sw.str()+"]"),
      "Y", StringValue("ns3::UniformRandomVariable[Min=0.0|Max="+sd.str()+"]")
    );
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(container);
  
  OlsrHelper olsr;
  // Don't need static routing right now.
  // Ipv4StaticRoutingHelper staticRouting;
  // Ipv4ListRoutingHelper list;
  // list.Add(staticRouting, 0);
  // list.Add(olsr, 10);
  InternetStackHelper internet;
  internet.SetRoutingHelper(olsr);
  internet.Install (container);

  Ipv4AddressHelper ipv4;
  //PLLog("Assigning addresses.");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4container = ipv4.Assign(devices);
  
  // Create receive sockets for all of the nodes in the network.
  TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
  for(uint32_t idx = 0; idx < nodeCount; idx++) {
    PLLogWrite("node," << idx);
    sockets[idx] = Socket::CreateSocket(container.Get(idx), tid);
    InetSocketAddress local = InetSocketAddress(ipv4container.GetAddress(idx, 0), port);
    sockets[idx]->Bind(local);
    sockets[idx]->SetRecvCallback(MakeCallback(&UDPReceivePacket));
  }
  for(uint32_t c1 = 0; c1 < nodeCount; c1++) {
    for(uint32_t c2 = 0; c2 < nodeCount; c2++) {
      if(c1 != c2) {
        PLLogWrite("link," << c1 << "," << c2 << "," << rate << ",,wireless,");
      }
    }
  }
  PLLogWrite(""); // Empty line between sections.
  
  // Give OLSR time to converge before starting packet sending.
  PLLogWrite(Simulator::Now().GetMicroSeconds() << ",notice,OLSR converging");
  Simulator::Schedule(Seconds(30.0), &UDPSendPacket, packetSize, numPackets, packetInterval);
  
  // Set up tracing at network layer.
  Config::Connect("/NodeList/*/DeviceList/*/Mac/MacTx", MakeCallback(&MacSendTrace));
  Config::Connect("/NodeList/*/DeviceList/*/Mac/MacRx", MakeCallback(&MacRecvTrace));
  // And also at the physical layer.
  Config::Connect("/NodeList/*/DeviceList/*/Phy/PhyTxBegin", MakeCallback(&PhySendTrace));
  Config::Connect("/NodeList/*/DeviceList/*/Phy/PhyRxBegin", MakeCallback(&PhyRecvTrace));
  
  Simulator::Stop(Seconds(120.0));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
