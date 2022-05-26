#include "Skybox.h"
#include "../shaders/Shared.h"

Skybox::Skybox() : shader_("shaders/skybox.vert", "shaders/skybox.frag"), box_(Mesh::CreateQuad(2.0f))
{
}

void Skybox::Render(const glm::mat4 &viewMatrix, const glm::mat4 &projMatrix)
{
    shader_.Use();

    // Calculate matrix used to determine view vector
    glm::mat4 view = glm::inverse(viewMatrix);
    view[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    shader_.SetVar("invOriginCamMat", view * glm::inverse(projMatrix));

    // Draw without depth test
    glDisable(GL_DEPTH_TEST);
    box_.Draw();
    glEnable(GL_DEPTH_TEST);
}
