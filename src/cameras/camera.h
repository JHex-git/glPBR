#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
namespace cameras
{
    
class Camera
{
public:
    Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float speed = 1.f);

    inline glm::mat4 GetViewMatrix() const { return glm::lookAt(m_position, m_position + m_direction, m_up); }
    inline glm::mat4 GetProjectionMatrix() const { return m_projection_mat; }
    inline void SetSpeed(float speed) { m_speed = speed; }

    void Rotate(float xpos, float ypos);
    void Dolly(float offset);
    void Truck(float xOffset, float yOffset);

protected:
    glm::mat4 m_projection_mat;

private:
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_up;
    glm::vec3 m_right;
    float m_speed;
};

class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float fov, float aspect, float near, float far) : Camera(position, direction, up)
    {
        m_projection_mat = glm::perspective(glm::radians(fov), aspect, near, far);
    }

    void Zoom(float offset)
    {
        m_projection_mat = glm::perspective(glm::radians(offset), 800.0f / 600.0f, 0.1f, 100.0f);
    }
};

class OrthographicCamera : public Camera
{
public:
    OrthographicCamera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float left, float right, float bottom, float top, float near, float far) : Camera(position, direction, up)
    {
        m_projection_mat = glm::ortho(left, right, bottom, top, near, far);
    }
};
} // namespace cameras

