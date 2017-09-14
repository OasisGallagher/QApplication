#include <QDebug>
#include <QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <gl/glew.h>

#include "skybox.h"
#include "camera.h"
#include "shader.h"
#include "skybox.h"
#include "engine.h"
#include "texture.h"
#include "surface.h"
#include "GLWidget.h"

static void OnEngineLogReceived(int level, const char* message) {
	switch (level) {
	case LogLevelDebug:
		qDebug() << message;
		break;
	case LogLevelWarning:
		qWarning() << message;
		break;
	case LogLevelError:
	case LogLevelFatal:
		qFatal(message);
		break;
	}
}

GLWidget::GLWidget(QWidget *parent) 
	: QGLWidget(parent), mpressed_(false), lpressed(false) {
	setMouseTracking(true);
}

void GLWidget::initializeGL() {
	Engine::Ptr()->Initialize();
	Engine::Ptr()->SetDebugCallback(OnEngineLogReceived);

	World world = Engine::Ptr()->WorldPtr();
	Camera camera = dynamic_sp_cast<Camera>(world->Create("Camera"));
	camera->SetClearColor(glm::vec3(0.0f, 0.0f, 0.4f));

	Sprite sprite = dynamic_sp_cast<Sprite>(world->Create("Sprite"));
	sprite->SetPosition(glm::vec3(4, 1, -9));

	Surface surface = dynamic_sp_cast<Surface>(world->Create("Surface"));
	/* Mesh.
	Mesh mesh = dynamic_ptr_cast<Mesh>(world->Create("Mesh"));
	SurfaceAttribute attribute;
	attribute.positions.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	attribute.positions.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	attribute.positions.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	attribute.indices.push_back(0);
	attribute.indices.push_back(1);
	attribute.indices.push_back(2);

	surface->SetAttribute(attribute);
	mesh->SetTriangles(3, 0, 0);
	surface->AddMesh(mesh);
	*/

	surface->Load("models/suzanne.obj");
	MaterialTextures textures = surface->GetMesh(0)->GetMaterialTextures();
	Texture2D diffuse = dynamic_sp_cast<Texture2D>(world->Create("Texture2D"));
	diffuse->Load("textures/suzanne_uvmap.dds");
	textures.diffuse = diffuse;
	
	surface->GetMesh(0)->SetMaterialTextures(textures);

	sprite->SetSurface(surface);

	Renderer renderer = dynamic_sp_cast<Renderer>(world->Create("Renderer"));
	renderer->AddOption(RC_Cull, Back);
	renderer->AddOption(RC_DepthTest, Less);

	Shader shader = dynamic_sp_cast<Shader>(world->Create("Shader")); 
	shader->Load("buildin/shaders/texture");

	Material material = dynamic_sp_cast<Material>(world->Create("Material"));
	material->SetShader(shader);
	renderer->AddMaterial(material);

	sprite->SetRenderer(renderer);
}

void GLWidget::resizeGL(int w, int h) {
	Engine::Ptr()->OnResize(w, h);
}

void GLWidget::paintGL() {
	Engine::Ptr()->Update();
	QMetaObject::invokeMethod(this, "updateGL", Qt::QueuedConnection);
}

void GLWidget::wheelEvent(QWheelEvent* event) {
	//camera_->Zoom(-0.1f * 0.05f* event->delta());
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
		//camera_->Move(0.1f * 0.05f * glm::vec2(delta.x(), delta.y()));
	}

	if (lpressed) {
		QPoint delta = event->pos() - lpos_;
		lpos_ = event->pos();
		//camera_->Rotate(-0.1f * 0.005f * glm::vec2(delta.x(), delta.y()));
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
