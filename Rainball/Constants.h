#pragma once
#include <MxEngine.h>

namespace Rainball
{
    /*
    SETUP CONSTANTS
    */
    constexpr float SETUP_SCALE = 100.1f;
    constexpr size_t SETUP_SIZE_X = 200;
    constexpr size_t SETUP_SIZE_Z = 200;
    constexpr float SETUP_BOX_HEIGHT = 4.5f;

    /*
    PLAYER CONSTANTS
    */
    constexpr MxEngine::Vector3 BALL_GRAVITY = { 0, -6.f, 0 };
    constexpr float BALL_DEFAULT_SPEED = 40.f;
    constexpr float BALL_JUMP_COEF = 0.6f;
    constexpr float BALL_JUMP_RESISTANCE = 0.8f;
    constexpr float BALL_BOUNCE_THRESHOLD = 10.f;

    /*
    WAVE CONSTANTS
    */
    constexpr float WAVE_SPEED = 7;
    constexpr float WAVE_HEIGHT_SCALE = 0.01;
    constexpr float WAVE_TIME_CONST = 10;
    constexpr float WAVE_LENGTH = 10;
    constexpr float WAVE_DEFAULT_FLOW_COEF = 0.025f;
    constexpr float WAVE_DEFAULT_FLOW_FREQUENCY = 0.3f;
    constexpr float WAVE_QUALITY = 3.f;
}