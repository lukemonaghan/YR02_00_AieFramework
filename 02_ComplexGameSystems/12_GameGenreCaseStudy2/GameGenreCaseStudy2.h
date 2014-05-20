#ifndef ___12_GameGenreCaseStudy2_H_
#define ___12_GameGenreCaseStudy2_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class GameGenreCaseStudy2 : public Application {
public:

	GameGenreCaseStudy2();
	virtual ~GameGenreCaseStudy2();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	void		setupSelectionBox();
	void		drawSelectionBox(const glm::vec2& a_start, const glm::vec2& a_end);

	glm::vec3	getPickRayDirection();
	glm::vec3	rayPlaneIntersection(const glm::vec3& a_start,const glm::vec3& a_dir, const glm::vec4& a_plane);

	glm::vec3		m_target;
	bool			m_mouseDown;
	glm::vec2		m_mouseDownPos;

	unsigned int	m_selectionVAO, m_selectionVBO;
	unsigned int	m_selectionShader;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // ___12_GameGenreCaseStudy2_H_