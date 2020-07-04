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

        /*
        Returns the current velocity
        */
        Vector3 GetVelocity();
    };


    struct SurfaceScript
    {
    public:
        float t = 0;
        float offset;
        float scale;
        float offsetWaveY;
        float boxHeight;

        Array2D<float>* surface;
        size_t xId, zId;
        SurfaceScript(){}
        SurfaceScript(float offset, float SCALE, float boxHeight, Array2D<float>& surface, size_t xId, size_t zId);
        void OnUpdate(float timeDelta);
    };

    class Waver;

    struct WaveScript
    {
    public:
        float t = 0;
        float strength;
        Array2D<SurfaceScript>& arr2;
        Waver& parent;
        bool useless;
        float scale;
        WaveScript(float strength, Array2D<SurfaceScript>& cubes, float scale, Waver& waver);
        void OnUpdate(MxObject& obj, float delta);
    };

}