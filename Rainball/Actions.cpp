#include "Actions.h"
#include <MxEngine.h>
#include "Constants.h"

/*

WAVER

*/

Rainball::Waver::Waver(int sizeX, int sizeZ, float scale, float boxHeight) : 
	sizeX(sizeX), sizeZ(sizeZ), scale(scale)
{
	elevations.resize(sizeX, sizeZ);

	auto wave = MxObject::Create();
	wave->Name = "Waves";
	waves = wave->AddComponent<InstanceFactory>();

	water = MxObject::Create();
	water->AddComponent<MeshSource>();
	water->AddComponent<MeshRenderer>()->GetMaterial()->Reflection = 1;
	water->Transform.SetScale(Vector3(scale, 1, scale));
	water->Name = "Surface";

	bumpers.resize(sizeX, sizeZ, SurfaceScript());
	for (int x = 0; x < sizeX; x++)
		for (int z = 0; z < sizeZ; z++)
		{
			bumpers[x][z] = SurfaceScript(x + z, scale, boxHeight, elevations, x, z);
		}
}

void Rainball::Waver::Update(float delta, float strength)
{
	for (int x = 0; x < sizeX; x++)
		for (int z = 0; z < sizeZ; z++)
			bumpers[x][z].OnUpdate(delta);
	water->GetComponent<MeshSource>()->Mesh = Primitives::CreateSurface(elevations);
	
}

std::tuple<int, int> Rainball::Waver::GetSurfaceId(const MxEngine::Vector3& pos)
{
	auto x = (int)round((pos.x + scale / 2) / scale * sizeX);
	auto z = (int)round((pos.z + scale / 2) / scale * sizeZ);
	return { x, z };
}

MxEngine::Vector3 Rainball::Waver::GetPosition(const int x, const int z)
{
	return Vector3(x * scale / sizeX, elevations[x][z], z * scale / sizeZ);
}

void Rainball::Waver::Wave(const int x, const int z, const float strength)
{
	auto wInst = waves->MakeInstance();
	auto pos = GetPosition(x, z);
	wInst->Transform.SetPosition(pos);
	wInst->AddComponent<Behaviour>(WaveScript(
		strength
		, bumpers, scale, *this));

}

void Rainball::Waver::GarbageCollect()
{
	for (auto& inst : waves->GetInstances())
		if (inst->GetComponent<Behaviour>()->GetBehaviour<WaveScript>().useless)
		{
			MxObject::Destroy(inst);
			MX_DBG("Wave #" + (MxString)inst.GetUUID() + " removed");
		}
}


/*

PLAYER

*/


void Rainball::Player::Shoot(const Vector3& position, const Vector3& direction, const float speed)
{
	auto inst = spheres->MakeInstance();
	inst->Transform.SetPosition(position);

	inst->AddComponent<Behaviour>(ShotBehaviour(
		direction * speed,
		position,
		gravity
	));
}

void Rainball::Player::CheckReaction(Waver& destination)
{
	for (auto& inst : spheres->GetInstances())
	{
		if (inst->Transform.GetPosition().y < -10)
		{
			MxObject::Destroy(inst);
			continue;
		}

		auto [x, z] = destination.GetSurfaceId(inst->Transform.GetPosition());

		if (x < 0 || z < 0 || x >= destination.bumpers.width() || z >= destination.bumpers.height())
			return;

		if (inst->Transform.GetPosition().y < destination.GetPosition(x, z).y)
		{
			auto& shot = inst->GetComponent<Behaviour>()->GetBehaviour<ShotBehaviour>();

			MX_DBG("New wave at (" + ToMxString(x) + "; " + ToMxString(z) + ") was created");
			destination.Wave(x, z, shot.GetSpeed());

			auto velo = shot.GetVelocity();
			volatile auto vecCoef = abs(Dot(Normalize(Vector3(1, 0, 1)), Normalize(velo)));
			auto xzSpeed = Length(Vector2(velo.x, velo.z));

			if (xzSpeed * vecCoef < BALL_BOUNCE_THRESHOLD || vecCoef < OneOverRootTwo<float>())
			{
				MxObject::Destroy(inst);
				continue;
			}

			auto currVelocity = velo;
			currVelocity.y *= -BALL_JUMP_COEF;
			currVelocity.x *= BALL_JUMP_RESISTANCE;
			currVelocity.z *= BALL_JUMP_RESISTANCE;
			shot.defaultVelocity = currVelocity;

			shot.defaultPosition = inst->Transform.GetPosition() + currVelocity * 0.1f;

			shot.timeGone = 0;
		}
	}
}

Rainball::Player::Player(const Vector3& gravity, const float scale)
	: gravity(gravity), scale(scale)
{
	auto sph = MxObject::Create();
	sph->AddComponent<MeshSource>()->Mesh = Primitives::CreateSphere(100);
	sph->AddComponent<MeshRenderer>();
	sph->Transform.SetScale(0.6);
	sph->Name = "ShotFactory";
	spheres = sph->AddComponent<InstanceFactory>();
}

float Rainball::Waver::GetWidth()
{
	return this->bumpers.width() * scale / sizeX;
}

float Rainball::Waver::GetHeight()
{
	return this->bumpers.height() * scale / sizeZ;
}