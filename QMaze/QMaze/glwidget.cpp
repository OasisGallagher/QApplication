#include <QDebug>
#include <QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <gl/glew.h>

#include "mesh.h"
#include "defs.h"
#include "skybox.h"
#include "camera.h"
#include "shader.h"
#include "skybox.h"
#include "texture.h"
#include "GLWidget.h"

#include <environment.h>

static void OnEngineLogReceived(LogLevel level, const std::string& message) {
	switch (level) {
	case LogLevelDebug:
		qDebug() << message.c_str();
		break;
	case LogLevelWarning:
		qWarning() << message.c_str();
		break;
	case LogLevelError:
	case LogLevelFatal:
		qFatal(message.c_str());
		break;
	}
}

Shader* reflect_, *refract_;
SkyBox* skyBox_;
GLuint vao_, vbo_;
Camera* camera_;
Mesh* mesh_;

GLWidget::GLWidget(QWidget *parent) 
	: QGLWidget(parent), mpressed_(false), lpressed(false) {
	setMouseTracking(true);
}

void GLWidget::initializeGL() {
	Environment::Initialize(OnEngineLogReceived);

	glClearDepth(1);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

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

	mesh_->Load("models/cube.obj");

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderReflect();
	RenderRefract();
	skyBox_->Render();

	QMetaObject::invokeMethod(this, "updateGL", Qt::QueuedConnection);
}

void GLWidget::wheelEvent(QWheelEvent* event) {
	camera_->Zoom(-0.1f * 0.05f* event->delta());
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::MiddleButton) {
		mpos_ = event->pos();
		mpressed_ = true;
	}
	if (event->button() == Qt::LeftButton) {
		lpos_ = event->pos();
		lpressed = true;
	}

	QGLWidget::mousePressEvent(event);
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::MiddleButton) {
		mpressed_ = false;
	}
	else if (event->button() == Qt::LeftButton) {
		lpressed = false;
	}

	QGLWidget::mouseReleaseEvent(event);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
	if (mpressed_) {
		QPoint delta = event->pos() - mpos_;
		mpos_ = event->pos();
		camera_->Move(0.1f * 0.05f * glm::vec2(delta.x(), delta.y()));
	}

	if (lpressed) {
		QPoint delta = event->pos() - lpos_;
		lpos_ = event->pos();
		camera_->Rotate(-0.1f * 0.005f * glm::vec2(delta.x(), delta.y()));
	}

	QGLWidget::mouseMoveEvent(event);
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
