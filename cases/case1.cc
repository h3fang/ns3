// Network topology
//
//            n0
//            | CSMA
//       ------------
//       | Switch 0 |
//       ------------
//            | CSMA
//       ------------
//       | Switch 1 |
//       ------------
//            | CSMA
//            n1
//
// Switch is replaced by router
// Fang He 2018/02/20

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

NS_LOG_COMPONENT_DEFINE("Case1");

bool verbose = true;
float simDurationSeconds = 10.0;

bool SetVerbose(std::string value) {
  verbose = true;
  return true;
}

int main(int argc, char *argv[]) {
  // Allow the user to override any of the defaults and the above Bind() at
  // run-time, via command-line arguments
  CommandLine cmd;
  cmd.Parse(argc, argv);

  Time::SetResolution (Time::NS);
  if (verbose) {
    LogComponentEnable("Case1", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
  }

  // Explicitly create the nodes required by the topology (shown above).
  NS_LOG_INFO("Create nodes.");
  NodeContainer terminals;
  terminals.Create(2);

  NodeContainer switchs;
  switchs.Create(2);

  NS_LOG_INFO("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute("Delay", StringValue ("2ms"));

  // Create the csma links, from each terminal to the switch
  NetDeviceContainer link0 = csma.Install(NodeContainer(terminals.Get(0), switchs.Get(0)));
  NetDeviceContainer link1 = csma.Install(NodeContainer(switchs.Get(0), switchs.Get(1)));
  NetDeviceContainer link2 = csma.Install(NodeContainer(switchs.Get(1), terminals.Get(1)));

  NetDeviceContainer lan1Devices;
  NetDeviceContainer lan2Devices;
  NetDeviceContainer switchDevices;

  lan1Devices.Add(link0.Get(0));
  lan1Devices.Add(link0.Get(1));
  switchDevices.Add(link1.Get(0));
  switchDevices.Add(link1.Get(1));
  lan2Devices.Add(link2.Get(1));
  lan2Devices.Add(link2.Get(0));

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

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Create applications
  NS_LOG_INFO("Create Applications.");
  uint16_t udpPort = 5000;

  UdpServerHelper server (udpPort);

  ApplicationContainer serverApp = server.Install (terminals.Get(1));
  serverApp.Start (Seconds (0.5));
  serverApp.Stop  (Seconds (simDurationSeconds));

  auto addr = terminals.Get(1)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
  UdpClientHelper client (addr, udpPort);

  client.SetAttribute ("MaxPackets", UintegerValue ((simDurationSeconds - 2.0) / 0.5));
  client.SetAttribute ("Interval",   TimeValue     (Seconds (0.5)));
  client.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApp = client.Install (terminals.Get(0));
  clientApp.Start (Seconds (0.5));
  clientApp.Stop  (Seconds (simDurationSeconds));

  NS_LOG_INFO("Configure Tracing.");
  // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // Trace output will be sent to the file "Case1.tr"
  AsciiTraceHelper ascii;
  csma.EnableAsciiAll(ascii.CreateFileStream("Case1.tr"));

  // Also configure some tcpdump traces; each interface will be traced.
  // The output files will be named:
  //     Case1-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -r" command (use "-tt" option to
  // display timestamps correctly)
  csma.EnablePcapAll("Case1", false);

  // NetAnim
  // doesn't work
  // MobilityHelper mobility;
  // mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  // mobility.Install (terminals);
  // mobility.Install (switchs);
  // AnimationInterface anim ("Case1-animation.xml");
  // anim.SetConstantPosition (terminals.Get(0), 10, 50);
  // anim.SetConstantPosition (terminals.Get(1), 40, 50);
  // anim.SetConstantPosition (switchs.Get(0), 60, 50);
  // anim.SetConstantPosition (switchs.Get(1), 90, 50);

  // Now, do the actual simulation.
  NS_LOG_INFO("Run Simulation.");
  Simulator::Run();

  Simulator::Destroy();
  NS_LOG_INFO("Done.");
}
