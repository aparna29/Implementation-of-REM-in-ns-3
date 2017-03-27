/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 NITK Surathkal
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
 *
 */

#include "ns3/test.h"
#include "ns3/rem-queue-disc.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

using namespace ns3;

/**
 * \ingroup traffic-control-test
 * \ingroup tests
 *
 * \brief Rem Queue Disc Test Item
 */
class RemQueueDiscTestItem : public QueueDiscItem
{
public:
  /**
   * Constructor
   *
   * \param p the packet
   * \param addr the address
   * \param protocol the protocol
   */
  RemQueueDiscTestItem (Ptr<Packet> p, const Address & addr, uint16_t protocol);
  virtual ~RemQueueDiscTestItem ();
  virtual void AddHeader (void);
  virtual bool Mark (void);

private:
  RemQueueDiscTestItem ();
  /// copy constructor
  RemQueueDiscTestItem (const RemQueueDiscTestItem &);
  /// assignment operator
  RemQueueDiscTestItem &operator = (const RemQueueDiscTestItem &);
};

RemQueueDiscTestItem::RemQueueDiscTestItem (Ptr<Packet> p, const Address & addr, uint16_t protocol)
  : QueueDiscItem (p, addr, protocol)
{
}

RemQueueDiscTestItem::~RemQueueDiscTestItem ()
{
}

void
RemQueueDiscTestItem::AddHeader (void)
{
}

bool
RemQueueDiscTestItem::Mark (void)
{
  return false;
}

/**
 * \ingroup traffic-control-test
 * \ingroup tests
 *
 * \brief Rem Queue Disc Test Case
 */
class RemQueueDiscTestCase : public TestCase
{
public:
  RemQueueDiscTestCase ();
  virtual void DoRun (void);
private:
  /**
   * Enqueue function
   * \param queue the queue disc
   * \param size the size
   * \param nPkt the number of packets
   */
  void Enqueue (Ptr<RemQueueDisc> queue, uint32_t size, uint32_t nPkt);
  /**
   * Enqueue with delay function
   * \param queue the queue disc
   * \param size the size
   * \param nPkt the number of packets
   */
   void Dequeue (Ptr<RemQueueDisc> queue, uint32_t nPkt);
  /**
   * Dequeue with delay function
   * \param queue the queue disc
   * \param delay the delay
   * \param nPkt the number of packets
   */
  void RunRemTest (StringValue mode);
};

RemQueueDiscTestCase::RemQueueDiscTestCase ()
  : TestCase ("Sanity check on the rem queue disc implementation")
{
}

