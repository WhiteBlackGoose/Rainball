#pragma once
#include <MxEngine.h>

namespace Rainball
{
    using namespace MxEngine;

    struct ShotBehaviour {
        float timeGone;
        Vector3 defaultVelocity;
        Vector3 defaultPosition;
        Vector3 gravity;

        /*
        Creates an instance of a shot ball
        */
        ShotBehaviour(const Vector3& defaultVelocity, const Vector3& defaultPosition, const Vector3& gravity);

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
        float boxHeight;

        MxObject::Handle inst;
        SurfaceScript(float offset, float SCALE, float boxHeight);
        void OnUpdate(float timeDelta);
    };


    struct WaveScript
    {
    public:
        float t = 0;
        float strength;
        Array2D<SurfaceScript>& arr2;
        bool useless;
        float scale;
        WaveScript(float strength, Array2D<SurfaceScript>& cubes, float scale);
        void OnUpdate(MxObject& obj, float delta);
    };

}