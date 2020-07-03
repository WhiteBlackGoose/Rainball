#include <MxEngine.h>
#include "Physics.h"
#include "Constants.h"
#include <cstdarg>

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
    return Length(defaultVelocity + timeGone * gravity);
}


/*

SURFACE SCRIPT

*/

Rainball::SurfaceScript::SurfaceScript(float offset, float SCALE, float boxHeight)
    : offset(offset / 4 * SCALE), scale(SCALE), offsetWaveY(0), boxHeight(boxHeight)
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
    auto newElev = scale * (0.5f + defWave / 2) * WAVE_DEFAULT_FLOW_COEF + offsetWaveY;
    auto newSize = newElev + boxHeight;
    auto newPos = inst->Transform.GetPosition();
    newPos.y = newElev / 2;
    inst->Transform.SetPosition(newPos);
    auto newScale = inst->Transform.GetScale();
    newScale.y = newSize;
    inst->Transform.SetScale(newScale);

    inst->GetComponent<Instance>()->SetColor({ newElev / (8 * scale) * 0.3 + 0.7f, newElev / (8 * scale) * 0.3f + 0.7f, 1 });
    offsetWaveY = 0;
}

/*

WAVE SCRIPT

*/


Rainball::WaveScript::WaveScript(float strength, Array2D<SurfaceScript>& cubes, float scale)
    : strength(strength),
    arr2(cubes), scale(scale)
{
    useless = false;
}

void Rainball::WaveScript::OnUpdate(MxObject& obj, float delta)
{
    const float S = 7;
    const float A = 0.008;
    const float T = 10;

    t += delta;
    auto epidist = WAVE_SPEED * t;
    useless = epidist > arr2.width() * scale;
    for (int x = 0; x < arr2.width(); x++)
        for (int z = 0; z < arr2.height(); z++)
        {
            auto epicenter = obj.Transform.GetPosition();
            auto place = arr2[x][z].inst->Transform.GetPosition();
            auto D = (Vector2(place.x - epicenter.x, place.z - epicenter.z)) / WAVE_QUALITY;
            if (Length(D) - (t * WAVE_SPEED) > 2)
                continue;
            arr2[x][z].offsetWaveY += strength / (std::abs(Length(D) - (t * WAVE_SPEED)) + 1) * -A * std::sin(log(t + 1) * T / t * Length(D) / 2);
        }
}