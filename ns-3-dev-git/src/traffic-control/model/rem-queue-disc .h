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
 * PORT NOTE: This code was ported from ns-2.36rc1 (queue/rem.h).
 * Most of the comments are also ported from the same.
 */

#ifndef REM_QUEUE_DISC_H
#define REM_QUEUE_DISC_H

#include <queue>
#include "ns3/packet.h"
#include "ns3/queue-disc.h"
#include "ns3/nstime.h"
#include "ns3/boolean.h"
#include "ns3/data-rate.h"
#include "ns3/timer.h"
#include "ns3/event-id.h"
#include "ns3/random-variable-stream.h"

//#define BURST_RESET_TIMEOUT 1.5

namespace ns3 {

class TraceContainer;
class UniformRandomVariable;

/**
 * \ingroup traffic-control
 *
 * \brief Implements REM Active Queue Management discipline
 */
class RemQueueDisc : public QueueDisc
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief RemQueueDisc Constructor
   */
  RemQueueDisc ();

  /**
   * \brief RemQueueDisc Destructor
   */
  virtual ~RemQueueDisc ();

  /**
   * \brief Stats
   */
  typedef struct
  {
    uint32_t unforcedDrop;      //!< Early probability drops: proactive
    uint32_t qLimDrop;       //!< Drops due to queue limit: reactive
  } Stats;

  /**
   * \brief Burst types
   */
  /*enum BurstStateT
  {
    NO_BURST,
    IN_BURST,
    IN_BURST_PROTECTING,
  };*/

  /**
   * \brief Set the operating mode of this queue.
   *
   * \param mode The operating mode of this queue.
   */
  void SetMode (Queue::QueueMode mode);

  /**
   * \brief Get the encapsulation mode of this queue.
   *
   * \returns The encapsulation mode of this queue.
   */
  Queue::QueueMode GetMode (void);

  /**
   * \brief Get the current value of the queue in bytes or packets.
   *
   * \returns The queue size in bytes or packets.
   */
  uint32_t GetQueueSize (void);

  /**
   * \brief Set the limit of the queue in bytes or packets.
   *
   * \param lim The limit in bytes or packets.
   */
  void SetQueueLimit (uint32_t lim);

  /**
   * \brief Get queue delay
   */
  //Time GetQueueDelay (void);

  /**
   * \brief Get REM statistics after running.
   *
   * \returns The drop statistics.
   */
  Stats GetStats ();

  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this model.  Return the number of streams (possibly zero) that
   * have been assigned.
   *
   * \param stream first stream index to use
   * \return the number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);

protected:
  /**
   * \brief Dispose of the object
   */
  virtual void DoDispose (void);

private:
  virtual bool DoEnqueue (Ptr<QueueDiscItem> item);
  virtual Ptr<QueueDiscItem> DoDequeue (void);
  virtual Ptr<const QueueDiscItem> DoPeek (void) const;
  virtual bool CheckConfig (void);

  /**
   * \brief Initialize the queue parameters.
   */
  virtual void InitializeParams (void);

  /**
   * \brief Check if a packet needs to be dropped due to probability drop
   * \param item queue item
   * \param qSize queue size
   * \returns 0 for no drop, 1 for drop
   */
  bool DropEarly (Ptr<QueueDiscItem> item, uint32_t qSize);

  /**
   * Periodically update the drop probability based on the delay samples:
   * not only the current delay sample but also the trend where the delay
   * is going, up or down
   */
  //void CalculateP ();
  /**
   * Compute the average input rate, the price and the marking probability
   */
  void RunUpdateRule ();
  Stats m_stats;                                //!< REM statistics

  // ** Variables supplied by user
  Queue::QueueMode m_mode;                      //!< Mode (bytes or packets)
  double m_inw;                                 //!< Queue weight given to cur q size sample
  double m_gamma;                               //!< Weight assigned to deviation of queue length from target and input rate from capacity
  double m_phi;                                 //!< Constant for calculation of probability
  uint32_t m_meanPktSize;                       //!< Average packet size in bytes
  Time m_tUpdate;                               //!< Time period after which RunUpdateRule () is called
  uint32_t m_bo;                                //!< Target queue length
  uint32_t m_queueLimit;                        //!< Queue limit in bytes / packets
  double m_ptc;                                  //!< Bandwidth in packets per second
  Time m_sUpdate;                               //!< Start time of the update timer
  
  Time m_qDelayRef;                             //!< Desired queue delay
  
  //Time m_maxBurst;                              //!< Maximum burst allowed before random early dropping kicks in
  //double m_a;                                   //!< Parameter to pie controller
  //double m_b;                                   //!< Parameter to pie controller
  //uint32_t m_dqThreshold;                       //!< Minimum queue size in bytes before dequeue rate is measured

  // ** Variables maintained by REM
  double m_vPl;                                 //!< Variable to compute the link price
  double m_vProb;                               //!< Probability of packet dropping
  double m_vIn;                                 //!< Variable used in computing the input rate
  double m_vAve;                                //!< Variable to store average input rate
  double m_vCount;                              //!< the number of bytes or packets arriving at the link during one update time interval
  
  
  //double m_dropProb;                            //!< Variable used in calculation of drop probability
  //Time m_qDelayOld;                             //!< Old value of queue delay
  //Time m_qDelay;                                //!< Current value of queue delay
  //Time m_burstAllowance;                        //!< Current max burst value in seconds that is allowed before random drops kick in
  //uint32_t m_burstReset;                        //!< Used to reset value of burst allowance
  //BurstStateT m_burstState;                     //!< Used to determine the current state of burst
  //bool m_inMeasurement;                         //!< Indicates whether we are in a measurement cycle
  //double m_avgDqRate;                           //!< Time averaged dequeue rate
  //double m_dqStart;                             //!< Start timestamp of current measurement cycle
  //uint32_t m_dqCount;                           //!< Number of bytes departed since current measurement cycle starts
  EventId m_rtrsEvent;                          //!< Event used to decide the decision of interval of drop probability calculation
  Ptr<UniformRandomVariable> m_uv;              //!< Rng stream
};

};   // namespace ns3

#endif

