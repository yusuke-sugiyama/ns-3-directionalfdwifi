/* -*- Mode:C++; -*- */
/*
 * Copyright (c) 2014 Yusuke Sugiyama
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
 * Foundation, Inc., Saruwatari Lab, Shizuoka University, Japan
 *
 * Author: Yusuke Sugiyama <sugiyama@aurum.cs.inf.shizuoka.ac.jp>
 */

#ifndef SURROUND_NODE_TABLE_H
#define SURROUND_NODE_TABLE_H

#include <stdint.h>
#include <vector>
#include "ns3/object.h"
#include "ns3/mac48-address.h"
#include "ns3/random-variable-stream.h"

namespace ns3 {

class SurroundNodeItem
{
public:
  ~SurroundNodeItem();
  SurroundNodeItem (Mac48Address address, bool nextHop, bool hasFrames);
  Mac48Address GetAddress (void);
  bool IsNextHop  (void);
  bool IsHasFrames (void);
  void SetNextHop  (bool nextHop);
  void SetHasFrames (bool hasFrames);
  SurroundNodeItem* Copy ();
  
private:
  Mac48Address m_address;
  bool m_nextHop;
  bool m_hasFrames;
};

class SurroundNodeTable : public Object
{
public:
  static TypeId GetTypeId (void);
  Mac48Address SelectSecondaryTransmissionNode();
  void AddItem(Mac48Address address, bool nextHop, bool moreData);
  void InitItem();
  bool IsExistsAddress(Mac48Address address);
  void DeleteItemByAddress(Mac48Address address);
  void UpdateNextHop(Mac48Address address, bool nextHop);
  void UpdateHasFrames(Mac48Address address, bool hasFrames);
  void UpdateTable(Mac48Address address, bool nextHop, bool hasFrames);
  uint32_t GetRandom(double min, double max);

  SurroundNodeTable();
  ~SurroundNodeTable();
private:
  enum PRIORITIES {
    FIRST  = 0,
    SECOND = 1,
    THIRD  = 2,
    FOURTH = 3
  };
  std::vector<SurroundNodeItem*> Items;
  Ptr<UniformRandomVariable> m_random;  //!< Provides uniform random variables.
};

} // namespace ns3


#endif /* SURROUND_NODE_TABLE_H */