void
RemQueueDiscTestCase::RunRemTest (StringValue mode)
{
  uint32_t pktSize = 0;

  // 1 for packets; pktSize for bytes
  uint32_t modeSize = 1;

  uint32_t qSize = 300;
  Ptr<RemQueueDisc> queue = CreateObject<RemQueueDisc> ();


  // test 1: simple enqueue/dequeue with defaults, no drops
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Mode", mode), true,
                         "Verify that we can actually set the attribute Mode");

  Address dest;

  if (queue->GetMode () == Queue::QUEUE_MODE_BYTES)
    {
      // pktSize should be same as MeanPktSize to avoid performance gap between byte and packet mode
      pktSize = 1000;
      modeSize = pktSize;
      qSize = qSize * modeSize;
    }

  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("QueueLimit", UintegerValue (qSize)), true,
                         "Verify that we can actually set the attribute QueueLimit");

  Ptr<Packet> p1, p2, p3, p4, p5, p6, p7, p8;
  p1 = Create<Packet> (pktSize);
  p2 = Create<Packet> (pktSize);
  p3 = Create<Packet> (pktSize);
  p4 = Create<Packet> (pktSize);
  p5 = Create<Packet> (pktSize);
  p6 = Create<Packet> (pktSize);
  p7 = Create<Packet> (pktSize);
  p8 = Create<Packet> (pktSize);

  queue->Initialize ();
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 0 * modeSize, "There should be no packets in there");
  queue->Enqueue (Create<RemQueueDiscTestItem> (p1, dest, 0));
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 1 * modeSize, "There should be one packet in there");
  queue->Enqueue (Create<RemQueueDiscTestItem> (p2, dest, 0));
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 2 * modeSize, "There should be two packets in there");
  queue->Enqueue (Create<RemQueueDiscTestItem> (p3, dest, 0));
  queue->Enqueue (Create<RemQueueDiscTestItem> (p4, dest, 0));
  queue->Enqueue (Create<RemQueueDiscTestItem> (p5, dest, 0));
  queue->Enqueue (Create<RemQueueDiscTestItem> (p6, dest, 0));
  queue->Enqueue (Create<RemQueueDiscTestItem> (p7, dest, 0));
  queue->Enqueue (Create<RemQueueDiscTestItem> (p8, dest, 0));
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 8 * modeSize, "There should be eight packets in there");

  Ptr<QueueDiscItem> item;

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the first packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 7 * modeSize, "There should be seven packets in there");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p1->GetUid (), "was this the first packet ?");

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the second packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 6 * modeSize, "There should be six packet in there");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p2->GetUid (), "Was this the second packet ?");

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the third packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 5 * modeSize, "There should be five packets in there");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p3->GetUid (), "Was this the third packet ?");

  item = queue->Dequeue ();
  item = queue->Dequeue ();
  item = queue->Dequeue ();
  item = queue->Dequeue ();
  item = queue->Dequeue ();

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item == 0), true, "There are really no packets in there");


  // test 2: more data with defaults, unforced drops but no forced drops
  queue = CreateObject<RemQueueDisc> ();
  pktSize = 1000; 
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Mode", mode), true,
                         "Verify that we can actually set the attribute Mode");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("QueueLimit", UintegerValue (qSize)), true,
                         "Verify that we can actually set the attribute QueueLimit");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Alpha", DoubleValue (0.1)), true,
                         "Verify that we can actually set the attribute Alpha");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Gamma", DoubleValue (0.001)), true,
                         "Verify that we can actually set the attribute Gamma");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("UpdateInterval", TimeValue (Seconds (0.002))), true,
                         "Verify that we can actually set the attribute UpdateInterval");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Target", UintegerValue (100*modeSize)), true,
                         "Verify that we can actually set the attribute Target");
  queue->Initialize ();
  Enqueue (queue, pktSize, 300);
  Dequeue(queue,300);
  RemQueueDisc::Stats st = StaticCast<RemQueueDisc> (queue)->GetStats ();
  uint32_t test2 = st.unforcedDrop;
  NS_TEST_EXPECT_MSG_NE (test2, 0, "There should be some unforced drops");
  NS_TEST_EXPECT_MSG_EQ (st.qLimDrop, 0, "There should be zero forced drops");


  // test 3: same as test 2, but with higher QueueDelayReference
  queue = CreateObject<RemQueueDisc> ();
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Mode", mode), true,
                         "Verify that we can actually set the attribute Mode");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("QueueLimit", UintegerValue (qSize)), true,
                         "Verify that we can actually set the attribute QueueLimit");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Alpha", DoubleValue (0.1)), true,
                         "Verify that we can actually set the attribute Alpha");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Gamma", DoubleValue (0.001)), true,
                         "Verify that we can actually set the attribute Gamma");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("UpdateInterval", TimeValue (Seconds (0.002))), true,
                         "Verify that we can actually set the attribute UpdateInterval");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Target", UintegerValue (150*modeSize)), true,
                         "Verify that we can actually set the attribute Target");
  queue->Initialize ();
  Enqueue (queue, pktSize, 300);
  Dequeue(queue,300);
  st = StaticCast<RemQueueDisc> (queue)->GetStats ();
  uint32_t test3 = st.unforcedDrop;
  NS_TEST_EXPECT_MSG_LT (test3, test2, "Test 3 should have less unforced drops than test 2");
  NS_TEST_EXPECT_MSG_EQ (st.qLimDrop, 0, "There should be zero forced drops");


  }

void
RemQueueDiscTestCase::Enqueue (Ptr<RemQueueDisc> queue, uint32_t size, uint32_t nPkt)
{
  Address dest;
  for (uint32_t i = 0; i < nPkt; i++)
    {
      queue->Enqueue (Create<RemQueueDiscTestItem> (Create<Packet> (size), dest, 0));
    }
}

void
RemQueueDiscTestCase::Dequeue (Ptr<RemQueueDisc> queue, uint32_t nPkt)
{
  for (uint32_t i = 0; i < nPkt; i++)
    {
      Ptr<QueueDiscItem> item = queue->Dequeue ();
    }
}

void
RemQueueDiscTestCase::DoRun (void)
{
  RunRemTest (StringValue ("QUEUE_MODE_PACKETS"));
  RunRemTest (StringValue ("QUEUE_MODE_BYTES"));
  Simulator::Destroy ();
}

/**
 * \ingroup traffic-control-test
 * \ingroup tests
 *
 * \brief Rem Queue Disc Test Suite
 */
static class RemQueueDiscTestSuite : public TestSuite
{
public:
  RemQueueDiscTestSuite ()
    : TestSuite ("rem-queue-disc", UNIT)
  {
    AddTestCase (new RemQueueDiscTestCase (), TestCase::QUICK);
  }
} g_remQueueTestSuite; ///< the test suite
