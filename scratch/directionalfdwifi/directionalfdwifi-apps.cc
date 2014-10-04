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
 *
 * Authors: Joe Kopena <tjkopena@cs.drexel.edu>
 *
 * These applications are used in the WiFi Distance Test experiment,
 * described and implemented in test02.cc.  That file should be in the
 * same place as this file.  The applications have two very simple
 * jobs, they just generate and receive packets.  We could use the
 * standard Application classes included in the NS-3 distribution.
 * These have been written just to change the behavior a little, and
 * provide more examples.
 *
 */

#include <ostream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"

#include "ns3/stats-module.h"

#include "directionalfdwifi-apps.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FdWifiApps");

TypeId
FdSender::GetTypeId (void)
{
  static TypeId tid = TypeId ("FdSender")
    .SetParent<Application> ()
    .AddConstructor<FdSender> ()
    .AddAttribute ("PacketSize", "The size of packets transmitted.",
                   UintegerValue (1500),
                   MakeUintegerAccessor (&FdSender::m_pktSize),
                   MakeUintegerChecker<uint32_t>(1))
    .AddAttribute ("Destination", "Target host address.",
                   Ipv4AddressValue ("255.255.255.255"),
                   MakeIpv4AddressAccessor (&FdSender::m_destAddr),
                   MakeIpv4AddressChecker ())
    .AddAttribute ("Port", "Destination app port.",
                   UintegerValue (1603),
                   MakeUintegerAccessor (&FdSender::m_destPort),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("Interval", "Delay between transmissions.",
                   StringValue ("ns3::ConstantRandomVariable[Constant=1]"),
                   MakePointerAccessor (&FdSender::m_interval),
                   MakePointerChecker <RandomVariableStream>())
    .AddAttribute ("Stream", "Random Stream.",
                   StringValue ("ns3::UniformRandomVariable[Stream=-1]"),
                   MakePointerAccessor (&FdSender::m_random),
                   MakePointerChecker <RandomVariableStream>())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&FdSender::m_txTrace))
  ;
  return tid;
}


FdSender::FdSender()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_interval = CreateObject<ConstantRandomVariable> ();
  m_socket = 0;
  m_random = CreateObject<UniformRandomVariable> ();
}

FdSender::~FdSender()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
FdSender::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_socket = 0;
  // chain up
  Application::DoDispose ();
}

void FdSender::StartApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();

  if (m_socket == 0) {
      Ptr<SocketFactory> socketFactory = GetNode ()->GetObject<SocketFactory>
          (UdpSocketFactory::GetTypeId ());
      m_socket = socketFactory->CreateSocket ();
      m_socket->Bind ();
    }

  Simulator::Cancel (m_sendEvent);
  m_sendEvent = Simulator::ScheduleNow (&FdSender::SendPacket, this);

  // end FdSender::StartApplication
}

void FdSender::StopApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_sendEvent);
  // end FdSender::StopApplication
}

void FdSender::SendPacket ()
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_INFO ("Sending packet at " << Simulator::Now () << " to " <<
               m_destAddr);

  Ptr<Packet> packet = Create<Packet>(m_pktSize);
  

  FdTimestampTag timestamp;
  timestamp.SetTimestamp (Simulator::Now ());
  packet->AddByteTag (timestamp);

  // Could connect the socket since the address never changes; using SendTo
  // here simply because all of the standard apps do not.
  m_socket->SendTo (packet, 0, InetSocketAddress (m_destAddr, m_destPort));

  // Report the event to the trace.
  m_txTrace (packet);
  double interval = m_interval->GetValue ();
  double logval = -log(m_random->GetValue());
  Time nextTxTime = Seconds (interval * logval);
  NS_LOG_INFO("nextTime:" << nextTxTime << " in:" << interval << " log:" << logval);
  m_sendEvent = Simulator::Schedule (nextTxTime, &FdSender::SendPacket, this);

}




