#include "ReadyEvent.h"
#include "RakPeerInterface.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "RakAssert.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

int ReadyEvent::ReadyEventNodeComp( const int &key, ReadyEvent::ReadyEventNode * const &data )
{
	if (key < data->eventId)
		return -1;
	else if (key==data->eventId)
		return 0;
	else
		return 1;
}

ReadyEvent::ReadyEvent()
{
	channel=0;
	rakPeer=0;
}

ReadyEvent::~ReadyEvent()
{
	Clear();
}


bool ReadyEvent::SetEvent(int eventId, bool isReady)
{
	bool objectExists;
	unsigned eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists==false)
	{
		// Totally new event
		CreateEvent(eventId, isReady);
	}
	else
	{
		return SetEventByIndex(eventIndex, isReady);
	}
	return true;
}
bool ReadyEvent::DeleteEvent(int eventId)
{
	bool objectExists;
	unsigned eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists)
	{
		delete readyEventNodeList[eventIndex];
		readyEventNodeList.RemoveAtIndex(eventIndex);
		return true;
	}
	return false;
}
bool ReadyEvent::IsEventSet(int eventId)
{
	bool objectExists;
	unsigned eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists)
	{
		return readyEventNodeList[eventIndex]->eventSignaled;
	}
	return false;
}
bool ReadyEvent::IsEventCompletionProcessing(int eventId) const
{
	bool objectExists;
	unsigned eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists)
	{
		return IsLocked(eventIndex);
	}
	return false;
}
bool ReadyEvent::IsEventCompleted(int eventId) const
{
	bool objectExists;
	unsigned eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists)
	{
		return IsEventCompletedByIndex(eventIndex);
	}
	return false;
}

bool ReadyEvent::HasEvent(int eventId)
{
	return readyEventNodeList.HasData(eventId);
}

unsigned ReadyEvent::GetEventListSize(void) const
{
	return readyEventNodeList.Size();
}

int ReadyEvent::GetEventAtIndex(unsigned index) const
{
	return readyEventNodeList[index]->eventId;
}

bool ReadyEvent::AddToWaitList(int eventId, SystemAddress address)
{
	bool eventExists;
	unsigned eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &eventExists);
	if (eventExists==false)
		eventIndex=CreateEvent(eventId, false);

	// Don't do this, otherwise if we are trying to start a 3 player game, it will not allow the 3rd player to hit ready if the first two players have already done so
	//if (IsLocked(eventIndex))
	//	return false; // Not in the list, but event is already completed, or is starting to complete, and adding more waiters would fail this.

	unsigned numAdded=0;
	if (address==UNASSIGNED_SYSTEM_ADDRESS)
	{
		unsigned i;
		for (i=0; i < rakPeer->GetMaximumNumberOfPeers(); i++)
		{
			SystemAddress internalAddress = rakPeer->GetSystemAddressFromIndex(i);
			if (internalAddress!=UNASSIGNED_SYSTEM_ADDRESS)
			{
				numAdded+=AddToWaitListInternal(eventIndex, internalAddress);
			}
		}

		return numAdded>0;
	}
	else
	{
		numAdded=AddToWaitListInternal(eventIndex, address);
		return numAdded>0;
	}
}
bool ReadyEvent::RemoveFromWaitList(int eventId, SystemAddress address)
{
	bool eventExists;
	unsigned eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &eventExists);
	if (eventExists)
	{
		if (address==UNASSIGNED_SYSTEM_ADDRESS)
		{
			// Remove all waiters
			readyEventNodeList[eventIndex]->waitList.Clear();
		}
		else
		{
			bool systemExists;
			unsigned systemIndex = readyEventNodeList[eventIndex]->waitList.GetIndexFromKey(address, &systemExists);
			if (systemExists)
			{
				bool isReady = IsAllReadyByIndex(eventIndex);
				bool isCompleted = IsEventCompletedByIndex(eventIndex);

				readyEventNodeList[eventIndex]->waitList.RemoveAtIndex(systemIndex);

				// Send completed notification to all waiting systems
				if (isReady==false && IsAllReadyByIndex(eventIndex))
					BroadcastAllReady(eventIndex);

				if (isCompleted==false && IsEventCompletedByIndex(eventIndex))
					PushCompletionPacket(readyEventNodeList[eventIndex]->eventId);

				return true;
			}
		}		
	}
	return false;
}
bool ReadyEvent::IsInWaitList(int eventId, SystemAddress address)
{
	bool objectExists;
	unsigned readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists)
	{
		return readyEventNodeList[readyIndex]->waitList.HasData(address);
	}
	return false;
}

