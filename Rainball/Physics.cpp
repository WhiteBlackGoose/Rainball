#include <MxEngine.h>
#include "Physics.h"

/*

SHOT BEHAVIOUR

*/

Rainball::ShotBehaviour::ShotBehaviour(const Vector3& defaultVelocity, const Vector3& defaultPosition)
    : defaultVelocity(defaultVelocity), defaultPosition(defaultPosition), timeGone(0)
{

}

void Rainball::ShotBehaviour::OnUpdate(MxObject& obj, float delta)
{
    timeGone += delta;
    auto newPos = defaultPosition + defaultVelocity * timeGone + Sqr(timeGone) * GRAVITY;
    obj.Transform.SetPosition(newPos);
}

float Rainball::ShotBehaviour::GetSpeed()
{
    return Length(defaultVelocity + timeGone * GRAVITY);
}


/*

SURFACE SCRIPT

*/

Rainball::SurfaceScript::SurfaceScript(float offset, float SCALE)
    : offset(offset / 4 * SCALE), scale(SCALE), offsetWaveY(0)
{

}

void Rainball::SurfaceScript::OnUpdate(float timeDelta)
{
    t += timeDelta;
    auto newElev = 8 * scale * (0.5f + sin(t + offset) / 2) * 0 + offsetWaveY;
    auto newSize = newElev + BOX_HEIGHT;
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


Rainball::WaveScript::WaveScript(float strength, Array2D<SurfaceScript>& cubes)
    : strength(strength),
    arr2(cubes)
{
    useless = false;
}

void Rainball::WaveScript::OnUpdate(MxObject& obj, float delta)
{
    const float S = 7;
    const float w = 0.45;
    const float A = 0.008;
    const float T = 10;
    const float L = 2 / w;
    const float phi = S * 2 / L;

    t += delta;
    auto epidist = S * t;
    useless = epidist > arr2.width() * SCALE * 1;
    for (int x = 0; x < arr2.width(); x++)
        for (int z = 0; z < arr2.height(); z++)
        {
            auto epicenter = obj.Transform.GetPosition();
            auto place = arr2[x][z].inst->Transform.GetPosition();
            auto D = (Vector2(place.x - epicenter.x, place.z - epicenter.z)) / 2.7f;
            if (Length(D) - (t * S) > 2)
                continue;
            arr2[x][z].offsetWaveY += strength / (std::abs(Length(D) - (t * S)) + 1) * -A * std::sin(log(t + 1) * T / t * Length(D) * w);
        }
}