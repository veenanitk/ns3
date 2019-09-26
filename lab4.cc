/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

 // LogComponentEnable ("BulkSendApplication", LOG_LEVEL_INFO);
 // LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
  std::cout<<"1.TcpWestwood 2.TcpHighSpeed";
  int a;
  std::cin>>a;
  if(a==1){
  std::string transport_prot = "TcpWestwood";
  // TcpWestwoodPlus is not an actual TypeId name; we need TcpWestwood here
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
  // the default protocol type in ns3::TcpWestwood is WESTWOOD
  Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
}
  else{
    std::string transport_prot = "ns3::TcpHighSpeed";
    TypeId tcpTid;
    NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (transport_prot, &tcpTid), "TypeId " << transport_prot << " not found");
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (transport_prot)));
  }
  std::cout<<"window scaling:1 enable 0 disable"<<std::endl;
  bool tcpws;
  std::cin>>tcpws;
  if(!tcpws)
  Config::SetDefault ("ns3::TcpSocketBase::WindowScaling", BooleanValue (false));

  uint32_t tcp_adu_size = 1000;
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (tcp_adu_size));


  PointToPointHelper p2pleft;
  p2pleft.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2pleft.SetChannelAttribute ("Delay", StringValue ("2ms"));

  PointToPointHelper p2pright;
  p2pright.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2pright.SetChannelAttribute ("Delay", StringValue ("2ms"));

  PointToPointHelper p2pbottle;
  p2pbottle.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2pbottle.SetChannelAttribute ("Delay", StringValue ("2ms"));

  AsciiTraceHelper ascii;
  p2pbottle.EnableAsciiAll (ascii.CreateFileStream ("first2.tr"));
  p2pbottle.EnablePcapAll ("first2", false);

  uint32_t n=5; 

  PointToPointDumbbellHelper dumbbell(n,p2pleft,n,p2pright,p2pbottle);
  
  InternetStackHelper ist;
  dumbbell.InstallStack(ist);
  dumbbell.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),Ipv4AddressHelper ("11.1.1.0", "255.255.255.0"),Ipv4AddressHelper ("12.1.1.0", "255.255.255.0"));


  uint16_t port = 50000;

  Address hubLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", hubLocalAddress);
  ApplicationContainer hubApp = packetSinkHelper.Install (dumbbell.GetRight (0));
  hubApp.Start (Seconds (1.0));
  hubApp.Stop (Seconds (10.0));

  ApplicationContainer hubApp1 = packetSinkHelper.Install (dumbbell.GetRight (1));
  hubApp1.Start (Seconds (1.0));
  hubApp1.Stop (Seconds (10.0));

  ApplicationContainer hubApp2 = packetSinkHelper.Install (dumbbell.GetRight (2));
  hubApp2.Start (Seconds (1.0));
  hubApp2.Stop (Seconds (10.0));

  OnOffHelper onOffHelper ("ns3::TcpSocketFactory", Address ());
  onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  ApplicationContainer spokeApps;
  for (uint32_t i = 0; i < 3; ++i)
    {
      AddressValue remoteAddress (InetSocketAddress (dumbbell.GetRightIpv4Address (i), port));
      onOffHelper.SetAttribute ("Remote", remoteAddress);
      spokeApps.Add (onOffHelper.Install (dumbbell.GetLeft (i)));
    }
  spokeApps.Start (Seconds (1.0));
  spokeApps.Stop (Seconds (10.0));

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (dumbbell.GetRight (3));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  ApplicationContainer serverApps1 = echoServer.Install (dumbbell.GetRight(4));
  serverApps1.Start (Seconds (1.0));
  serverApps1.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (dumbbell.GetRightIpv4Address (3), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (10));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (dumbbell.GetLeft (3));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient1 (dumbbell.GetRightIpv4Address (4), 9);
  echoClient1.SetAttribute ("MaxPackets", UintegerValue (10));
  echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps1 = echoClient1.Install (dumbbell.GetLeft (4));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (10.0));
  /*NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));*/

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
