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

				std::string tex;
				packet >> tex;
				go->sprite = App->modRender->addSprite(go);
				packet >> go->sprite->order;
				if (go->sprite)
				{
					if (tex == "spacecraft1.png")
						go->sprite->texture = App->modResources->spacecraft1;
					else if(tex == "spacecraft2.png")
						go->sprite->texture = App->modResources->spacecraft2;
					else if (tex == "spacecraft3.png")
						go->sprite->texture = App->modResources->spacecraft3;
					else if (tex == "laser.png")
						go->sprite->texture = App->modResources->laser;
					else if (tex == "explosion1.png")
					{
						go->sprite->texture = App->modResources->explosion1;
						//animated sprite
						go->animation = App->modRender->addAnimation(go);
						go->animation->clip = App->modResources->explosionClip;
						//explosion sound
						App->modSound->playAudioClip(App->modResources->audioClipExplosion);
					}
				}

				BehaviourType type;
				packet >> type;

				if (type == BehaviourType::Spaceship)
					go->behaviour = App->modBehaviour->addSpaceship(go);
				else if (type == BehaviourType::Laser)
					go->behaviour = App->modBehaviour->addLaser(go);


				packet >> go->tag;

				
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
