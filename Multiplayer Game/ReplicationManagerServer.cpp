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

void ReplicationManagerServer::write(OutputMemoryStream& packet)
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
				packet << go->sprite_id;
				packet << go->tag;

				if (go->behaviour)
					packet << go->behaviour->type();
				else
					packet << BehaviourType::None;
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
	}
	//Cleaning commands map
	commands.clear();
}