unsigned ReadyEvent::GetRemoteWaitListSize(int eventId) const
{
	bool objectExists;
	unsigned readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists)
	{
		return readyEventNodeList[readyIndex]->waitList.Size();
	}
	return 0;
}

SystemAddress ReadyEvent::GetFromWaitListAtIndex(int eventId, unsigned index) const
{
	bool objectExists;
	unsigned readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists)
	{
		return readyEventNodeList[readyIndex]->waitList[index];
	}
	return UNASSIGNED_SYSTEM_ADDRESS;
}

ReadyEventSystemStatus ReadyEvent::GetReadyStatus(int eventId, SystemAddress address)
{
	bool objectExists;
	unsigned readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists)
	{
		ReadyEventNode *ren = readyEventNodeList[readyIndex];
		bool isWaiting = ren->waitList.HasData(address);
		bool isReady = ren->readySystemList.HasData(address);
		bool isCompleted = ren->completedSystemList.HasData(address);
		if (isWaiting==false)
		{
			if (isReady==true)
				return RES_READY_NOT_WAITING;
			if (isCompleted==true)
				return RES_ALL_READY_NOT_WAITING;
			return RES_NOT_WAITING;
		}
		else
		{
			if (isReady==true)
				return RES_READY;
			if (isCompleted)
				return RES_ALL_READY;
			return RES_WAITING;
		}
	}

	return RES_UNKNOWN_EVENT;
}
void ReadyEvent::SetSendChannel(unsigned char newChannel)
{
	channel=newChannel;
}
void ReadyEvent::OnAttach(RakPeerInterface *peer)
{
	rakPeer=peer;
}
PluginReceiveResult ReadyEvent::OnReceive(RakPeerInterface *peer, Packet *packet)
{
	unsigned char packetIdentifier;
	packetIdentifier = ( unsigned char ) packet->data[ 0 ];

	switch (packetIdentifier)
	{
	case ID_READY_EVENT_SET:
		OnReadyEventSet(peer, packet);
		return RR_CONTINUE_PROCESSING;
	case ID_READY_EVENT_UNSET:
		OnReadyEventUnset(peer, packet);
		return RR_CONTINUE_PROCESSING;
	case ID_READY_EVENT_ALL_SET:
		return OnReadyEventCompletedSystem(peer, packet);
	case ID_READY_EVENT_QUERY:
		OnReadyEventQuery(peer, packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_DISCONNECTION_NOTIFICATION:
	case ID_CONNECTION_LOST:
		OnCloseConnection(peer, packet->systemAddress);
		return RR_CONTINUE_PROCESSING;	
	}

	return RR_CONTINUE_PROCESSING;
}
void ReadyEvent::OnReadyEventSet(RakPeerInterface *peer, Packet *packet)
{
	(void) peer;

	RakNet::BitStream incomingBitStream(packet->data, packet->length, false);
	incomingBitStream.IgnoreBits(8);
	int eventId;
	incomingBitStream.Read(eventId);
	bool objectExists;
	unsigned readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists)
	{
		ReadyEventNode *ren = readyEventNodeList[readyIndex];
		if (ren->completedSystemList.HasData(packet->systemAddress)==false)
		{
			bool systemExists;
			unsigned systemIndex = ren->readySystemList.GetIndexFromKey(packet->systemAddress, &systemExists);
			if (systemExists==false)
			{
				bool wasReady = IsAllReadyByIndex(readyIndex);
				ren->readySystemList.InsertAtIndex(packet->systemAddress, systemIndex);
				if (wasReady==false && IsAllReadyByIndex(readyIndex))
				{
					BroadcastAllReady(readyIndex);
				}
			}
		}
	}
	else
	{
		// NO IMPLICIT CREATION - Causes bugs from late packets
		/*
		// Create this event
		readyIndex = CreateEvent(eventId,false);
		readyEventNodeList[readyIndex]->readySystemList.Insert(packet->systemAddress, packet->systemAddress, true);
		AddToWaitList(eventId, packet->systemAddress);
		*/
	}
}
void ReadyEvent::OnReadyEventUnset(RakPeerInterface *peer, Packet *packet)
{
	(void) peer;

	RakNet::BitStream incomingBitStream(packet->data, packet->length, false);
	incomingBitStream.IgnoreBits(8);
	int eventId;
	incomingBitStream.Read(eventId);
	bool objectExists;
	unsigned readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists)
	{
		ReadyEventNode *ren = readyEventNodeList[readyIndex];
		unsigned completedSystemIndex;
		completedSystemIndex=ren->completedSystemList.GetIndexFromKey(packet->systemAddress, &objectExists);
		if (objectExists)
		{
			// From completed to ready for other systems
			// Move all to ready list
			unsigned i;
			for (i=0; i < ren->completedSystemList.Size(); i++)
				ren->readySystemList.Insert(ren->completedSystemList[i], ren->completedSystemList[i], true);
			ren->completedSystemList.Clear();
		}

		bool systemExists;
		unsigned systemIndex = ren->readySystemList.GetIndexFromKey(packet->systemAddress, &systemExists);
		if (systemExists==true)
		{
			// From ready to unready
			ren->readySystemList.RemoveAtIndex(systemIndex);
		}
	}
}
PluginReceiveResult ReadyEvent::OnReadyEventCompletedSystem(RakPeerInterface *peer, Packet *packet)
{
	(void) peer;

	RakNet::BitStream incomingBitStream(packet->data, packet->length, false);
	incomingBitStream.IgnoreBits(8);
	int eventId;
	incomingBitStream.Read(eventId);
	bool objectExists;
	unsigned readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists)
	{
		ReadyEventNode *ren = readyEventNodeList[readyIndex];
		ren->readySystemList.RemoveIfExists(packet->systemAddress);
		unsigned systemIndex = ren->completedSystemList.GetIndexFromKey(packet->systemAddress, &objectExists);
		if (objectExists==false)
		{
			bool wasCompleted = IsEventCompletedByIndex(readyIndex);
			ren->completedSystemList.InsertAtIndex(packet->systemAddress, systemIndex);

			// Check if now completed, and if so, broadcast this out
			if (wasCompleted==false && IsEventCompletedByIndex(readyIndex))
			{
				BroadcastAllReady(readyIndex);

				// Pass this message along to the user, equivalent to PushCompletionPacket();
				return RR_CONTINUE_PROCESSING;
			}
		}
	}
	else
	{

		// NO IMPLICIT CREATION - Causes bugs from late packets
		/*
		// Create this event
		readyIndex = CreateEvent(eventId,false);
		readyEventNodeList[readyIndex]->completedSystemList.Insert(packet->systemAddress, packet->systemAddress, true);
		AddToWaitList(eventId, packet->systemAddress);
		*/
	}

	// Absorb this message, since we are not totally done with this event
	return RR_STOP_PROCESSING_AND_DEALLOCATE;
}
void ReadyEvent::OnReadyEventQuery(RakPeerInterface *peer, Packet *packet)
{
	(void) peer;

	RakNet::BitStream incomingBitStream(packet->data, packet->length, false);
	incomingBitStream.IgnoreBits(8);
	int eventId;
	incomingBitStream.Read(eventId);
	bool objectExists;
	unsigned readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
	if (objectExists)
	{
		SendReadyUpdate(readyIndex, packet->systemAddress);

		if (IsAllReadyByIndex(readyIndex))
			SendAllReady(eventId, packet->systemAddress);
	}
}
void ReadyEvent::OnCloseConnection(RakPeerInterface *peer, SystemAddress systemAddress)
{
	(void) peer;
	(void) systemAddress;

	RemoveFromAllLists(systemAddress);
}
void ReadyEvent::OnShutdown(RakPeerInterface *peer)
{
	(void) peer;
	Clear();
}
bool ReadyEvent::AnyWaitersCompleted(unsigned eventIndex) const
{
	ReadyEventNode *ren = readyEventNodeList[eventIndex];
	// Return if all items in wait list are in the completed list
	unsigned waitListIndex, completedListIndex;
	completedListIndex=0;
	for (waitListIndex=0; waitListIndex < ren->waitList.Size(); waitListIndex++)
	{
		// Loop through the complete list and find the current address pointed to by the wait list
#ifdef _MSC_VER
#pragma warning( disable : 4127 ) // warning 4127: conditional expression is constant
#endif
		while (1)
		{
			if (completedListIndex >= ren->completedSystemList.Size())
				return false; // Finished with the complete list and found nothing

			if (ren->waitList[waitListIndex]==ren->completedSystemList[completedListIndex])
				return true; // Found a completed address

			// Went past the sort position
			if (ren->waitList[waitListIndex]>ren->completedSystemList[completedListIndex])
				break;

			// Check the next index
			completedListIndex++;
		}
	}

	// Finished with the waiting list and found nothing
	return false;
}

