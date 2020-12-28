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
			int is_dummy = 0;
			packet >> is_dummy;

			GameObject* go = App->modLinkingContext->getNetworkGameObject(networkId);
			bool dummy = false;

			if (go)
				dummy = true;

			go = Instantiate();

			if (dummy == false)
			{
				GameObject* go_del = App->modLinkingContext->getNetworkGameObject(networkId, false);
				if (go_del)
				{
					App->modLinkingContext->unregisterNetworkGameObject(go_del);
					Destroy(go_del);
				}
				
				App->modLinkingContext->registerNetworkGameObjectWithNetworkId(go, networkId);
			}

			//--------------------------------------------------------------------------------------------
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
				else if (tex == "spacecraft2.png")
					go->sprite->texture = App->modResources->spacecraft2;
				else if (tex == "spacecraft3.png")
					go->sprite->texture = App->modResources->spacecraft3;
				else if (tex == "laser.png")
					go->sprite->texture = App->modResources->laser;
				else if (tex == "healthPack.png")
					go->sprite->texture = App->modResources->healthpack;
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
			else if(type == BehaviourType::HealthPack)
				go->behaviour = App->modBehaviour->addHealthpack(go);

			packet >> go->tag;

			if (go->behaviour)
			{ //initial positions
				go->interpolation.InitialPos = go->position;
				go->interpolation.InitialAngle = go->angle;
			}

			//--------------------------------------------------------------------------------------------

			if (dummy)
			{
				Destroy(go);
			}

			if (is_dummy == 1)
			{
				App->modLinkingContext->unregisterNetworkGameObject(go);
				Destroy(go);
			}

		}
		else if (action == ReplicationAction::Update)
		{
			GameObject* go = App->modLinkingContext->getNetworkGameObject(networkId);

			// Initial state
			go->interpolation.InitialPos = go->position;
			go->interpolation.InitialAngle = go->angle;

			packet >> go->position.x;
			packet >> go->position.y;
			packet >> go->size.x;
			packet >> go->size.y;
			packet >> go->angle;

			//Final state
			go->interpolation.FinalPos = go->position;
			go->interpolation.FinalAngle = go->angle;

			if(!App->modNetClient->IsClientID(networkId)) // if go is different from client spaceship
				go->interpolation.Seconds = 0.0; //reset interpolation time

			if (go->behaviour)
				go->behaviour->read(packet);
			
		}
		else if (action == ReplicationAction::Destroy)
		{
			GameObject* go = App->modLinkingContext->getNetworkGameObject(networkId);
			if (go)
			{
				if( go->behaviour && go->behaviour->type() == BehaviourType::HealthPack)
					App->modSound->playAudioClip(App->modResources->audioClipHealthPickUp);

				App->modLinkingContext->unregisterNetworkGameObject(go);
				Destroy(go);
			}
		}
	}
}
