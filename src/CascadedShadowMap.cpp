#include "CascadedShadowMap.h"
#include "ChunkManager.h"
#include "WindowManager.h"
#include "../shaders/Shared.h"

#include <queue>
#include <array>

CascadedShadowMap::CascadedShadowMap(const std::vector<Cascade> &cascades) :
    cascades_(cascades),
    shader_("shaders/shadow.vert", "shaders/shadow.frag")
{
    assert(cascades.size() <= MAX_CASCADES);

    for (const Cascade &c : cascades_)
        framebuffers_.emplace_back(c.resolution, c.resolution);
}

void CascadedShadowMap::Render(const glm::mat4 &cameraMatrix)
{
    shaderInfo_.clear();

    // Transforms from NDC space to world space
    glm::mat4 inverseCameraMatrix = glm::inverse(cameraMatrix);

    // Transforms from world space to light view space
    glm::mat4 lightTransform = glm::lookAt(glm::vec3(0.0f), -glm::vec3(LIGHT_DIR), glm::vec3(0.0f, 1.0f, 0.0f));

    constexpr std::array<glm::vec2, 4> corners = {
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f,  1.0f),
        glm::vec2( 1.0f, -1.0f),
        glm::vec2( 1.0f,  1.0f)
    };

    // Corners of camera frustum in light space
    std::array<glm::vec3, 4> nearCorners;
    std::array<glm::vec3, 4> farCorners;
    for (size_t i = 0; i < corners.size(); i++)
    {
        glm::vec4 world = inverseCameraMatrix * glm::vec4(corners[i], -1.0f, 1.0f);
        nearCorners[i] = lightTransform * (world / world.w);
        world = inverseCameraMatrix * glm::vec4(corners[i], 1.0f, 1.0f);
        farCorners[i] = lightTransform * (world / world.w);
    }

    // Circular buffer that stores all for current subfrustum
    std::array<glm::vec3, 8> subfrustaCorners;
    std::copy(nearCorners.begin(), nearCorners.end(), subfrustaCorners.begin());
    size_t index = nearCorners.size();

    // Calculate the transformations for each cascade and render
    for (size_t i = 0; i < cascades_.size(); i++)
    {
        // Add the next plane's corners, overwriting previous's
        for (size_t j = 0; j < corners.size(); j++)
        {
            subfrustaCorners[index++] = glm::mix(nearCorners[j], farCorners[j], cascades_[i].depth);
        }
        index %= subfrustaCorners.size(); // Circular buffer

        // Calculate min and max of all 8 subfrustum points
        glm::vec3 min = glm::vec3(std::numeric_limits<float>::infinity());
        glm::vec3 max = glm::vec3(-std::numeric_limits<float>::infinity());
        for (const glm::vec3 &v : subfrustaCorners)
        {
            min = glm::min(min, v);
            max = glm::max(max, v);
        }

        // Create transformation for cascade (offset zNear so it includes objects outside view frustum)
        shaderInfo_.push_back({glm::ortho(min.x, max.x, min.y, max.y, -max.z - 400.0f, -min.z) * lightTransform, cascades_[i].depth, &framebuffers_[i].GetTexture()});
        
        // Render chunks using transformation
        framebuffers_[i].BindFramebuffer();
        ChunkManager::Instance().DrawChunks(shaderInfo_.back().transform, shader_);
    }
    // Set default framebuffer
    WindowManager::Instance().SetFramebuffer();
}

const std::vector<CascadeShaderInfo> &CascadedShadowMap::GetShaderInfo()
{
    return shaderInfo_;
}

