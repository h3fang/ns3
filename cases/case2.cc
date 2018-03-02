// Network topology
//
//          n0  n1
//          |   |  CSMA
//       ------------
//       | Switch 0 |
//       ------------
//            | CSMA
//       ------------
//       | Switch 1 |
//       ------------
//            | CSMA
//            n2
//
// Switch is replaced by router
// Fang He 2018/02/22

#include <fstream>
#include <iostream>

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/log.h"
// #include "ns3/netanim-module.h"
// #include "ns3/mobility-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Case2");

bool verbose = true;
float simDurationSeconds = 10.0;

int main(int argc, char *argv[]) {
  // Allow the user to override any of the defaults and the above Bind() at
  // run-time, via command-line arguments
  CommandLine cmd;
  cmd.Parse(argc, argv);

  Time::SetResolution(Time::NS);
  if (verbose) {
    LogComponentEnable("Case2", LOG_LEVEL_INFO);
    LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
  }

  // Explicitly create the nodes required by the topology (shown above).
  NS_LOG_INFO("Create nodes.");
  NodeContainer terminals;
  terminals.Create(3);

  NodeContainer switchs;
  switchs.Create(2);

  NS_LOG_INFO("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
  csma.SetChannelAttribute("Delay", StringValue("2ms"));

  // Create the csma links, from each terminal to the switch
  NetDeviceContainer link0 =
      csma.Install(NodeContainer(terminals.Get(0), switchs.Get(0)));
  NetDeviceContainer link1 =
      csma.Install(NodeContainer(terminals.Get(1), switchs.Get(0)));
  NetDeviceContainer link2 =
      csma.Install(NodeContainer(switchs.Get(0), switchs.Get(1)));
  NetDeviceContainer link3 =
      csma.Install(NodeContainer(switchs.Get(1), terminals.Get(2)));

  NetDeviceContainer lan1Devices;
  NetDeviceContainer lan2Devices;
  NetDeviceContainer switchDevices;

  lan1Devices.Add(link0.Get(0));
  lan1Devices.Add(link1.Get(0));
  lan1Devices.Add(link0.Get(1));
  lan1Devices.Add(link1.Get(1));
  switchDevices.Add(link2.Get(0));
  switchDevices.Add(link2.Get(1));
  lan2Devices.Add(link3.Get(1));
  lan2Devices.Add(link3.Get(0));

  // Add internet stack to the terminals
  InternetStackHelper internet;
  internet.Install(terminals);
  internet.Install(switchs);

  // We've got the "hardware" in place.  Now we need to add IP addresses.
  NS_LOG_INFO("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.1.0", "255.255.255.0");
  ipv4.Assign(lan1Devices);
  ipv4.SetBase("10.1.2.0", "255.255.255.0");
  ipv4.Assign(lan2Devices);
  ipv4.SetBase("76.1.1.0", "255.255.255.0");
  ipv4.Assign(switchDevices);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Create applications
  NS_LOG_INFO("Create Applications.");
  uint16_t udpPort = 5000;

  UdpServerHelper server(udpPort);

  ApplicationContainer serverApp = server.Install(terminals.Get(2));
  serverApp.Start(Seconds(0.5));
  serverApp.Stop(Seconds(simDurationSeconds));

  auto addr = terminals.Get(2)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
  UdpClientHelper client(addr, udpPort);

  client.SetAttribute("MaxPackets", UintegerValue((simDurationSeconds - 2.0) / 0.5));
  client.SetAttribute("Interval", TimeValue(Seconds(0.5)));
  client.SetAttribute("PacketSize", UintegerValue(1024));
  ApplicationContainer clientApps;
  for (size_t i = 0; i < 2; i++) {
    clientApps.Add(client.Install(terminals.Get(i)));
  }
  clientApps.Start(Seconds(0.5));
  clientApps.Stop(Seconds(simDurationSeconds));

  NS_LOG_INFO("Configure Tracing.");
  // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // Trace output will be sent to the file "Case2.tr"
  AsciiTraceHelper ascii;
  csma.EnableAsciiAll(ascii.CreateFileStream("Case2.tr"));

  // Also configure some tcpdump traces; each interface will be traced.
  // The output files will be named:
  //     Case2-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -r" command (use "-tt" option to
  // display timestamps correctly)
  csma.EnablePcapAll("Case2", false);
  
  // Trace routing tables 
  Ipv4GlobalRoutingHelper g;
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("Case2-global-routing.routes", std::ios::out);
  g.PrintRoutingTableAllAt (Seconds (1), routingStream);

  // Now, do the actual simulation.
  NS_LOG_INFO("Run Simulation.");
  Simulator::Run();

  Simulator::Destroy();
  NS_LOG_INFO("Done.");
}
