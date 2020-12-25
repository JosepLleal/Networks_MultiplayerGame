#include "Networks.h"
#include "ReplicationManagerClient.h"

// TODO(you): World state replication lab session

void ReplicationManagerClient::read(const InputMemoryStream& packet)
{
	while (packet.RemainingByteCount() > 0)
	{
		uint32 networkId;
		packet >> networkId;
		ReplicationAction action = ReplicationAction::None;
		packet >> action;

		if (action == ReplicationAction::Create)
		{
			if (App->modLinkingContext->getNetworkGameObject(networkId) == nullptr)
			{
				GameObject* go = Instantiate();
				App->modLinkingContext->registerNetworkGameObjectWithNetworkId(go, networkId);

				packet >> go->position.x;
				packet >> go->position.y;
				packet >> go->size.x;
				packet >> go->size.y;
				packet >> go->angle;
				packet >> go->sprite_id;
				packet >> go->tag;

				BehaviourType type;
				packet >> type;

				if (type == BehaviourType::Spaceship)
				{
					go->behaviour = App->modBehaviour->addSpaceship(go);
					go->sprite = App->modRender->addSprite(go);
					go->sprite->order = 5;

					if (go->sprite_id == 0) {
						go->sprite->texture = App->modResources->spacecraft1;
					}
					else if (go->sprite_id == 1) {
						go->sprite->texture = App->modResources->spacecraft2;
					}
					else if (go->sprite_id == 2) {
						go->sprite->texture = App->modResources->spacecraft3;
					}
				}
				else if (type == BehaviourType::Laser)
				{
					go->behaviour = App->modBehaviour->addLaser(go);
					go->sprite = App->modRender->addSprite(go);
					go->sprite->order = 4;
					go->sprite->texture = App->modResources->laser;
				}
			}
			else
			{
				//Destroy Go and create the new one ?
			}
		}
		else if (action == ReplicationAction::Update)
		{
			GameObject* go = App->modLinkingContext->getNetworkGameObject(networkId);

			if (go)
			{
				packet >> go->position.x;
				packet >> go->position.y;
				packet >> go->size.x;
				packet >> go->size.y;
				packet >> go->angle;
			}
			
		}
		else if (action == ReplicationAction::Destroy)
		{
			GameObject* go = App->modLinkingContext->getNetworkGameObject(networkId);
			if (go)
			{
				App->modLinkingContext->unregisterNetworkGameObject(go);
				Destroy(go);
			}
		}
	}
}
