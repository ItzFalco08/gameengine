#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "SceneView.hpp"

struct Camera {
    // camera properties
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    double _yaw = 90.0; 
    double _pitch = 0.0; 

    // projection matrix
    float fov = 80.0f;
    float _near = 0.1f;
    float _far = 500.0f;

    // api
    void rotateY(double yaw) { // assumption: radians
        _yaw += yaw;

        front.x = glm::cos(_pitch) * glm::cos(_yaw);
        front.z = glm::cos(_pitch) * glm::sin(_yaw);
    }

    void rotateX(double pitch) {
        _pitch += pitch;

        
    }

    // matrix calculations
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