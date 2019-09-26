#include <fstream>
#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Lab2");

int main (int argc, char *argv[])
{
    CommandLine cmd;
    // cmd.AddValue ("latency", "P2P link Latency in miliseconds", lat);
    // cmd.AddValue ("rate", "P2P data rate in bps", rate);
    //cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);

    cmd.Parse (argc, argv);
    NS_LOG_INFO ("Create nodes.");
    std::cout<<"creating nodes";
    NodeContainer c; // ALL Nodes
    c.Create(3);

    NodeContainer n0n1 = NodeContainer (c.Get (0), c.Get (1));
    NodeContainer n2n1 = NodeContainer (c.Get (2), c.Get (1));

    InternetStackHelper internet;
    internet.Install (c);

    NS_LOG_INFO ("Creating channel1");
    std::cout<<"creating channel1";
    PointToPointHelper p2p1;
    p2p1.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    p2p1.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer d0d1 = p2p1.Install (n0n1);

    NS_LOG_INFO ("Creating channel2");
    std::cout<<"creating channel2";
    PointToPointHelper p2p2;
    p2p2.SetDeviceAttribute ("DataRate", StringValue ("500Kbps"));
    p2p2.SetChannelAttribute ("Delay", StringValue ("5ms"));

    NetDeviceContainer d2d1 = p2p2.Install (n2n1);

    NS_LOG_INFO ("Assign IP Addresses.");
    std::cout<<"assign ip address";
    Ipv4AddressHelper ipv4;
    
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i0i1 = ipv4.Assign (d0d1);
    
    ipv4.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i2i1 = ipv4.Assign (d2d1);

    NS_LOG_INFO ("Enable static global routing.");
    std::cout<<"enable static global routing";

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    NS_LOG_INFO ("Create Applications.");
    std::cout<<"create applications";

    std::cout<<"tcp connection from 0-1";

   // uint16_t sinkPort = 8080;
    Address sinkAddress (InetSocketAddress (i0i1.GetAddress (1), 8080));
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 8080));
    ApplicationContainer sinkApps = packetSinkHelper.Install (c.Get (1)); //n1 as sink
    sinkApps.Start (Seconds (0.0));
    sinkApps.Stop (Seconds (100.0));

    OnOffHelper onOffHelper ("ns3::TcpSocketFactory", Address ());
   onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

    ApplicationContainer spokeApps;
    AddressValue remoteAddress (InetSocketAddress (i0i1.GetAddress(0), 8080));
    onOffHelper.SetAttribute ("Remote", remoteAddress);
    ApplicationContainer onoff=onOffHelper.Install(c.Get(0));
    onoff.Start (Seconds(1.0));
    onoff.Stop(Seconds(100.0));
    spokeApps.Add (onoff);
    //spokeApps.Add (sinkApps);
    

    BulkSendHelper bulkSendHelper ("ns3::TcpSocketFactory",Address());
    AddressValue remoteAddress1(InetSocketAddress (i2i1.GetAddress(2), 8080));
    bulkSendHelper.SetAttribute("Remote",remoteAddress1);
    ApplicationContainer bulk=bulkSendHelper.Install(c.Get(2));
    bulk.Start (Seconds (2.0));
    bulk.Stop (Seconds (100.0));
    
    spokeApps.Add (bulk);

    AsciiTraceHelper ascii;
        p2p1.EnableAsciiAll (ascii.CreateFileStream ("itry.tr"));


    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}
    




