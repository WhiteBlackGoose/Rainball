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
		Array2D<float> elevations;
		MxObject::Handle water;
		int sizeX, sizeZ;
		float scale;
	public:
		Waver(int sizeX, int sizeZ, float scale, float boxHeight);

		/*
		Call it in OnUpdate so it recomputes waves of the surface
		*/
		void Update(float delta, float strength);
		std::tuple<int, int> GetSurfaceId(const Vector3& pos);

		/*
		Starts a new script for waving
		*/
		void Wave(const int x, const int z, const float strength);

		/*
		Collects scripts generating waves if a wave is too far and destroyes them
		*/
		void GarbageCollect();

		float GetWidth();
		float GetHeight();

		Vector3 GetPosition(const int x, const int z);
	};

	class Player
	{
		friend Waver;
		InstanceFactory::Handle spheres;
		Vector3 gravity;
		float scale;
	public:
		/*
		Shoots a sphere from position along with direction with the speed of speed
		*/
		void Shoot(const Vector3& position, const Vector3& direction, const float speed);
		
		/*
		Removes useless shots
		and causes waves if one intersects a surface
		*/
		void CheckReaction(Waver& destination);

		/*
		Creates a factory for shots
		*/
		Player(const Vector3& gravity, const float scale);
	};

	
}