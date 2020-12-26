#pragma once

// TODO(you): World state replication lab session
#include <map>
class DeliveryDelegateRepManager; 

enum class ReplicationAction
{ None, Create, Update, Destroy };

struct ReplicationCommand
{
	ReplicationAction action;
	uint32 networkId;
};

class ReplicationManagerServer
{
public:
	void create(uint32 networkID);
	void update(uint32 networkID);
	void destroy(uint32 networkID);

	void write(OutputMemoryStream& packet, DeliveryDelegateRepManager* del);

	std::map<uint32, ReplicationAction> commands;
};

class DeliveryDelegateRepManager : public DeliveryDelegate
{
public: 
	void onDeliverySuccess(DeliveryManager* deliveryManager) override;
	void onDeliveryFailure(DeliveryManager* deliveryManager) override;

	std::vector<ReplicationCommand> delegateCommands; 

	ReplicationManagerServer* RepManager; 

};