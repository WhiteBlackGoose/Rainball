#pragma once
#include <MxEngine.h>
#include "Physics.h"
#include "Actions.h"

namespace Rainball
{
    using namespace MxEngine;

    

    class MxApplication : public Application
    {
        
        MxObject::Handle camera;

        Player player;
        Waver waver{ sizeX, sizeZ, SCALE };

        void DefaultInit()
        {
            camera = MxObject::Create();
            camera->Name = "Player Camera";
            auto controller = camera->AddComponent<CameraController>();
            auto input = camera->AddComponent<InputControl>();
            controller->ListenWindowResizeEvent();
            controller->SetMoveSpeed(30);
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
            
            // add additional light
            auto light = MxObject::Create();
            light->AddComponent<PointLight>()->DiffuseColor = Vector3(1, 0.7f, 0) * 54.f;
            light->Transform.SetPosition(Vector3(25, 25, 25));
            light->Name = "Little Sun";            
            
            EventManager::AddEventListener("GC", [this](FpsUpdateEvent& even) mutable {
                waver.GarbageCollect();
                });

        }

        virtual void OnUpdate() override
        {
            waver.Update(GetTimeDelta(), 0.1);
            if (InputManager::IsMousePressed(MouseButton::LEFT))
            {
                auto controller = camera->GetComponent<CameraController>();
                player.Shoot(camera->Transform.GetPosition(), controller->GetDirection(), DEFAULT_SPEED);
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