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
                   MakeDoubleAccessor (&PieQueueDisc::m_inW),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Phi",
                   "Value of phi used in calculation of probability",
                   DoubleValue (1.001),
                   MakeDoubleAccessor (&PieQueueDisc::m_phi),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MeanPktSize",
                   "Average packet size",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&RemQueueDisc::m_meanPktSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("UpdateInterval",
                   "Time period after which link price and probability are calculated",
                   TimeValue (Seconds (0.002)),
                   MakeTimeAccessor (&PieQueueDisc::m_updateInterval),
                   MakeTimeChecker ())
    .AddAttribute ("Target",
                   "Target queue length",
                   UintegerValue (20),
                   MakeUintegerAccessor (&PieQueueDisc::m_target),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Gamma",
                   "Value of gamma",
                   DoubleValue (0.001),
                   MakeDoubleAccessor (&PieQueueDisc::m_gamma),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Alpha",
                   "Value of alpha",
                   DoubleValue (0.1),
                   MakeDoubleAccessor (&PieQueueDisc::m_alpha),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("QueueLimit",
                   "Queue limit in packets",
                   UintegerValue (50),
                   MakeUintegerAccessor (&PieQueueDisc::m_queueLimit),
                   MakeUintegerChecker<uint32_t> ())
    
  ;

  return tid;
}



} //namespace ns3
