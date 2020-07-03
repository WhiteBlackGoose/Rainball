#pragma once
#include <MxEngine.h>
#include "Physics.h"

namespace Rainball
{
	using namespace MxEngine;

	class Player;

	class Waver
	{
		friend Player;
	private:
		Array2D<SurfaceScript> bumpers;
		InstanceFactory::Handle waves;
		int sizeX, sizeZ;
		float SCALE;
	public:
		Waver(int sizeX, int sizeZ, float scale) : sizeX(sizeX), sizeZ(sizeZ), SCALE(scale)
		{
			auto wave = MxObject::Create();
			wave->Name = "Waves";
			waves = wave->AddComponent<InstanceFactory>();

			auto instancer = MxObject::Create();
			instancer->AddComponent<MeshSource>()->Mesh = Primitives::CreateCube();
			instancer->AddComponent<MeshRenderer>()->GetMaterial()->Reflection = 1;
			auto factory = instancer->AddComponent<InstanceFactory>();
			instancer->Name = "Surface";

			bumpers.resize(sizeX, sizeZ, SurfaceScript(0, SCALE));
			for (int x = 0; x < sizeX; x++)
				for (int z = 0; z < sizeZ; z++)
				{
					bumpers[x][z] = SurfaceScript(x + z, SCALE);
					bumpers[x][z].inst = factory->MakeInstance();
					bumpers[x][z].inst->Transform.SetPosition({ SCALE * x, 0, SCALE * z });
					bumpers[x][z].inst->Transform.SetScale(Vector3(SCALE, SCALE, SCALE));
				}
		}
		void Update(float delta, float strength)
		{
			for (int x = 0; x < sizeX; x++)
				for (int z = 0; z < sizeZ; z++)
					bumpers[x][z].OnUpdate(delta);
		}
		std::tuple<int, int> GetSurfaceId(const Vector3& pos)
		{
			auto x = (int)round(pos.x / SCALE);
			auto z = (int)round(pos.z / SCALE);
			return { x, z };
		}

		/*
		Starts a new script for waving
		*/
		void Wave(const int x, const int z, const float strength)
		{
			auto wInst = waves->MakeInstance();
			auto pos = bumpers[x][z].inst->Transform.GetPosition();
			wInst->Transform.SetPosition(pos);
			wInst->AddComponent<Behaviour>(WaveScript(
				strength
				, bumpers));

		}

		/*
		Collects scripts generating waves if a wave is too far and destroyes them
		*/
		void GarbageCollect()
		{
			for (auto& inst : waves->GetInstances())
				if (inst->GetComponent<Behaviour>()->GetBehaviour<WaveScript>().useless)
				{
					MxObject::Destroy(inst);
					MX_DBG("Wave #" + (MxString)inst.GetUUID() + " removed");
				}
		}
	};

	class Player
	{
		friend Waver;
		InstanceFactory::Handle spheres;
	public:
		/*
		Shoots a sphere from position along with direction with the speed of speed
		*/
		void Shoot(const Vector3& position, const Vector3& direction, const float speed)
		{
			auto inst = spheres->MakeInstance();
			inst->Transform.SetPosition(position);

			inst->AddComponent<Behaviour>(ShotBehaviour(
				direction * speed,
				position
			));
		}
		
		/*
		Removes useless shots
		and causes waves if one intersects a surface
		*/
		void CheckReaction(Waver& destination)
		{
			for (auto& inst : spheres->GetInstances())
			{
				if (inst->Transform.GetPosition().y < -10)
				{
					MxObject::Destroy(inst);
					continue;
				}
				
				//auto [x, z] = destination.GetSurfaceId(inst->Transform.GetPosition());
				auto sx = destination.GetSurfaceId(inst->Transform.GetPosition());
				auto x = std::get<0>(sx);
				auto z = std::get<1>(sx);

				if (x < 0 || z < 0 || x >= destination.bumpers.width() || z >= destination.bumpers.height())
					return;

				if (inst->Transform.GetPosition().y < 
					destination.bumpers[x][z].inst->Transform.GetPosition().y + destination.bumpers[x][z].inst->Transform.GetScale().y / 2)
				{
					destination.Wave(x, z,
						inst->GetComponent<Behaviour>()->GetBehaviour<ShotBehaviour>().GetSpeed());
					MxObject::Destroy(inst);
				}
			}
		}

		/*
		Creates a factory for shots
		*/
		Player()
		{
			auto sph = MxObject::Create();
			sph->AddComponent<MeshSource>()->Mesh = Primitives::CreateSphere(100);
			sph->AddComponent<MeshRenderer>();
			sph->Transform.SetScale(0.6);
			sph->Name = "ShotFactory";
			spheres = sph->AddComponent<InstanceFactory>();
		}
	};

	
}