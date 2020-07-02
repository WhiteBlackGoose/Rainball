#pragma once
#include "../MxEngine/src/MxEngine.h"

namespace Rainball
{
    using namespace MxEngine;

    static constexpr float SCALE = 0.3f;
    static constexpr Vector3 GRAVITY = { 0, -6.f, 0 };
    static constexpr float DEFAULT_SPEED = 10.f;
    static constexpr size_t sizeX = 200;
    static constexpr size_t sizeZ = 200;



    struct ShotBehaviour {
        float timeGone;
        Vector3 defaultVelocity;
        Vector3 defaultPosition;
        ShotBehaviour(const Vector3& defaultVelocity, const Vector3& defaultPosition)
            : defaultVelocity(defaultVelocity), defaultPosition(defaultPosition), timeGone(0)
        {

        }
        void OnUpdate(MxObject& obj, float delta)
        {
            timeGone += delta;
            auto newPos = defaultPosition + defaultVelocity * timeGone + Sqr(timeGone) * GRAVITY;
            obj.Transform.SetPosition(newPos);
        }
        float GetSpeed()
        {
            return Length(defaultVelocity + timeGone * GRAVITY);
        }
    };


    struct PosBehavior
    {
    public:
        float t = 0;
        float offset;
        float scale;
        float offsetWaveY;
        MxObject::Handle inst;
        PosBehavior(float offset, float SCALE)
            : offset(offset / 4 * SCALE), scale(SCALE), offsetWaveY(0)
        {

        }
        void OnUpdate(float timeDelta)
        {
            t += timeDelta;
            auto newElev = 8 * scale * (0.5f + sin(t + offset) / 2) * 0 + offsetWaveY;
            inst->Transform.SetPosition(Vector3(inst->Transform.GetPosition().x, newElev, inst->Transform.GetPosition().z));
            inst->GetComponent<Instance>()->SetColor({ newElev / (8 * scale) * 0.3 + 0.7f, newElev / (8 * scale) * 0.3f + 0.7f, 1 });
            offsetWaveY = 0;
        }
    };


    struct WaveBehaviour
    {
    public:


        float t = 0;
        float strength;
        Array2D<PosBehavior>& arr2;
        bool useless;

        WaveBehaviour(float strength, Array2D<PosBehavior>& cubes) : strength(strength),
            arr2(cubes)
        {
            useless = false;
        }

        void OnUpdate(MxObject& obj, float delta)
        {
            const float S = 7;
            const float w = 0.45;
            const float A = 0.05;
            const float T = 10;
            const float L = 2 / w;
            const float phi = S * 2 / L;

            t += delta;
            auto epidist = S * t;
            useless = epidist > arr2.width() * SCALE * 0.1;
            for (int x = 0; x < arr2.width(); x++)
                for (int z = 0; z < arr2.height(); z++)
                {
                    auto epicenter = obj.Transform.GetPosition();
                    auto place = arr2[x][z].inst->Transform.GetPosition();
                    auto D = (Vector2(place.x - epicenter.x, place.z - epicenter.z));
                    if (Length(D) - (t * S) > 2)
                        continue;
                    arr2[x][z].offsetWaveY += strength / (std::abs(Length(D) - (t * S)) + 1) * -A * std::sin(log(t + 1) * T / t * Length(D) * w);
                }
        }
    };

    class MxApplication : public Application
    {
        Array2D<PosBehavior> bumpers;
        MxObject::Handle camera;
        InstanceFactory::Handle spheres;
        InstanceFactory::Handle waves;
    public:

        virtual void OnCreate() override
        {
            // create camera object
            camera = MxObject::Create();
            camera->Name = "Player Camera";
            // add CameraController component which handles camera image rendering
            auto controller = camera->AddComponent<CameraController>();
            // add InpitControl which handles keyboard and mouse input events
            auto input = camera->AddComponent<InputControl>();
            // set camera to change ratio automatically depending on application window size
            controller->ListenWindowResizeEvent();
            controller->SetMoveSpeed(30);
            // bind player movement to classic WASD mode and space/shift to fly, rotation is done with mouse
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();
            // set controller to be main application viewport
            RenderManager::SetViewport(controller);

            // create global directional light
            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            // add DirectionalLight component with custom light direction
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.5f, 1.0f, 1.0f);
            // make directional light to be centered at current viewport position (is set by RenderManager::SetViewport)
            dirLight->FollowViewport();