bool ReadyEvent::SetEventByIndex(int eventIndex, bool isReady)
{
	ReadyEventNode *ren = readyEventNodeList[eventIndex];
	if (ren->eventSignaled==isReady)
		return true; // Success - no change

	// Don't do this, otherwise if we are trying to start a 3 player game, it will not allow the 3rd player to hit ready if the first two players have already done so
	//		if (IsLocked(eventIndex) && isReady==false)
	//			return false; // We are finishing up the event, it's too late to change to unready now

	// Already ready
	//		if (IsEventCompletedByIndex(eventIndex))
	//			return false;

	ren->eventSignaled=isReady;

	// Send to all systems in the waiting list flag status update
	BroadcastReadyUpdate(eventIndex);

	// Can occur is isReady is now set to true where it was set to false before
	if (IsAllReadyByIndex(eventIndex))
	{
		// Send completed notification to all waiting systems
		BroadcastAllReady(eventIndex);
	}

	// Check if now completed, and if so, tell the user about it
	if (IsEventCompletedByIndex(eventIndex))
	{
		PushCompletionPacket(ren->eventId);
	}

	if (isReady==false)
	{
		// Move completed to ready
		// For each completed system, send ready to them
		unsigned i;
		for (i=0; i < ren->completedSystemList.Size(); i++)
		{
			SendReadyUpdate(eventIndex, ren->completedSystemList[i]);
			ren->readySystemList.Insert(ren->completedSystemList[i], ren->completedSystemList[i], true);
		}
		ren->completedSystemList.Clear();
	}

	return true;
}
#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
bool ReadyEvent::AllWaitersCompleted(unsigned eventIndex) const
{
	ReadyEventNode *ren = readyEventNodeList[eventIndex];

	// Return if all items in wait list are in the completed list
	unsigned waitListIndex, completedListIndex;
	completedListIndex=0;
	for (waitListIndex=0; waitListIndex < ren->waitList.Size(); waitListIndex++)
	{
		// Loop through the complete list and find the current address pointed to by the wait list
		while (1)
		{
			if (completedListIndex >= ren->completedSystemList.Size())
				return false; // Items in wait list that are not in the completed list

			if (ren->waitList[waitListIndex]==ren->completedSystemList[completedListIndex++])
				break;
		}
	}

	// All wait list elements in the completed list
	return true;

}
#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
bool ReadyEvent::AllWaitersReady(unsigned eventIndex) const
{
	ReadyEventNode *ren = readyEventNodeList[eventIndex];

	// Return if all items in wait list are in the ready list
	unsigned waitListIndex, readyListIndex;
	readyListIndex=0;
	for (waitListIndex=0; waitListIndex < ren->waitList.Size(); waitListIndex++)
	{
		// Loop through the complete list and find the current address pointed to by the wait list
		while (1)
		{
			if (readyListIndex >= ren->readySystemList.Size())
				return false; // Items in wait list that are not in the ready list

			if (ren->waitList[waitListIndex]==ren->readySystemList[readyListIndex++])
				break;
		}
	}

	// All wait list elements in the ready list
	return true;

}
bool ReadyEvent::IsEventCompletedByIndex(unsigned eventIndex) const
{
	ReadyEventNode *ren = readyEventNodeList[eventIndex];
	if (ren->eventSignaled==false)
		return false;
	if (ren->waitList.Size()==0)
		return false;
	return AllWaitersCompleted(eventIndex);
}

