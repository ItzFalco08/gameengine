#pragma once
#include "Scene.hpp"
#include "Camera.hpp"
#include "components/Mesh.hpp"
#include "components/Light.hpp"
#include "../utils/globals.hpp"
#include "../utils/Shader.hpp"

extern Shader litShader;
extern Shader unlitShader;

struct GPUPointLight {
	glm::vec4 positionRange;     // w = range 
	glm::vec4 colorIntensity;    // w = intensity 
};

class Renderer {
public:
    Renderer() = default;

    void OpenGLRenderer(Scene* _scene) {
        scene = _scene;
        // renders the scene in a opengl context

        loadLights();
        renderScene();
    }
private:
    Scene* scene;
    
	// Clustered Forward Rendering constants
    static constexpr uint32_t GX = 16, GY = 9, GZ = 24;
    static constexpr uint32_t CLUSTER_COUNT = GX * GY * GZ;
    static constexpr uint32_t MAX_LIGHTS = 1024;          // buffer capacity
    static constexpr uint32_t MAX_PER_CLUSTER = 128;
    GLuint lightSSBO, clusterSSBO, countSSBO, indexSSBO = GL_NONE;

    void loadLights() {
        glCreateBuffers(1, &lightSSBO);
        glCreateBuffers(1, &clusterSSBO);
        glCreateBuffers(1, &countSSBO);
        glCreateBuffers(1, &indexSSBO);

		// light data
		glNamedBufferStorage(lightSSBO, sizeof(GPUPointLight) * MAX_LIGHTS, nullptr, GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(clusterSSBO, sizeof(glm::vec3) * 2 * CLUSTER_COUNT, nullptr, GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(countSSBO, sizeof(uint32_t) * CLUSTER_COUNT, nullptr, GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(indexSSBO, sizeof(uint32_t) * CLUSTER_COUNT, nullptr, GL_DYNAMIC_STORAGE_BIT);
    };

    void renderScene() {
    };
};