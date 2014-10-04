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

#include "ns3/header.h"
#include "ns3/packet.h"
#include "ns3/mac48-address.h"

#ifndef SOURCE_TAG_H
#define SOURCE_TAG_H

namespace ns3 {

class Tag;

class SourceTag : public Tag {
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  void SetAddress (Mac48Address address);
  Mac48Address GetAddress (void) const;

  void Print (std::ostream &os) const;

private:
  Mac48Address address;
  uint8_t m_address[6];

  // end class SourceTag
};

} // namespace ns3

#endif /* SOURCE_TAG_H */
