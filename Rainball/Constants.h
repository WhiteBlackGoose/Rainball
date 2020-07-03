#pragma once
#include <MxEngine.h>

namespace Rainball
{
    /*
    SETUP CONSTANTS
    */
    constexpr float SETUP_SCALE = 0.12f;
    constexpr size_t SETUP_SIZE_X = 200;
    constexpr size_t SETUP_SIZE_Z = 200;
    constexpr float SETUP_BOX_HEIGHT = 2.5f;

    /*
    PLAYER CONSTANTS
    */
    constexpr MxEngine::Vector3 BALL_GRAVITY = { 0, -6.f, 0 };
    constexpr float BALL_DEFAULT_SPEED = 40.f;

    /*
    WAVE CONSTANTS
    */
    constexpr float WAVE_SPEED = 7;
    constexpr float WAVE_HEIGHT_SCALE = 0.008;
    constexpr float WAVE_LENGTH = 10;
    constexpr float WAVE_DEFAULT_FLOW_COEF = 5.f;
    constexpr float WAVE_DEFAULT_FLOW_FREQUENCY = 3.f;
}