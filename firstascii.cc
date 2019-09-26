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
#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  // bool tracing = false;
  // uint32_t maxBytes = 0;
  CommandLine cmd;
  // cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing);
  // cmd.AddValue ("maxBytes",
  //               "Total number of bytes for application to send", maxBytes);
  cmd.Parse (argc, argv);
  //Take logs we are taking logs for research
  Time::SetResolution (Time::NS); // total time your application is going to consume
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO); //  log component enable enables the log 
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes; // creating an object from NodeContaier class ad the object named is nodes 
  // nodecontainer class has the method create  it created 2 nodes over here 
  nodes.Create (2); //how many nodes or how many computers are being utilised 
  //here point to point communication so only 2 nodes 


  //here we are definig what channel we want it can be anyhting wifi,ethernet 
  // here the channel being used is pointtopoint 
  // now pointtopointhelper class is being used and object of it is created
  PointToPointHelper pointToPoint;
  //setting attributes to device 
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  //setting attributes to channel 
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
  //Both devices will be configured to transmit
 //data at five megabits per second over the channel which has a two millisecond transmission delay.


  //here we are installing the pointtopoint channel on top of our nodes 
  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);
  
  // now the devices that is the created nodes have to follow some rules those are there on internet 
  // for this we use the internetstackhelper class this helps in installing the internet protocols
  // on the top of those devices 
  InternetStackHelper stack;
  stack.Install (nodes);
  

  //here we are assigning the ip address 
  Ipv4AddressHelper address;
  //set base  ip address it takes 2 arguments ip and subnet mask 
  // ipv6 address helper class ould also be used  
  address.SetBase ("10.1.1.0", "255.255.255.0");

  // so the ip is assigned to all the devices we have created so far
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  //here what type of server we want so we are echo server as we want echo server 
  //creating a x type server this server is running on port 9
  UdpEchoServerHelper echoServer (9);


  //the nodes.get(1)  method helps in  acessing the created nodes 0th node is the first node 1st node is the second node
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  // here the server is installed on the nodes and is made running on nodes and then the server is stopped 
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));


  // just like for server side we are creating echo client 
  // the client gets address of 1 and communicates with port number 9
  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  // now attribues are being set 
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

    
      AsciiTraceHelper ascii;
      pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("abc1.tr"));
    

  // the nodes.get(0) method helps in accesing the created nodes 
  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  // client is being installed on the node and is made running on nodes and then the client is stopped 
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  // running simulation 
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
// the output shows that we have created ip and port for server
