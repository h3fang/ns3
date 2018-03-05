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
//          |   |  CSMA
//          n2  n3
//
// Switch is replaced by router
// Yanran Wang and Fang He 2018/03/01

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

NS_LOG_COMPONENT_DEFINE("Case3");

bool verbose = true;
float simDurationSeconds = 10.0;

int main(int argc, char *argv[]) {
  // Allow the user to override any of the defaults and the above Bind() at
  // run-time, via command-line arguments
  CommandLine cmd;
  cmd.Parse(argc, argv);

  Time::SetResolution(Time::NS);
  if (verbose) {
    LogComponentEnable("Case3", LOG_LEVEL_INFO);
    LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
  }

  // Explicitly create the nodes required by the topology (shown above).
  NS_LOG_INFO("Create nodes.");
  NodeContainer terminals;
  terminals.Create(4);

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
  NetDeviceContainer link4 =
      csma.Install(NodeContainer(switchs.Get(1), terminals.Get(3)));
  //NetDeviceContainer link5 =
  //    csma.Install(NodeContainer(switchs.Get(1), terminals.Get(4)));
      
  NetDeviceContainer lan1Devices;
  NetDeviceContainer lan2Devices;
  NetDeviceContainer lan3Devices;
  NetDeviceContainer switchDevices;

  lan1Devices.Add(link0.Get(0));
  lan1Devices.Add(link1.Get(0));
  lan1Devices.Add(link0.Get(1));
  lan1Devices.Add(link1.Get(1));
  switchDevices.Add(link2.Get(0));
  switchDevices.Add(link2.Get(1));
  lan2Devices.Add(link3.Get(1));
  lan2Devices.Add(link3.Get(0));
  lan3Devices.Add(link4.Get(1));
  lan3Devices.Add(link4.Get(0));
  
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
  ipv4.SetBase("10.1.3.0", "255.255.255.0");
  ipv4.Assign(lan3Devices);
  ipv4.SetBase("76.1.1.0", "255.255.255.0");
  ipv4.Assign(switchDevices);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Create applications
  NS_LOG_INFO("Create Applications.");
  //the first server and client
  uint16_t udpPort0 = 5000;

  UdpServerHelper server0(udpPort0);
  ApplicationContainer serverApp0 = server0.Install(terminals.Get(2));
  serverApp0.Start(Seconds(0.5));
  serverApp0.Stop(Seconds(simDurationSeconds));
  
  auto addr0 = terminals.Get(2)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
  //auto addr = interfaces.GetAddress(0,0);
  NS_LOG_INFO(addr0);
  UdpClientHelper client0(addr0, udpPort0);

  client0.SetAttribute("MaxPackets", UintegerValue((simDurationSeconds - 2.0) / 0.5));
  client0.SetAttribute("Interval", TimeValue(Seconds(1)));
  client0.SetAttribute("PacketSize", UintegerValue(1024));
  ApplicationContainer clientApps0;
  for (size_t i = 0; i < 2; i++) {
    clientApps0.Add(client0.Install(terminals.Get(i)));
  }
  clientApps0.Start(Seconds(0.5));
  clientApps0.Stop(Seconds(simDurationSeconds));
  
  
  //the second server and client
  uint16_t udpPort1 = 5000;

  UdpServerHelper server1(udpPort1);
  ApplicationContainer serverApp1 = server1.Install(terminals.Get(3));
  serverApp1.Start(Seconds(1));
  serverApp1.Stop(Seconds(simDurationSeconds));
  
  auto addr1 = terminals.Get(3)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
  //auto addr = interfaces.GetAddress(0,0);
  NS_LOG_INFO(addr1);
  UdpClientHelper client1(addr1, udpPort1);

  client1.SetAttribute("MaxPackets", UintegerValue((simDurationSeconds - 2.0) / 0.5));
  client1.SetAttribute("Interval", TimeValue(Seconds(1)));
  client1.SetAttribute("PacketSize", UintegerValue(1024));
  ApplicationContainer clientApps1;
  for (size_t i = 0; i < 2; i++) {
    clientApps1.Add(client1.Install(terminals.Get(i)));
  }
  clientApps1.Start(Seconds(1));
  clientApps1.Stop(Seconds(simDurationSeconds));

  NS_LOG_INFO("Configure Tracing.");
  // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // Trace output will be sent to the file "Case33.tr"
  AsciiTraceHelper ascii;
  csma.EnableAsciiAll(ascii.CreateFileStream("Case3.tr"));

  // Also configure some tcpdump traces; each interface will be traced.
  // The output files will be named:
  //     Case33-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -r" command (use "-tt" option to
  // display timestamps correctly)
  csma.EnablePcapAll("Case3", false);
  
  // Trace routing tables 
  Ipv4GlobalRoutingHelper g;
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("Case3-global-routing.routes", std::ios::out);
  g.PrintRoutingTableAllAt (Seconds (1), routingStream);

  // Now, do the actual simulation.
  NS_LOG_INFO("Run Simulation.");
  Simulator::Run();

  Simulator::Destroy();
  NS_LOG_INFO("Done.");
}
