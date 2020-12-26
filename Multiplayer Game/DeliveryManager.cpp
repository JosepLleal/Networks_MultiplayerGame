#include "Networks.h"
#include "DeliveryManager.h"
// TODO(you): Reliability on top of UDP lab session

DeliveryManager::~DeliveryManager()
{
	clear();
}

Delivery* DeliveryManager::writeSequenceNumber(OutputMemoryStream& packet)
{
	packet << nextOutSeqNumber;

	Delivery* del = new Delivery();
	del->startTime = Time.time;

	pendingDeliveries.emplace(nextOutSeqNumber++, del);

	return del;
}

bool DeliveryManager::processSequenceNumber(const InputMemoryStream& packet)
{
	uint32 seqNum;
	packet >> seqNum;

	if (seqNum >= nextExpSeqNumber)
	{
		nextExpSeqNumber = seqNum + 1;
		pendingAck.push_back(seqNum);
		return true;
	}

	return false;
}

bool DeliveryManager::hasSequenceNumberspendingAck() const
{
	return !pendingAck.empty();
}

void DeliveryManager::writeSequenceNumbersPendingAck(OutputMemoryStream& packet)
{
	if (hasSequenceNumberspendingAck())
	{
		for (uint32 seq_num : pendingAck)
		{
			packet << seq_num;
		}
	}
	pendingAck.clear();
}

void DeliveryManager::processAckdSequenceNumbers(const InputMemoryStream& packet)
{
	while (packet.RemainingByteCount() > 0)
	{
		uint32 ACK;
		packet >> ACK;

		if (pendingDeliveries.find(ACK) != pendingDeliveries.end())
		{
			if(pendingDeliveries[ACK]->delegate)
				pendingDeliveries[ACK]->delegate->onDeliverySuccess(this);
			pendingDeliveries.erase(ACK);
		}
	}
}

void DeliveryManager::processTimedOutPackets()
{
	std::vector<uint32> eraseList;
	for (std::map<uint32, Delivery*>::iterator it = pendingDeliveries.begin(); it != pendingDeliveries.end(); ++it)
	{
		if (Time.time - (*it).second->startTime > TIME_OUT_SEC)
		{	
			if((*it).second->delegate)
				(*it).second->delegate->onDeliveryFailure(this);
			eraseList.push_back((*it).first);
		}
	}

	for (uint32& i : eraseList)
	{
		pendingDeliveries.erase(i);
	}
}

void DeliveryManager::clear()
{
	//TODO
}