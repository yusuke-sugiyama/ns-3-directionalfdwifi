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

#include "busytone-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BusytoneTag)
  ;

TypeId 
BusytoneTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("BusytoneTag")
    .SetParent<Tag> ()
    .AddConstructor<BusytoneTag> ()
    .AddAttribute ("dataSize",
                   "The size of busytone.",
                   EmptyAttributeValue (),
                   MakeUintegerAccessor (&BusytoneTag::GetDataSize),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}
TypeId 
BusytoneTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t 
BusytoneTag::GetSerializedSize (void) const
{
  return sizeof (uint32_t);
}
void 
BusytoneTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_dataSize);
}
void 
BusytoneTag::Deserialize (TagBuffer i)
{
  m_dataSize = i.ReadU32();
}

void
BusytoneTag::SetDataSize (uint32_t dataSize)
{
  m_dataSize = dataSize;
}
uint32_t
BusytoneTag::GetDataSize (void) const
{
  return m_dataSize;
}

void 
BusytoneTag::Print (std::ostream &os) const
{
  os << "busytone=" << m_dataSize;
}
}

