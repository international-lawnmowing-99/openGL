#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <Application.h>


class MyCamera {
public:
	MyCamera(aie::Application *parent);
	~MyCamera();

	void Update(float deltaTime);

	void LookAt(glm::vec3 target);
	void SetPosition(glm::vec3 position);

	const glm::vec3 & GetPosition();
	const glm::mat4 & GetView();
	const glm::mat4 & GetProjection();

	bool isVisible(glm::vec3 position, glm::vec3 furthestPos);

private:
	aie::Application * app;
	glm::mat4 m_viewMatrix; // the matrix we'll pass to OpenGL so it knows how the camera looks
							// at the world

	glm::mat4 m_projectionMatrix;

	glm::vec3 m_cameraLook; // the actual direction the camera is aiming
	glm::vec3 m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // setting camera orientation so that the
														// y axis is always up
	glm::vec3 m_position = glm::vec3(0, 0, 0);
	float m_yaw = 0.0f;		// left/right rotation of view
	float m_pitch = 0.0f;	// up/down rotation of view
	float m_roll = 0.0f;	// rotating the orientation of the view


	//mouse speeds
	float m_mouseFastSpeed = 30.0f;
	float m_mouseSlowSpeed = 5.0f;
	const float m_mouseSensitivity = 10.0f;
	float m_moveSpeed = m_mouseFastSpeed;

	int m_lastMouseXPos = 0;	// recording previous mouse position so we can
	int m_lastMouseYPos = 0;	// see how far it has moved

	void CalculateLook();	// gives us the vector of direction using yaw, pitch and roll
	void CalculateView();	// sets up the view matrix based on our camera information
	void CalculateProjection();
};