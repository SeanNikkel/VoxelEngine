#include "Crosshair.h"
#include "../shaders/Shared.h"

Crosshair::Crosshair() : shader_("shaders/Crosshair.vert", "shaders/Crosshair.frag")
{
    // Rotated quad mesh
    cross_.SetVertices(
        {
            { glm::vec3(-1.0f,  0.0f, 0.0f) },
            { glm::vec3( 0.0f, -1.0f, 0.0f) },
            { glm::vec3( 0.0f,  1.0f, 0.0f) },
            { glm::vec3( 1.0f,  0.0f, 0.0f) },
        },
        { 0, 1, 2, 3, 2, 1 }
    );
}

void Crosshair::Render(glm::vec2 size)
{
    shader_.Use();
    shader_.SetVar("size", size);

    // Invert destination color and add alpha * destination color
    glEnable(GL_BLEND);
    glBlendColor(0.0f, 0.0f, 0.0f, 0.3f);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_CONSTANT_ALPHA);

    // Draw without depth
    glDisable(GL_DEPTH_TEST);
    cross_.Draw();
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
}
