#include "RenderTarget.h"

int RenderTarget::NORMAL_FBO = 0;

RenderTarget::RenderTarget(int W,int H){
	int i;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &NORMAL_FBO);
	printf("Default FBO is: %i\n", NORMAL_FBO);

	int maxAttachments = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttachments);
	printf("The number of FBO color attachments my graphics card supports is: %i\n", maxAttachments);

	//Gen Colour Tex
	glGenTextures(1,&m_FBO_texture);
	glBindTexture(GL_TEXTURE_2D,m_FBO_texture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,W,H,0,GL_RGBA,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	i = glGetError();
	if(i != 0){ std::cout << "Error happened while loading the texture: " << i << std::endl;}
	glBindTexture(GL_TEXTURE_2D,0);

	//Gen Depth Tex
	glGenTextures(1,&m_FBO_depthtexture);
	glBindTexture(GL_TEXTURE_2D,m_FBO_depthtexture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,W,H,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
	i = glGetError();
	if(i != 0){ std::cout << "Error happened while loading the texture: " << i << std::endl;}
	glBindTexture(GL_TEXTURE_2D,0);

	//Gen FBO
	glGenFramebuffers(1,&m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER,m_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,m_FBO_texture,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,m_FBO_depthtexture,0);

	i = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(i != GL_FRAMEBUFFER_COMPLETE) { std::cout << "Framebuffer is not OK, status=" << i << std::endl; }

	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

RenderTarget::~RenderTarget(void) { }

void RenderTarget::StartDraw(){
	// bind the framebuffer for rendering
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
}

void RenderTarget::EndDraw(){
	// switch back to rendering to the back buffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NORMAL_FBO);
}

unsigned int RenderTarget::GetTexture(){
	// bind m_FBO_texture as a texture input so we can use it when rendering to the back buffer
	return m_FBO_texture;
}