void ReadyEvent::Clear(void)
{
	unsigned i;
	for (i=0; i < readyEventNodeList.Size(); i++)
	{
		delete readyEventNodeList[i];
	}
	readyEventNodeList.Clear();
}

unsigned ReadyEvent::CreateEvent(int eventId, bool isReady)
{
	ReadyEventNode *ren = new ReadyEventNode;
	ren->eventId=eventId;
	ren->eventSignaled=isReady;
	return readyEventNodeList.Insert(eventId, ren, true);
}
void ReadyEvent::SendAllReady(unsigned eventId, SystemAddress address)
{
	RakNet::BitStream bs;
	bs.Write((MessageID)ID_READY_EVENT_ALL_SET);
	bs.Write(eventId);
	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, channel, address, false);
}
void ReadyEvent::BroadcastAllReady(unsigned eventIndex)
{
	ReadyEventNode *ren = readyEventNodeList[eventIndex];
	unsigned i;
	RakNet::BitStream bs;
	bs.Write((MessageID)ID_READY_EVENT_ALL_SET);
	bs.Write(ren->eventId);
	for (i=0; i < ren->waitList.Size(); i++)
	{
		rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, channel, ren->waitList[i], false);
	}
}
void ReadyEvent::SendReadyUpdate(unsigned eventIndex, SystemAddress address)
{
	ReadyEventNode *ren = readyEventNodeList[eventIndex];
	RakNet::BitStream bs;
	// I do this rather than write true or false, so users that do not use BitStreams can still read the data
	if (ren->eventSignaled)
		bs.Write((MessageID)ID_READY_EVENT_SET);
	else
		bs.Write((MessageID)ID_READY_EVENT_UNSET);
	bs.Write(ren->eventId);
	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, channel, address, false);
}
void ReadyEvent::BroadcastReadyUpdate(unsigned eventIndex)
{
	ReadyEventNode *ren = readyEventNodeList[eventIndex];
	unsigned i;
	RakNet::BitStream bs;
	// I do this rather than write true or false, so users that do not use BitStreams can still read the data
	if (ren->eventSignaled)
		bs.Write((MessageID)ID_READY_EVENT_SET);
	else
		bs.Write((MessageID)ID_READY_EVENT_UNSET);
	bs.Write(ren->eventId);
	for (i=0; i < ren->waitList.Size(); i++)
	{
		rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, channel, ren->waitList[i], false);
	}
}
void ReadyEvent::SendReadyStateQuery(unsigned eventId, SystemAddress address)
{
	RakNet::BitStream bs;
	bs.Write((MessageID)ID_READY_EVENT_QUERY);
	bs.Write(eventId);
	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, channel, address, false);
}
void ReadyEvent::RemoveFromAllLists(SystemAddress address)
{
	unsigned eventIndex;
	for (eventIndex=0; eventIndex < readyEventNodeList.Size(); eventIndex++)
	{
		bool systemExists;
		unsigned systemIndex = readyEventNodeList[eventIndex]->waitList.GetIndexFromKey(address, &systemExists);
		if (systemExists)
		{
			bool isReady = IsAllReadyByIndex(eventIndex);
			bool isCompleted = IsEventCompletedByIndex(eventIndex);

			readyEventNodeList[eventIndex]->waitList.RemoveAtIndex(systemIndex);

			// Send completed notification to all waiting systems
			if (isReady==false && IsAllReadyByIndex(eventIndex))
				BroadcastAllReady(eventIndex);

			if (isCompleted==false && IsEventCompletedByIndex(eventIndex))
				PushCompletionPacket(readyEventNodeList[eventIndex]->eventId);
		}
	
		systemIndex = readyEventNodeList[eventIndex]->readySystemList.GetIndexFromKey(address, &systemExists);
		if (systemExists)
		{
			readyEventNodeList[eventIndex]->readySystemList.RemoveAtIndex(systemIndex);
		}

		systemIndex = readyEventNodeList[eventIndex]->completedSystemList.GetIndexFromKey(address, &systemExists);
		if (systemExists)
		{
			readyEventNodeList[eventIndex]->completedSystemList.RemoveAtIndex(systemIndex);
		}
	}
}
bool ReadyEvent::AddToWaitListInternal(unsigned eventIndex, SystemAddress address)
{
	// Add to the list		
	ReadyEventNode *ren = readyEventNodeList[eventIndex];
	bool systemExists;
	unsigned systemIndex = ren->waitList.GetIndexFromKey(address, &systemExists);

	if (systemExists==false)
	{
		// Add this system to the wait list
		ren->waitList.InsertAtIndex(address, systemIndex);

		// Update the new system with our ready status
//		if (IsEventCompleted(eventIndex))
//		{
			// No longer ready, broadcast this to all systems that think we are completed
//			SetEventByIndex(eventIndex, false);
//		}

//		if (IsAllReadyByIndex(eventIndex))
//		{
			// Send completed notification to all waiting systems
//			SendAllReady(eventIndex, address);
//		}

		// Update the new system with our ready status
		SendReadyUpdate(eventIndex, address);

		// Ask this new system for their status, so we can add them to the completed or ready list.
		// If no answer (such as they don't have the plugin)
		// They just stay in the waiting list
		SendReadyStateQuery(ren->eventId, address);

		// Move completed to ready
		unsigned i;
		for (i=0; i < ren->completedSystemList.Size(); i++)
			ren->readySystemList.Insert(ren->completedSystemList[i], ren->completedSystemList[i], true);
		ren->completedSystemList.Clear();
		
		return true;
	}
	else
	{
		// Already in the list
		return false;
	}
}
void ReadyEvent::PushCompletionPacket(unsigned eventId)
{
	// Pass a packet to the user that we are now completed, as setting ourselves to signaled was the last thing being waited on
	Packet *p = rakPeer->AllocatePacket(sizeof(MessageID)+sizeof(int));
	RakNet::BitStream bs(p->data, sizeof(MessageID)+sizeof(int), false);
	bs.SetWriteOffset(0);
	bs.Write((MessageID)ID_READY_EVENT_ALL_SET);
	bs.Write(eventId);
	rakPeer->PushBackPacket(p, false);
}
bool ReadyEvent::IsLocked(unsigned eventIndex) const
{
	return AnyWaitersCompleted(eventIndex) || IsAllReadyByIndex(eventIndex);
}
bool ReadyEvent::IsAllReadyByIndex(unsigned eventIndex) const
{
	ReadyEventNode *ren = readyEventNodeList[eventIndex];
	return ren->eventSignaled && ren->waitList.Size()>0 && AllWaitersReady(eventIndex);
}
#ifdef _MSC_VER
#pragma warning( pop )
#endif
