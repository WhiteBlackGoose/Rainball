#include <MxEngine.h>
#include "Physics.h"
#include "Constants.h"
#include <cstdarg>
#include "Actions.h"

/*

SHOT BEHAVIOUR

*/

Rainball::ShotBehaviour::ShotBehaviour(const Vector3& defaultVelocity, const Vector3& defaultPosition, const Vector3& gravity)
    : defaultVelocity(defaultVelocity), defaultPosition(defaultPosition), timeGone(0), gravity(gravity)
{

}

void Rainball::ShotBehaviour::OnUpdate(MxObject& obj, float delta)
{
    timeGone += delta;
    auto newPos = defaultPosition + defaultVelocity * timeGone + Sqr(timeGone) * gravity;
    obj.Transform.SetPosition(newPos);
}

float Rainball::ShotBehaviour::GetSpeed()
{
    return Length(GetVelocity());
}

MxEngine::Vector3 Rainball::ShotBehaviour::GetVelocity()
{
    return defaultVelocity + timeGone * gravity;
}


/*

SURFACE SCRIPT

*/

Rainball::SurfaceScript::SurfaceScript(float offset, float SCALE, float boxHeight, Array2D<float>& surface, size_t xId, size_t zId)
    : offset(offset / 4 * SCALE), scale(SCALE), offsetWaveY(0), boxHeight(boxHeight), surface(&surface), xId(xId), zId(zId)
{
    
}

template<typename Arg, typename H>
float Fourier(Arg arg, H coef)
{
    static_assert(std::is_same<Arg, float>::value, "Arg should be float");
    static_assert(std::is_same<H, float>::value, "Arg should be float");
    return std::sin(arg / coef);
}

template<typename Arg, typename H, typename ... T>
float Fourier(Arg arg, H coef, T... rest)
{
    static_assert(std::is_same<Arg, float>::value, "Arg should be float");
    static_assert(std::is_same<H, float>::value, "Arg should be float");
    return Fourier(arg, rest...) * Fourier(arg, coef);
}

void Rainball::SurfaceScript::OnUpdate(float timeDelta)
{
    t += timeDelta;
    auto defWave = Fourier((t + offset) * WAVE_DEFAULT_FLOW_FREQUENCY, 1.f, 3.f, 5.f, 7.f);
    auto newElev = scale * (0.5f + defWave / 2) * WAVE_DEFAULT_FLOW_COEF * 0 + offsetWaveY * 10;

    (*surface)[xId][zId] = newElev;

    offsetWaveY = 0;
}

/*

WAVE SCRIPT

*/


Rainball::WaveScript::WaveScript(float strength, Array2D<SurfaceScript>& cubes, float scale, Waver& waver)
    : strength(strength),
    arr2(cubes), scale(scale),
    parent(waver)
{
    useless = false;
}

void Rainball::WaveScript::OnUpdate(MxObject& obj, float delta)
{
    t += delta;
    auto epidist = WAVE_SPEED * t;
    useless = epidist > parent.GetWidth() * 1.3f;
    for (int x = 0; x < arr2.width(); x++)
        for (int z = 0; z < arr2.height(); z++)
        {
            auto epicenter = obj.Transform.GetPosition();
            auto place = parent.GetPosition(x, z);
            auto D = (Vector2(place.x - epicenter.x, place.z - epicenter.z)) / WAVE_QUALITY;
            auto delta = strength / (std::abs(Length(D) - (t * WAVE_SPEED)) + 15) * -WAVE_HEIGHT_SCALE * std::sin(log(t + 1) * WAVE_TIME_CONST / t * Length(D) / 2);
            auto effectiveNotReached = Length(D) - t * WAVE_SPEED;
            if (effectiveNotReached > 2)
                delta /= std::pow(1.8f, effectiveNotReached);
            arr2[x][z].offsetWaveY += delta;
        }
}