//----------------------------------------------------------------------
//-- FdReceiver
//------------------------------------------------------
TypeId
FdReceiver::GetTypeId (void)
{
  static TypeId tid = TypeId ("FdReceiver")
    .SetParent<Application> ()
    .AddConstructor<FdReceiver> ()
    .AddAttribute ("Port", "Listening port.",
                   UintegerValue (1603),
                   MakeUintegerAccessor (&FdReceiver::m_port),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("NumPackets", "Total number of packets to recv.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&FdReceiver::m_numPkts),
                   MakeUintegerChecker<uint32_t>(0))
    .AddTraceSource ("Rx", "Receive data packet",
                     MakeTraceSourceAccessor (&FdReceiver::m_rxTrace))
  ;
  return tid;
}

FdReceiver::FdReceiver() :
  m_calc (0),
  m_delay (0)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_socket = 0;
}

FdReceiver::~FdReceiver()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
FdReceiver::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_socket = 0;
  // chain up
  Application::DoDispose ();
}

void
FdReceiver::StartApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();

  if (m_socket == 0) {
      Ptr<SocketFactory> socketFactory = GetNode ()->GetObject<SocketFactory>
          (UdpSocketFactory::GetTypeId ());
      m_socket = socketFactory->CreateSocket ();
      InetSocketAddress local = 
        InetSocketAddress (Ipv4Address::GetAny (), m_port);
      m_socket->Bind (local);
    }

  m_socket->SetRecvCallback (MakeCallback (&FdReceiver::Receive, this));
  m_count = 0;

  // end FdReceiver::StartApplication
}

void
FdReceiver::StopApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();

  if (m_socket != 0) {
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }

  // end FdReceiver::StopApplication
}

void
FdReceiver::SetCounter (Ptr<CounterCalculator<> > calc)
{
  m_calc = calc;
  // end FdReceiver::SetCounter
}
void
FdReceiver::SetDelayTracker (Ptr<TimeMinMaxAvgTotalCalculator> delay)
{
  m_delay = delay;
  // end FdReceiver::SetDelayTracker
}

void
FdReceiver::Receive (Ptr<Socket> socket)
{
  // NS_LOG_FUNCTION (this << socket << packet << from);
  
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from))) {
    if (InetSocketAddress::IsMatchingType (from)) {
      NS_LOG_INFO ("FdReceived " << packet->GetSize () << " bytes from " <<
		   InetSocketAddress::ConvertFrom (from).GetIpv4 ());
    }
    FdTimestampTag timestamp;
    // Should never not be found since the sender is adding it, but
    // you never know.
    if (packet->FindFirstMatchingByteTag (timestamp) && m_numPkts != 0) {
      Time tx = timestamp.GetTimestamp ();
      
      if (m_delay != 0) {
	m_delay->Update (Simulator::Now () - tx);
      }
      /* [add] 20140618 sugiyama */
      m_rxTrace (++m_count, m_numPkts);
      /* [end]*/
    }
    
    if (m_calc != 0) {
      m_calc->Update ();
    }
    
    // end receiving packets
  }
  
  // end FdReceiver::FdReceive
}

//----------------------------------------------------------------------
//-- FdTimestampTag
//------------------------------------------------------
TypeId 
FdTimestampTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("FdTimestampTag")
    .SetParent<Tag> ()
    .AddConstructor<FdTimestampTag> ()
    .AddAttribute ("FdTimestamp",
                   "Some momentous point in time!",
                   EmptyAttributeValue (),
                   MakeTimeAccessor (&FdTimestampTag::GetTimestamp),
                   MakeTimeChecker ())
  ;
  return tid;
}
TypeId 
FdTimestampTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t 
FdTimestampTag::GetSerializedSize (void) const
{
  return 8;
}
void 
FdTimestampTag::Serialize (TagBuffer i) const
{
  int64_t t = m_timestamp.GetNanoSeconds ();
  i.Write ((const uint8_t *)&t, 8);
}
void 
FdTimestampTag::Deserialize (TagBuffer i)
{
  int64_t t;
  i.Read ((uint8_t *)&t, 8);
  m_timestamp = NanoSeconds (t);
}

void
FdTimestampTag::SetTimestamp (Time time)
{
  m_timestamp = time;
}
Time
FdTimestampTag::GetTimestamp (void) const
{
  return m_timestamp;
}

void 
FdTimestampTag::Print (std::ostream &os) const
{
  os << "t=" << m_timestamp;
}
