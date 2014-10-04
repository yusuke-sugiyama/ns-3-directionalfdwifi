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

#include "source-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SourceTag)
  ;

TypeId 
SourceTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("SourceTag")
    .SetParent<Tag> ()
    .AddConstructor<SourceTag> ()
    ;
  return tid;
}
TypeId 
SourceTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t 
SourceTag::GetSerializedSize (void) const
{
  return 6;
}
void 
SourceTag::Serialize (TagBuffer i) const
{
  i.Write (m_address, 6);
}
void 
SourceTag::Deserialize (TagBuffer i)
{
  i.Read (m_address, 6);
  address.CopyFrom (m_address);
}

void
SourceTag::SetAddress (Mac48Address addr)
{
  address = addr;
  address.CopyTo (m_address);
}
Mac48Address
SourceTag::GetAddress (void) const
{
  return address;
}

void 
SourceTag::Print (std::ostream &os) const
{
  os << "address=" << address;
}
}

