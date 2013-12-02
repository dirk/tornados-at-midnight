#include "lib-all.h"

#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"

#include <iostream>
#include <fstream>

using namespace ns3;

int my_rand(int max) {
  return (rand() % (int)(max));
}

// All the nodes that we'll be using.
const uint32_t nodeCount = 5;
Ptr<Socket> sockets[nodeCount];

const int port = 80;
static NodeContainer container;
static Ipv4InterfaceContainer ipv4container;

int lookupIpv4Index(Ipv4Address needle) {
  for(uint32_t idx = 0; idx < nodeCount; idx++) {
    Ipv4Address ip = ipv4container.GetAddress(idx, 0);
    if(ip.IsEqual(needle)) { return int(idx); }
  }
  return -1;
}

void ReceivePacket(Ptr<Socket> socket) {
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
      int ri = lookupIpv4Index(recvAddress.GetIpv4());
      int si = lookupIpv4Index(sendAddress.GetIpv4());
      log << Simulator::Now().GetSeconds() << ",recv," << si << "," << ri;
    } else {
      log << "err,Packet recieved without tags";
    }
    PLLogWrite(log.str());
  }
}

void SendPacket(uint32_t size, uint32_t count, Time interval) {
  if(count > 0) {
    // Pick a random sender and destination for a packet.
    int si = my_rand(nodeCount);
    int di = my_rand(nodeCount); while(di == si) { di = my_rand(nodeCount); }
    
    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    Ptr<Socket> socket = Socket::CreateSocket(container.Get(si), tid);
    
    Ipv4Address       destAddress       = ipv4container.GetAddress(di, 0);
    InetSocketAddress destSocketAddress = InetSocketAddress(destAddress, port);
    
    socket->Connect(destSocketAddress);
    socket->Send(Create<Packet>(size));
    socket->Close();
    
    PLLogWrite(Simulator::Now().GetSeconds() << ",send," << si << "," << di);
    Simulator::Schedule(interval, &SendPacket, size, count - 1, interval);
  } else {
    Simulator::Stop();
  }
}

int PSWifiMesh() {
  // Packet::EnablePrinting();
  
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
  // Nodes for sending and receiving.
  //uint32_t recvNode   = 0;
  //uint32_t sendNode = 4;
  // What rate should the system be sending at.
  std::string physicalMode = "DsssRate1Mbps";
  
  // RUNNING ------------------------------------------------------------------
  
  // Holds all the network nodes.
  container.Create(nodeCount);
  
  WifiHelper wifi;
  // Set constant rate.
  wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager(
    "ns3::ConstantRateWifiManager",
    "DataMode", StringValue(physicalMode),
    "ControlMode", StringValue(physicalMode)
  );
  
  // Helper for setting up physical network.
  YansWifiPhyHelper phys = YansWifiPhyHelper::Default();
  phys.Set("RxGain", DoubleValue(-10));
  // 802.11b
  phys.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
    // Setting up the wifi channel for the physical layer.
    YansWifiChannelHelper channel;
    channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
    phys.SetChannel(channel.Create());
    
  NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
  mac.SetType("ns3::AdhocWifiMac");
  
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
  
  TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
  for(uint32_t idx = 0; idx < nodeCount; idx++) {
    PLLogWrite("node," << idx);
    sockets[idx] = Socket::CreateSocket(container.Get(idx), tid);
    InetSocketAddress local = InetSocketAddress(ipv4container.GetAddress(idx, 0), port);
    sockets[idx]->Bind(local);
    sockets[idx]->SetRecvCallback(MakeCallback(&ReceivePacket));
  }
  PLLogWrite(""); // Empty line between sections.
  
  // Give OLSR time to converge before starting packet sending.
  PLLogWrite(Simulator::Now().GetSeconds() << ",notice,OLSR converging");
  Simulator::Schedule(Seconds(30.0), &SendPacket, packetSize, numPackets, packetInterval);
  
  Simulator::Stop(Seconds(120.0));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}