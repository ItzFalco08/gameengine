#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "SceneView.hpp"

struct Camera {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    float fov = 80.0f;
    float _near = 0.5f;
    float _far = 100.0f;

    glm::mat4 projectionMat;
    glm::mat4 viewMat;
    bool projDirty = true;
    bool viewDirty = true;

    glm::mat4 getProjMat() {
        if (projDirty) {
            calculateProj();
            projDirty = false;
        }

        return projectionMat;
    }

    glm::mat4 getViewMat() {
        if (viewDirty) {
            calculateView();
            viewDirty = false;
        }

        return viewMat;
    }
private:
    void calculateProj() {
        projectionMat = glm::perspective(glm::radians(fov), sceneView.getAspect(), _near, _far);
    }

    void calculateView() {
        viewMat = glm::lookAt(position, position + front, up);
    }
};