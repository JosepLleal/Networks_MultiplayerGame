#include "Networks.h"
#include "ReplicationManagerServer.h"

// TODO(you): World state replication lab session
void ReplicationManagerServer::create(uint32 networkID)
{
	commands[networkID] = ReplicationAction::Create;
}

void ReplicationManagerServer::update(uint32 networkID)
{
	commands[networkID] = ReplicationAction::Update;
}

void ReplicationManagerServer::destroy(uint32 networkID)
{
	commands[networkID] = ReplicationAction::Destroy;
}

void ReplicationManagerServer::write(OutputMemoryStream& packet, DeliveryDelegateRepManager* del)
{
	if (commands.size() == 0)
		return;

	for (std::map<uint32, ReplicationAction>::iterator it = commands.begin(); it != commands.end(); ++it)
	{
		packet << (*it).first;
		packet << (*it).second;

		if ((*it).second == ReplicationAction::Create)
		{
			GameObject* go = App->modLinkingContext->getNetworkGameObject((*it).first, true);

			if (go)
			{
				packet << go->position.x;
				packet << go->position.y;
				packet << go->size.x;
				packet << go->size.y;
				packet << go->angle;

				std::string tex = "";
				if (go->sprite)
				{
					tex = go->sprite->texture->filename;
					packet << tex;
					packet << go->sprite->order;
				}
				else
				{
					packet << tex;
					packet << 0;
				}
				
				
				if (go->behaviour)
					packet << go->behaviour->type();
				else
					packet << BehaviourType::None;
				
				packet << go->tag;
			}
			
		}
		else if ((*it).second == ReplicationAction::Update)
		{
			GameObject* go = App->modLinkingContext->getNetworkGameObject((*it).first, true);

			if (go)
			{
				packet << go->position.x;
				packet << go->position.y;
				packet << go->size.x;
				packet << go->size.y;
				packet << go->angle;
			}
			
		}
		//Delete action does not need any more information than action and networID
		ReplicationCommand command; 
		command.action = (*it).second; 
		command.networkId = (*it).first; 

		del->delegateCommands.push_back(command); 
	}
	del->RepManager = this;

	//Cleaning commands map
	commands.clear();
}

void DeliveryDelegateRepManager::onDeliverySuccess(DeliveryManager* deliveryManager)
{
}

void DeliveryDelegateRepManager::onDeliveryFailure(DeliveryManager* deliveryManager)
{
	for (auto command : delegateCommands)
	{
		switch (command.action)
		{
			case ReplicationAction::Create:
			{
				RepManager->create(command.networkId); 
			}
			case ReplicationAction::Update:
			{
				RepManager->update(command.networkId);
			}
			case ReplicationAction::Destroy:
			{
				RepManager->destroy(command.networkId);
			}
		}
	}
}