            auto instancer = MxObject::Create();
            instancer->AddComponent<MeshSource>()->Mesh = Primitives::CreateCube();
            instancer->AddComponent<MeshRenderer>()->GetMaterial()->Reflection = 1;
            auto factory = instancer->AddComponent<InstanceFactory>();
            instancer->Name = "Cubes";

            bumpers.resize(sizeX, sizeZ, PosBehavior(13e-435, SCALE));
            for (int x = 0; x < sizeX; x++)
                for (int z = 0; z < sizeZ; z++)
                {
                    bumpers[x][z] = PosBehavior(x + z, SCALE);
                    bumpers[x][z].inst = factory->MakeInstance();
                    bumpers[x][z].inst->Transform.SetPosition({ SCALE * x, 0, SCALE * z });
                    bumpers[x][z].inst->Transform.SetScale(Vector3(SCALE, 4 * SCALE, SCALE));
                }
            auto light = MxObject::Create();
            light->AddComponent<PointLight>()->DiffuseColor = Vector3(1, 0.7f, 0) * 14.f;
            light->Transform.SetPosition(Vector3(5, 15, 5));
            light->Name = "Little Sun";

            auto sph = MxObject::Create();
            sph->AddComponent<MeshSource>()->Mesh = Primitives::CreateSphere(100);
            sph->AddComponent<MeshRenderer>();
            sph->Transform.SetScale(0.3);
            sph->Name = "Spheres";
            spheres = sph->AddComponent<InstanceFactory>();
            auto wave = MxObject::Create();
            wave->Name = "Waves";
            waves = wave->AddComponent<InstanceFactory>();
            camera->AddComponent<Skybox>()->Texture = AssetManager::LoadCubeMap(R"(D:\main\vs_prj\MxEngine\samples\SandboxApplication\Resources\textures\dawn.jpg)");

            //EventManager::AddEventListener("GC", [this](FpsUpdateEvent& even) mutable {
            /*
            EventManager::AddEventListener("GC", [this](UpdateEvent& even) mutable {
                auto num1 = waves->GetCount();
                for (auto& inst : waves->GetInstances())
                    if (inst->GetComponent<Behaviour>()->GetBehaviour<WaveBehaviour>().useless)
                        MxObject::Destroy(inst);
                auto a = 3;
                });*/
        }


        float timeGone = 0;
        int updateNum = 0;
        virtual void OnUpdate() override
        {
            for (int x = 0; x < sizeX; x++)
                for (int z = 0; z < sizeZ; z++)
                    bumpers[x][z].OnUpdate(GetTimeDelta());
            timeGone += GetTimeDelta();
            if (InputManager::IsMousePressed(MouseButton::LEFT))
            {
                timeGone = 0;
                auto inst = spheres->MakeInstance();
                inst->Transform.SetPosition(camera->Transform.GetPosition());
                auto controller = camera->GetComponent<CameraController>();
                inst->AddComponent<Behaviour>(ShotBehaviour(
                    controller->GetDirection() * DEFAULT_SPEED,
                    camera->Transform.GetPosition()
                ));
            }

            updateNum++;
            if (updateNum > 10)
            {
                for (auto& inst : spheres->GetInstances())
                {
                    if (inst->Transform.GetPosition().y < -10)
                    {
                        MxObject::Destroy(inst);
                        continue;
                    }
                    auto x = (int)round(inst->Transform.GetPosition().x / SCALE);
                    auto z = (int)round(inst->Transform.GetPosition().z / SCALE);
                    if (x < 0 || z < 0 || x >= sizeX || z >= sizeZ)
                        continue;
                    if (inst->Transform.GetPosition().y < bumpers[x][z].inst->Transform.GetPosition().y)
                    {
                        auto wInst = waves->MakeInstance();
                        auto pos = bumpers[x][z].inst->Transform.GetPosition();
                        wInst->Transform.SetPosition(pos);
                        wInst->AddComponent<Behaviour>(WaveBehaviour(
                            inst->GetComponent<Behaviour>()->GetBehaviour<ShotBehaviour>().GetSpeed()
                            , bumpers));
                        MxObject::Destroy(inst);
                    }
                }
                updateNum = 0;
            }
        }

        virtual void OnDestroy() override
        {

        }
    };
}

int main()
{
    ProjectTemplate::MxApplication app;
    app.Run();
    return 0;
}