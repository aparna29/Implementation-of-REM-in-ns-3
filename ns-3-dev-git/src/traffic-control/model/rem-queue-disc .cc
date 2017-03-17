/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 NITK Surathkal
 *
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
 * Authors: Aparna R. Joshi <aparna29th@gmail.com>
 *          Isha Tarte <tarteisha@gmail.com>
 *          Navya R S <navyars82@gmail.com>
 */

/*
 * PORT NOTE: This code was ported from ns-2.36rc1 (queue/rem.cc).
 * Most of the comments are also ported from the same.
 */

#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/simulator.h"
#include "ns3/abort.h"
#include "rem-queue-disc.h"
#include "ns3/drop-tail-queue.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("RemQueueDisc");

NS_OBJECT_ENSURE_REGISTERED (RemQueueDisc);

TypeId RemQueueDisc::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RemQueueDisc")
    .SetParent<QueueDisc> ()
    .SetGroupName ("TrafficControl")
    .AddConstructor<RemQueueDisc> ()
    .AddAttribute ("Mode",
                   "Determines unit for QueueLength",
                   EnumValue (Queue::QUEUE_MODE_PACKETS),
                   MakeEnumAccessor (&RemQueueDisc::SetMode),
                   MakeEnumChecker (Queue::QUEUE_MODE_BYTES, "QUEUE_MODE_BYTES",
                                    Queue::QUEUE_MODE_PACKETS, "QUEUE_MODE_PACKETS"))
    .AddAttribute ("InputWeight",
                   "Weight assigned to input rate",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&RemQueueDisc::m_inW),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Phi",
                   "Value of phi used to calculate probability",
                   DoubleValue (1.001),
                   MakeDoubleAccessor (&RemQueueDisc::m_phi),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MeanPktSize",
                   "Average packet size",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&RemQueueDisc::m_meanPktSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("UpdateInterval",
                   "Time period after which link price and probability are calculated",
                   TimeValue (Seconds (0.002)),
                   MakeTimeAccessor (&RemQueueDisc::m_updateInterval),
                   MakeTimeChecker ())
    .AddAttribute ("Target",
                   "Target queue length",
                   UintegerValue (20),
                   MakeUintegerAccessor (&RemQueueDisc::m_target),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Gamma",
                   "Value of gamma",
                   DoubleValue (0.001),
                   MakeDoubleAccessor (&RemQueueDisc::m_gamma),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Alpha",
                   "Value of alpha",
                   DoubleValue (0.1),
                   MakeDoubleAccessor (&RemQueueDisc::m_alpha),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("QueueLimit",
                   "Queue limit in packets",
                   UintegerValue (50),
                   MakeUintegerAccessor (&RemQueueDisc::m_queueLimit),
                   MakeUintegerChecker<uint32_t> ())
    
  ;

  return tid;
}

RemQueueDisc::RemQueueDisc ()
  : QueueDisc ()
{
  NS_LOG_FUNCTION (this);
  m_uv = CreateObject<UniformRandomVariable> ();
  m_rtrsEvent = Simulator::Schedule (m_updateInterval, &RemQueueDisc::RunUpdateRule, this);
}

RemQueueDisc::~RemQueueDisc ()
{
  NS_LOG_FUNCTION (this);
}

void
RemQueueDisc::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_uv = 0;
  Simulator::Remove (m_rtrsEvent);
  QueueDisc::DoDispose ();
}

void
RemQueueDisc::SetMode (Queue::QueueMode mode)
{
  NS_LOG_FUNCTION (this << mode);
  m_mode = mode;
}

Queue::QueueMode
RemQueueDisc::GetMode (void)
{
  NS_LOG_FUNCTION (this);
  return m_mode;
}

void
RemQueueDisc::SetQueueLimit (uint32_t lim)
{
  NS_LOG_FUNCTION (this << lim);
  m_queueLimit = lim;
}

uint32_t
RemQueueDisc::GetQueueSize (void)
{
  NS_LOG_FUNCTION (this);
  if (GetMode () == Queue::QUEUE_MODE_BYTES)
    {
      return GetInternalQueue (0)->GetNBytes ();
    }
  else if (GetMode () == Queue::QUEUE_MODE_PACKETS)
    {
      return GetInternalQueue (0)->GetNPackets ();
    }
  else
    {
      NS_ABORT_MSG ("Unknown REM mode.");
    }
}

RemQueueDisc::Stats
RemQueueDisc::GetStats ()
{
  NS_LOG_FUNCTION (this);
  return m_stats;
}

int64_t
RemQueueDisc::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_uv->SetStream (stream);
  return 1;
}

void
RemQueueDisc::InitializeParams (void)
{
  // Initially queue is empty so variables are initialize to zero except for the variables used in the calculation of link price and probability
  m_inW = 1;
  m_phi = 1.001;
  m_meanPktSize = 1000;
  m_updateInterval = 0.002; 
  m_target = 20;   
  m_gamma = 0.001;
  m_alpha = 0.1;
  m_queueLimit = 50; 
  
  m_linkPrice = 0.0;
  m_dropProb = 0.0;
  m_inputRate = 0.0;
  m_avgInputRate = 0.0;
  m_count = 0;
  m_countInBytes = 0;
  
  m_stats.qLimDrop = 0;
  m_stats.unforcedDrop = 0;
}

} //namespace ns3
