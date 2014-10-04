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

#include <vector>
#include "ns3/log.h"
#include "surround-node-table.h"
#include "ns3/random-variable-stream.h"
#include "ns3/mac48-address.h"
#include "ns3/double.h"

// NS_OBJECT_ENSURE_REGISTERED (SurroundNodeTable);
NS_LOG_COMPONENT_DEFINE ("SurroundNodeTable");

namespace ns3 {

TypeId
SurroundNodeTable::GetTypeId (void)
{
  static TypeId tid = TypeId ("SurroundNodeTable")
    .SetParent<Object> ()
    .AddConstructor<SurroundNodeTable> ()
    ;
  return tid;
}

SurroundNodeTable::SurroundNodeTable ()
{
  m_random = CreateObject<UniformRandomVariable> ();
}

SurroundNodeTable::~SurroundNodeTable ()
{
  InitItem();
}

Mac48Address
SurroundNodeTable::SelectSecondaryTransmissionNode()
{
  NS_LOG_FUNCTION(this);
  std::vector<uint32_t> ItemPriorities[4];
  for(uint32_t i = 0; i < Items.size(); i++){
    NS_LOG_DEBUG ("address:"   << Items[i]->GetAddress() <<
		  " nexthop:"  << Items[i]->IsNextHop() <<
		  " hasframes:" << Items[i]->IsHasFrames());

    if(!Items[i]->IsNextHop() && Items[i]->IsHasFrames())
      {
	ItemPriorities[FIRST].push_back(i);
      }
    else if(Items[i]->IsNextHop() && Items[i]->IsHasFrames())
      {
	ItemPriorities[SECOND].push_back(i);
      }
    else if(Items[i]->IsNextHop() && !Items[i]->IsHasFrames())
      {
	ItemPriorities[THIRD].push_back(i);
      }
    else
      {
	ItemPriorities[FOURTH].push_back(i);
      }
  }

  for(int i = 0; i < 4; i++)
    {
      if(ItemPriorities[i].size() > 0)
	{
	  double randomValue = GetRandom(0, ItemPriorities[i].size());
	  uint32_t num = ItemPriorities[i][randomValue];
	  NS_LOG_DEBUG ("address:" << Items[num]->GetAddress());
	  return Items[num]->GetAddress();
	}
    }
  
  // broadcast
  Mac48Address addr;
  return addr.GetBroadcast();
}
uint32_t
SurroundNodeTable::GetRandom(double min, double max)
{
  if(min >= max)
    {
      NS_LOG_INFO(this << "min >= max");
      return 0;
    }
  
  m_random->SetAttribute ("Min", DoubleValue (0));
  m_random->SetAttribute ("Max", DoubleValue ());
  double randomValue = m_random->GetValue ();
  NS_LOG_INFO("randomValue" << randomValue);
  return (uint32_t)randomValue;
}
  
void
SurroundNodeTable::AddItem(Mac48Address address, bool nextHop, bool hasFrames)
{
  NS_LOG_FUNCTION(this << address << nextHop << hasFrames);
  Items.push_back(new SurroundNodeItem(address, nextHop, hasFrames));
}
  
void
SurroundNodeTable::InitItem()
{
  Items.clear();
}
bool
SurroundNodeTable::IsExistsAddress(Mac48Address address)
{
  NS_LOG_FUNCTION(this);
  for(unsigned int i = 0; i < Items.size(); i++)
    {
      NS_LOG_DEBUG(this << Items[i]->GetAddress());
      if(Items[i]->GetAddress() == address)
	{
	  return true;
	}
    }
  return false;
}
  
void
SurroundNodeTable::UpdateNextHop(Mac48Address address, bool nextHop)
{
  NS_LOG_FUNCTION(this);
  for(uint32_t i = 0; i < Items.size(); i++)
    {
      if(Items[i]->GetAddress() == address)
	{
	  Items[i]->SetNextHop(nextHop);
	}
    }
}
  
void
SurroundNodeTable::UpdateHasFrames(Mac48Address address, bool hasFrames)
{
  NS_LOG_FUNCTION(this);
  for(unsigned int i = 0; i < Items.size(); i++)
    {
      if(Items[i]->GetAddress() == address)
	{
	  Items[i]->SetHasFrames(hasFrames);
	}
    }
}

void
SurroundNodeTable::UpdateTable(Mac48Address address, bool nextHop, bool hasFrames)
{

  NS_LOG_FUNCTION(this << address << nextHop << hasFrames);
  if(IsExistsAddress(address))
    {
      UpdateNextHop(address, nextHop);
      UpdateHasFrames(address, hasFrames);
    }
  else
    {
      AddItem(address, nextHop, hasFrames);
    }
}

void
SurroundNodeTable::DeleteItemByAddress(Mac48Address address)
{

  NS_LOG_FUNCTION(this << address);

  for(unsigned int i = 0; i < Items.size(); i++)
    {
      NS_LOG_DEBUG(this << Items[i]->GetAddress());
      if(Items[i]->GetAddress() == address)
	{
	  // delete
	  Items.erase(Items.begin() + i);
	  break;
	}
    }
}
  
SurroundNodeItem::SurroundNodeItem (Mac48Address address, bool nextHop, bool hasFrames)
{
  m_address = address;
  m_nextHop = nextHop;
  m_hasFrames = hasFrames;
}

SurroundNodeItem::~SurroundNodeItem ()
{
}

Mac48Address
SurroundNodeItem::GetAddress()
{
  return m_address;
}

bool
SurroundNodeItem::IsNextHop()
{
  return m_nextHop;
}

bool
SurroundNodeItem::IsHasFrames()
{
  return m_hasFrames;
}

void
SurroundNodeItem::SetNextHop(bool nextHop)
{
  m_nextHop = nextHop;
}

void
SurroundNodeItem::SetHasFrames(bool hasFrames)
{
  m_hasFrames = hasFrames;
}

SurroundNodeItem*
SurroundNodeItem::Copy ()
{
  return new SurroundNodeItem(GetAddress(), IsNextHop(), IsHasFrames());
}
  
} // namespace ns3
