#include <QtGui/QMouseEvent>
#include <gl/glew.h>

#include "mesh.h"
#include "defs.h"
#include "debug.h"
#include "skybox.h"
#include "camera.h"
#include "shader.h"
#include "skybox.h"
#include "texture.h"
#include "GLWidget.h"

#include "Header.h"

void APIENTRY debugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);

const GLfloat triangle_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
};

Shader* reflect_, *refract_;
SkyBox* skyBox_;
GLuint vao_, vbo_;
Camera* camera_;
Mesh* mesh_;

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent) {
	setMouseTracking(true);
	CBtt TT;
	int x = TT.GetValue();
}

void GLWidget::initializeGL() {
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		Debug::LogError("failed to initialize GLEW\n");
		return;
	}

	if (GLEW_ARB_debug_output) {
		glDebugMessageCallbackARB(debugOutputCallback, nullptr);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	}

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	camera_ = new Camera;
	reflect_ = new Shader;
	refract_ = new Shader;

	std::string textures[] = {
		"textures/lake_skybox/right.jpg",
		"textures/lake_skybox/left.jpg",
		"textures/lake_skybox/top.jpg",
		"textures/lake_skybox/bottom.jpg",
		"textures/lake_skybox/back.jpg",
		"textures/lake_skybox/front.jpg",
	};

	skyBox_ = new SkyBox(camera_, textures);
	mesh_ = new Mesh;

	mesh_->Load("models/box.obj");

	reflect_->Load("shaders/reflect.glsl");
	reflect_->Link();

	refract_->Load("shaders/refract.glsl");
	refract_->Link();

	camera_->Reset(glm::vec3(0, 1, 5), glm::vec3(0));
}

void GLWidget::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
}

void RenderRefract() {
	glm::mat4 m;
	m[3] = glm::vec4(1.3f, 0, 0, 1);
	glm::mat4 mvp = camera_->GetProjMatrix() * camera_->GetViewMatrix() * m;
	glm::mat4 mv = camera_->GetViewMatrix() * m;

	refract_->SetUniform("MV", &mv);
	refract_->SetUniform("MVP", &mvp);

	refract_->SetUniform("cube", Globals::ColorTextureIndex);
	skyBox_->GetTexture()->Bind(Globals::ColorTexture);

	refract_->Bind();
	mesh_->Render();
}

void RenderReflect() {
	glm::mat4 m;
	m[3] = glm::vec4(-1.3f, 0, 0, 1);

	glm::mat4 mvp = camera_->GetProjMatrix() * camera_->GetViewMatrix() * m;
	glm::mat4 mv = camera_->GetViewMatrix() * m;

	reflect_->SetUniform("MV", &mv);
	reflect_->SetUniform("MVP", &mvp);

	reflect_->SetUniform("cube", Globals::ColorTextureIndex);
	skyBox_->GetTexture()->Bind(Globals::ColorTexture);

	reflect_->Bind();
	mesh_->Render();
}

void GLWidget::paintGL() {
	//RenderReflect();
	//RenderRefract();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skyBox_->Render();
}

void GLWidget::mousePressEvent(QMouseEvent *event) {

}
void GLWidget::mouseMoveEvent(QMouseEvent *event) {
}

void GLWidget::keyPressEvent(QKeyEvent* event) {
	switch (event->key()) {
	case Qt::Key_Escape:
		close();
		break;
	default:
		event->ignore();
		break;
	}
}

void APIENTRY debugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	// Shader.
	if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB) {
		return;
	}

	QString text = "OpenGL Debug Output message:\n";

	if (source == GL_DEBUG_SOURCE_API_ARB)					text += "Source: API.\n";
	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)	text += "Source: WINDOW_SYSTEM.\n";
	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)	text += "Source: SHADER_COMPILER.\n";
	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)		text += "Source: THIRD_PARTY.\n";
	else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)		text += "Source: APPLICATION.\n";
	else if (source == GL_DEBUG_SOURCE_OTHER_ARB)			text += "Source: OTHER.\n";

	if (type == GL_DEBUG_TYPE_ERROR_ARB)					text += "Type: ERROR.\n";
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)	text += "Type: DEPRECATED_BEHAVIOR.\n";
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)	text += "Type: UNDEFINED_BEHAVIOR.\n";
	else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)			text += "Type: PORTABILITY.\n";
	else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)			text += "Type: PERFORMANCE.\n";
	else if (type == GL_DEBUG_TYPE_OTHER_ARB)				text += "Type: OTHER.\n";

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)				text += "Severity: HIGH.\n";
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		text += "Severity: MEDIUM.\n";
	else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)			text += "Severity: LOW.\n";

	text += message;

	Q_ASSERT_X(severity != GL_DEBUG_SEVERITY_HIGH_ARB, "", text.toLatin1());

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB) {
		qCritical() << text;
	}
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB) {
		qWarning() << text;
	}
	else {
		qDebug() << text;
	}
}
