#pragma once
#include "Scene.hpp"
#include "components/Transform.hpp"
#include "assets/meta/MaterialAsset.hpp"
#include "assets/runtime/Mesh.hpp"
#include "assets/runtime/Texture.hpp"

struct GPUPointLight {
	glm::vec4 positionRange;     // w = range 
	glm::vec4 colorIntensity;    // w = intensity 
};

struct GPUDirLight {
	glm::vec3 direction;         
	glm::vec3 color;    
};

inline constinit unsigned char whitepixel[] = { 255, 255, 255, 255 };
inline constinit unsigned char normal[] = { 128, 128, 255, 255 };
inline Texture2D fallbackmap;
inline Texture2D normalfallback;

struct BatchEntry {
    uint64_t key; // shader, material, mesh
    ShaderType m_shaderType;
    MaterialAsset* m_material;
    Mesh* m_mesh;
    Submesh* m_submesh;
    TransformComponent* m_transform;
};

class Renderer {
    Scene* scene;

    // Clustered Forward Rendering constants
    uint32_t GX = 16, GY = 9, GZ = 24;
    uint32_t CLUSTER_COUNT = GX * GY * GZ;
    uint32_t LIGHT_COUNT = 0;

    static constexpr uint32_t MAX_LIGHTS = 1024;          // buffer capacity
    static constexpr uint32_t MAX_PER_CLUSTER = 128;

    union {
        struct {
            GLuint lightSSBO;
            GLuint clusterSSBO;
            GLuint countSSBO;
            GLuint indexSSBO;
        };

        GLuint SSBOs[4];
    };

    // other constrants
	static constexpr uint32_t MAX_DIRLIGHTS = 6;
    bool firstFrame = true;
	float prevAspect = 0.0f;
    inline static std::vector<GPUDirLight> gpuDirLights;


public:
    Renderer() = default;
    void Init(); // allocates opengl resource in runtime
    void OpenGLRenderer(Scene* _scene);

private:
    void loadLights();
    void calculateAABBClusters();;
	void calculateClusterLights();;

    // batch material. select material, apply transform, render mesh
    constexpr uint64_t makeKey(uint32_t shader, uint32_t material, uint32_t mesh, uint32_t sub);
    void renderScene();;
};