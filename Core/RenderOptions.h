#pragma once

// Optional settings for one draw call.
// Keep this small first; add camera, flip, alpha, scale only when needed.
struct RenderOptions
{
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float alpha = 1.0f;
    bool flipX = false;
    bool flipY = false;
    bool useCamera = true;
};
