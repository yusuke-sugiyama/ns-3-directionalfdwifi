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
#include "secondary-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SecondaryTag)
  ;

TypeId 
SecondaryTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("SecondaryTag")
    .SetParent<Tag> ()
    .AddConstructor<SecondaryTag> ()
    .AddAttribute ("dataSize",
                   "The size of primary transmission.",
                   EmptyAttributeValue (),
                   MakeUintegerAccessor (&SecondaryTag::GetDataSize),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}
TypeId 
SecondaryTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t 
SecondaryTag::GetSerializedSize (void) const
{
  return sizeof (uint32_t);
}
void 
SecondaryTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_dataSize);
}
void 
SecondaryTag::Deserialize (TagBuffer i)
{
  m_dataSize = i.ReadU32();
}

void
SecondaryTag::SetDataSize (uint32_t dataSize)
{
  m_dataSize = dataSize;
}
uint32_t
SecondaryTag::GetDataSize (void) const
{
  return m_dataSize;
}

void 
SecondaryTag::Print (std::ostream &os) const
{
  os << "busytone=" << m_dataSize;
}
}

