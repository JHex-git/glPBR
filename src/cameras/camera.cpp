#include "cameras/camera.h"
extern float deltaTime;
namespace cameras
{
Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float speed/* = 1.f */) : m_position(position), m_direction(direction), m_up(up), m_speed(speed)
{
    m_direction = glm::normalize(direction);
    m_right = glm::normalize(glm::cross(m_direction, m_up));
    m_up = glm::normalize(glm::cross(m_right, m_direction));
}

void Camera::Rotate(float xpos, float ypos)
{
    static float lastX;
    static float lastY;
    static bool firstMouse = true;
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    constexpr float sensitivity = 0.05f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;
    float yaw = xOffset;
    float pitch = yOffset;
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    
    glm::mat4 rotate_mat(1.0f);
    rotate_mat = glm::rotate(rotate_mat, glm::radians(-yaw), m_up);
    m_right = rotate_mat * glm::vec4(m_right, 0.0f);
    m_right = glm::normalize(m_right);
    rotate_mat = glm::rotate(rotate_mat, glm::radians(pitch), m_right);
    m_direction = rotate_mat * glm::vec4(m_direction, 0.0f);
    m_direction = glm::normalize(m_direction);
    m_up = glm::normalize(glm::cross(m_right, m_direction));
}

void Camera::Dolly(float offset)
{
    constexpr float sensitivity = 0.05f;
    m_position += m_direction * offset * sensitivity;
}

void Camera::Truck(float xOffset, float yOffset)
{
    m_position += m_right * xOffset * m_speed * deltaTime;
    m_position += m_up * yOffset * m_speed * deltaTime;
}
} // namespace cameras