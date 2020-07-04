#pragma once
#include <MxEngine.h>
#include "Physics.h"
#include "Actions.h"
#include "Constants.h"

namespace Rainball
{
    using namespace MxEngine;

    class MxApplication : public Application
    {
        MxObject::Handle camera;

        Player player{ BALL_GRAVITY, SETUP_SCALE };
        Waver waver{ SETUP_SIZE_X, SETUP_SIZE_Z, SETUP_SCALE, SETUP_BOX_HEIGHT };
        
        void DefaultInit()
        {
            camera = MxObject::Create();
            camera->Name = "Player Camera";
            auto controller = camera->AddComponent<CameraController>();
            auto input = camera->AddComponent<InputControl>();
            controller->ListenWindowResizeEvent();
            controller->SetMoveSpeed(30);
            camera->Transform.SetPosition(Vector3(30, 25, 60));
            
            
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();
            RenderManager::SetViewport(controller);

            // create global directional light
            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            // add DirectionalLight component with custom light direction
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.5f, 1.0f, 1.0f);
            dirLight->ProjectionSize = 180;
            dirLight->FollowViewport();
        }
    public:

        virtual void OnCreate() override
        {
            DefaultInit();
            
            // Uncomment this to enable a skybox
            camera->AddComponent<Skybox>()->Texture = AssetManager::LoadCubeMap("./resources/sky.png");

            /*
            auto light = MxObject::Create();
            light->AddComponent<PointLight>()->DiffuseColor = Vector3(0.6f, 0.6f, 1) * 54.f;
            light->Transform.SetPosition(Vector3(25, 25, 25));
            light->Name = "Little Sun";*/
            
            EventManager::AddEventListener("GC", [this](FpsUpdateEvent& even) mutable {
                waver.GarbageCollect();
                });

            Logger::Instance().UseDebug(false);
        }

        float timeGone = 0;

        virtual void OnUpdate() override
        {
            timeGone += GetTimeDelta();
            if (timeGone > 5)
            {
                if (Random::GetFloat() > 1.5)
                    player.Shoot(Vector3((Random::GetFloat() - 0.5f) * waver.GetWidth() / 2, 60, (Random::GetFloat() - 0.5f) * waver.GetHeight() / 2),
                    Vector3(0, -1, 0), 48);
                else
                {
                    auto angle = Random::GetFloat() * 2 * Pi<float>();
                    auto posFromX = std::sin(angle) * waver.GetWidth();
                    auto posFromZ = std::cos(angle) * waver.GetWidth();
                    auto posFromY = 25;
                    auto posFrom = Vector3(posFromX, posFromY, posFromZ);
                    auto direction = Vector3(0, 0, 0) /*Center's position*/ - posFrom /*Player's position*/;
                    direction = Normalize(direction);
                    player.Shoot(posFrom, direction, 48);
                }
                timeGone = 0;
            }

            waver.Update(GetTimeDelta(), 0.1);
            if (InputManager::IsMousePressed(MouseButton::LEFT))
            {
                auto controller = camera->GetComponent<CameraController>();
                player.Shoot(camera->Transform.GetPosition(), controller->GetDirection(), BALL_DEFAULT_SPEED);
            }

            // cause a wave if a ball intersected the surface
            player.CheckReaction(waver);
        }

        virtual void OnDestroy() override
        {

        }
    };
}

int main()
{
    Rainball::MxApplication app;
    app.Run();
    return 0;
}