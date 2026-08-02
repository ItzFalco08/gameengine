#pragma once

#include <glm/glm.hpp>

struct Camera {
    // Camera properties
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);

    double _yaw = 90.0;
    double _pitch = 0.0;

    // Projection properties
    float fov = 80.0f;
    float _near = 0.1f;
    float _far = 500.0f;

    // API
    void rotate(double dx, double dy);
    void move(glm::vec3 deltaPos);

    glm::mat4 getProjMat();
    glm::mat4 getViewMat();

    bool projDirty = true;
    bool viewDirty = true;
    
    // Cached matrices
    glm::mat4 projectionMat;
    glm::mat4 viewMat;

private:


    void calculateProj();
    void calculateView();
};