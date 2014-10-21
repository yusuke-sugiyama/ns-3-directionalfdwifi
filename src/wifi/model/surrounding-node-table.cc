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
#include "surrounding-node-table.h"
#include "ns3/random-variable-stream.h"
#include "ns3/mac48-address.h"
#include "ns3/double.h"

// NS_OBJECT_ENSURE_REGISTERED (SurroundingNodeTable);
NS_LOG_COMPONENT_DEFINE ("SurroundingNodeTable");

namespace ns3 {

TypeId
SurroundingNodeTable::GetTypeId (void)
{
  static TypeId tid = TypeId ("SurroundingNodeTable")
    .SetParent<Object> ()
    .AddConstructor<SurroundingNodeTable> ()
    ;
  return tid;
}

SurroundingNodeTable::SurroundingNodeTable ()
{
  m_random = CreateObject<UniformRandomVariable> ();
}

SurroundingNodeTable::~SurroundingNodeTable ()
{
  InitItem();
}

Mac48Address
SurroundingNodeTable::SelectSecondaryTransmissionNode()
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
SurroundingNodeTable::GetRandom(double min, double max)
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
SurroundingNodeTable::AddItem(Mac48Address address, bool nextHop, bool hasFrames)
{
  NS_LOG_FUNCTION(this << address << nextHop << hasFrames);
  Items.push_back(new SurroundingNodeItem(address, nextHop, hasFrames));
}
  
void
SurroundingNodeTable::InitItem()
{
  Items.clear();
}
bool
SurroundingNodeTable::IsExistsAddress(Mac48Address address)
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
SurroundingNodeTable::UpdateNextHop(Mac48Address address, bool nextHop)
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
SurroundingNodeTable::UpdateHasFrames(Mac48Address address, bool hasFrames)
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
SurroundingNodeTable::UpdateTable(Mac48Address address, bool nextHop, bool hasFrames)
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
SurroundingNodeTable::DeleteItemByAddress(Mac48Address address)
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
  
SurroundingNodeItem::SurroundingNodeItem (Mac48Address address, bool nextHop, bool hasFrames)
{
  m_address = address;
  m_nextHop = nextHop;
  m_hasFrames = hasFrames;
}

SurroundingNodeItem::~SurroundingNodeItem ()
{
}

Mac48Address
SurroundingNodeItem::GetAddress()
{
  return m_address;
}

bool
SurroundingNodeItem::IsNextHop()
{
  return m_nextHop;
}

bool
SurroundingNodeItem::IsHasFrames()
{
  return m_hasFrames;
}

void
SurroundingNodeItem::SetNextHop(bool nextHop)
{
  m_nextHop = nextHop;
}

void
SurroundingNodeItem::SetHasFrames(bool hasFrames)
{
  m_hasFrames = hasFrames;
}

SurroundingNodeItem*
SurroundingNodeItem::Copy ()
{
  return new SurroundingNodeItem(GetAddress(), IsNextHop(), IsHasFrames());
}
  
} // namespace ns3
