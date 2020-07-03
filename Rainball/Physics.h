#pragma once
#include <MxEngine.h>

namespace Rainball
{
    using namespace MxEngine;

    constexpr float SCALE = 0.12f;
    constexpr Vector3 GRAVITY = { 0, -6.f, 0 };
    constexpr float DEFAULT_SPEED = 40.f;
    constexpr size_t sizeX = 200;
    constexpr size_t sizeZ = 200;
    constexpr float BOX_HEIGHT = 2.5f;

    struct ShotBehaviour {
        float timeGone;
        Vector3 defaultVelocity;
        Vector3 defaultPosition;

        /*
        Creates an instance of a shot ball
        */
        ShotBehaviour(const Vector3& defaultVelocity, const Vector3& defaultPosition);

        /*
        Called automatically by Mx
        */
        void OnUpdate(MxObject& obj, float delta);

        /*
        Returns the current speed of the shot
        */
        float GetSpeed();
    };


    struct SurfaceScript
    {
    public:
        float t = 0;
        float offset;
        float scale;
        float offsetWaveY;
        MxObject::Handle inst;
        SurfaceScript(float offset, float SCALE);
        void OnUpdate(float timeDelta);
    };


    struct WaveScript
    {
    public:
        float t = 0;
        float strength;
        Array2D<SurfaceScript>& arr2;
        bool useless;
        WaveScript(float strength, Array2D<SurfaceScript>& cubes);
        void OnUpdate(MxObject& obj, float delta);
    };

}