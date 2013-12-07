#include "lib.h"

// Create a Wifi networ helper with give data rate.
// rate: eg. "DsssRate1Mbps"
ns3::WifiHelper PLCreateWifiHelper(std::string rate) {
  ns3::WifiHelper wifi;
  // Set constant rate.
  wifi.SetStandard(ns3::WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager(
    "ns3::ConstantRateWifiManager",
    "DataMode",    ns3::StringValue(rate),
    "ControlMode", ns3::StringValue(rate)
  );
  return wifi;
}

// Returns a physical layer helper for Wifi networks.
// Has default gain of -10 dB and IEEE 802.11.
ns3::YansWifiPhyHelper PLCreateWifiPhysicalHelper() {
  ns3::YansWifiPhyHelper phys = ns3::YansWifiPhyHelper::Default();
  phys.Set("RxGain", ns3::DoubleValue(-10));
  // 802.11b
  phys.SetPcapDataLinkType (ns3::YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
    // Setting up the wifi channel for the physical layer.
    ns3::YansWifiChannelHelper channel;
    channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
    phys.SetChannel(channel.Create());
  return phys;
}
