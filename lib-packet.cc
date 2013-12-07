#include "lib.h"

using namespace ns3;

// Scan a packet for an IPv4 header. Returns blank header if not found.
Ipv4Header GetIpv4Header(Ptr<const Packet> p) {
  Ipv4Header ipv4Header;
  Ptr<Packet> packet = p->Copy();
  PacketMetadata::ItemIterator metadataIterator = packet->BeginItem();
  PacketMetadata::Item item;
  while (metadataIterator.HasNext()) {
    item = metadataIterator.Next();
    if(item.tid.GetName() == "ns3::Ipv4Header") {
      ipv4Header.Deserialize(item.current);
      return ipv4Header;
    }
  }
  return ipv4Header;
}
