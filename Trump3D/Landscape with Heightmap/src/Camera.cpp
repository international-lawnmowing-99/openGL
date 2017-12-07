#define GLM_SWIZZLE
#include "Camera.h"
#include <Input.h>
#include <imgui.h>

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <gl_core_4_4.h>

MyCamera::MyCamera(aie::Application *parent)
{
	app = parent;
	CalculateProjection();
}

MyCamera::~MyCamera()
{
}

void MyCamera::Update(float deltaTime)
{
	aie::Input *input = aie::Input::getInstance(); // get a pointer to the input manager

	// If shift is down speed up;

	if (input->wasKeyPressed(aie::INPUT_KEY_LEFT_SHIFT))
	{
		m_moveSpeed = m_mouseFastSpeed;
	}
	if (input->wasKeyReleased(aie::INPUT_KEY_LEFT_SHIFT))
	{
		m_moveSpeed = m_mouseSlowSpeed;
	}
	// Move the camera using keys
	if (input->isKeyDown(aie::INPUT_KEY_W))
		m_position += m_moveSpeed * deltaTime * m_cameraLook;

	if (input->isKeyDown(aie::INPUT_KEY_S))
		m_position -= m_moveSpeed * deltaTime * m_cameraLook;

	glm::vec3 strafeVec = glm::normalize(glm::cross(m_cameraLook, m_cameraUp)); // Get the cross product of forward and up vectors 
																// to produce a vector perpendicular to both

	if (input->isKeyDown(aie::INPUT_KEY_A))
		m_position -= m_moveSpeed * deltaTime * strafeVec;

	if (input->isKeyDown(aie::INPUT_KEY_D))
		m_position += m_moveSpeed * deltaTime * strafeVec;




	// Rotate camera based on mouse movement
	// -------------------------------------

	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
	{
		float rotationAmount = m_mouseSensitivity * deltaTime;
		int mouseX, mouseY;			// mouse X and Y coordinates from input
		float xOffset, yOffset;		// how far we want to rotate the camera in these axes
		input->getMouseXY(&mouseX, &mouseY); // get mouse location and place it in our variables

		// Find out how far the mouse has moved since last frame
		xOffset = (mouseX - m_lastMouseXPos) * rotationAmount;
		yOffset = (mouseY - m_lastMouseYPos) * rotationAmount;

		// use these values in our Yaw and Pitch values (so we can rotate that far)
		m_yaw += xOffset;
		m_pitch += yOffset;

		// save last mouse position values
		m_lastMouseXPos = mouseX;
		m_lastMouseYPos = mouseY;
	} 
	else
	{
		// track the last position of the mouse regardless of whether we're clicking
		input->getMouseXY(&m_lastMouseXPos, &m_lastMouseYPos);
	}
	// calculate the new cameraLook vector
	CalculateLook();
	CalculateProjection();
}

void MyCamera::LookAt(glm::vec3 target)
{
	glm::vec3 look = glm::normalize(target - m_position); // create a vector which is the direction to the target from our position
	m_pitch = glm::degrees(glm::asin(look.y));
	m_yaw = glm::degrees(atan2(look.y, look.x));
	// roll (might have to look up some maths if you want to compute the roll

	CalculateLook();
}

void MyCamera::CalculateProjection()
{
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
		app->getWindowWidth() / (float)app->getWindowHeight(),
		0.1f, 1000.f);
}
void MyCamera::SetPosition(glm::vec3 position)
{
	m_position = position;
	CalculateView();
}

const glm::vec3 & MyCamera::GetPosition()
{
	return m_position;
}

const glm::mat4 & MyCamera::GetView()
{
	return m_viewMatrix;
}

const glm::mat4 & MyCamera::GetProjection()
{
	return m_projectionMatrix;
}

bool MyCamera::isVisible(glm::vec3 position, glm::vec3 furthestPos)
{
	glm::mat4 projView = m_projectionMatrix * m_viewMatrix;
	position = (projView * glm::vec4(position, 1)).xyz;
	furthestPos = (projView * glm::vec4(furthestPos, 1)).xyz;
	
	glm::vec3 radial = (position - furthestPos);
	float radius = (float)radial.length();

	if (position.x <= 1 && position.x >= -1 && 
		position.y <= 1 && position.y >= -1 && 
		position.z <= 1 && position.z >= 1)
	{
		return true;
	}
	else return false;
} 

void MyCamera::CalculateLook()
{
	// some circle geometry maths to convert the viewing angle from
	// Yaw, Pitch and Roll into a single normalized vector
	if (m_pitch >= 90)
	{
		m_pitch = 89.9f;
	}
	if (m_pitch <= -90)
	{
		m_pitch = -89.9f;
	}

	glm::vec3 look;
	look.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
	look.y = glm::sin(glm::radians(m_pitch)) * glm::cos(glm::radians(m_roll));
	look.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
	m_cameraLook = glm::normalize(look);

	CalculateView();
}

void MyCamera::CalculateView()
{
	m_viewMatrix = glm::lookAt(m_position, m_position + m_cameraLook, m_cameraUp);
}