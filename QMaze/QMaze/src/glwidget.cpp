#include <QDebug>
#include <QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <gl/glew.h>

#include "light.h"
#include "skybox.h"
#include "camera.h"
#include "shader.h"
#include "skybox.h"
#include "engine.h"
#include "texture.h"
#include "surface.h"
#include "glwidget.h"
#include "cameracontroller.h"

// TODO: console class.
#include "qmaze.h"
#include <set>

static void OnEngineLogReceived(int level, const char* message) {
	static std::set<std::string> logs;
	if (logs.find(std::to_string(level) + message) != logs.end()) {
		return;
	}

	logs.insert(std::to_string(level) + message);

	switch (level) {
	case LogLevelDebug:
		QMaze::get()->addConsoleMessage(QString("<font color='#000000'>%1</font>").arg(message));
		qDebug() << message;
		break;

	case LogLevelWarning:
		QMaze::get()->addConsoleMessage(QString("<font color='#ff9912'>%1</font>").arg(message));
		qWarning() << message;
		break;

	case LogLevelError:
		QMaze::get()->addConsoleMessage(QString("<font color='#FF0000'>%1</font>").arg(message));
		qCritical() << message;
		break;

	case LogLevelFatal:
		QMaze::get()->addConsoleMessage(QString("%1: <font color='#FF0000'>%2</font>").arg(message));
		qFatal(message);
		break;
	}
}

GLWidget::GLWidget(QWidget *parent) 
	: QGLWidget(parent), sceneCreated_(false) {
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	controller_ = new CameraController;
}

GLWidget::~GLWidget() {
	delete controller_;
	Engine::get()->release();
}

void GLWidget::initializeGL() { 
	// TODO: make debug context.
	Engine::get()->initialize();
	Engine::get()->setDebugCallback(OnEngineLogReceived);
}

void GLWidget::resizeGL(int w, int h) {
	Engine::get()->onResize(w, h);
}

void GLWidget::paintGL() {
	if (!sceneCreated_) {
		createScene();
		sceneCreated_ = true;
	}

	Engine::get()->update();
	QMetaObject::invokeMethod(this, "updateGL", Qt::QueuedConnection);
}

void GLWidget::wheelEvent(QWheelEvent* event) {
	controller_->onMouseWheel(event->delta());
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
	controller_->onMousePress(event->button(), event->pos());
	QGLWidget::mousePressEvent(event);
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event) {
	controller_->onMouseRelease(event->button());
	QGLWidget::mouseReleaseEvent(event);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
	controller_->onMouseMove(event->pos());
	QGLWidget::mouseMoveEvent(event);
}

void GLWidget::keyPressEvent(QKeyEvent* event) {
	switch (event->key()) {
	case Qt::Key_Escape:
		qApp->quit();
		break;
	default:
		event->ignore();
		break;
	}
}

void GLWidget::createScene() {
	World world = Engine::get()->world();

	world->GetEnvironment()->SetAmbientColor(glm::vec3(0.15f));
	DirectionalLight light = dynamic_sp_cast<DirectionalLight>(world->Create(ObjectTypeDirectionalLight));
	light->SetColor(glm::vec3(0.8f));
	light->SetPosition(glm::vec3(0));

	Camera camera = dynamic_sp_cast<Camera>(world->Create(ObjectTypeCamera));
	controller_->setCamera(camera);

	camera->SetClearType(ClearTypeColor);
	camera->SetClearColor(glm::vec3(0, 0, 0.4f));

	/*Skybox skybox = dynamic_sp_cast<Skybox>(world->Create(ObjectTypeSkybox));
	std::string faces[] = {
		"textures/lake_skybox/right.jpg",
		"textures/lake_skybox/left.jpg",
		"textures/lake_skybox/top.jpg",
		"textures/lake_skybox/bottom.jpg",
		"textures/lake_skybox/back.jpg",
		"textures/lake_skybox/front.jpg",
	};

	skybox->Load(faces);
	camera->SetSkybox(skybox);
	*/

	RenderTexture renderTexture = dynamic_sp_cast<RenderTexture>(world->Create(ObjectTypeRenderTexture));

	renderTexture->Load(Rgba, width(), height());
	//camera->SetRenderTexture(renderTexture);
	//camera->SetClearColor(glm::vec3(0.0f, 0.0f, 0.4f));

	Sprite sprite = dynamic_sp_cast<Sprite>(world->Create(ObjectTypeSprite));
	sprite->SetPosition(glm::vec3(0, 0, -4));
	sprite->SetEulerAngles(glm::vec3(0, 30, 0));

	Surface surface = dynamic_sp_cast<Surface>(world->Create(ObjectTypeSurface));
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
	Texture2D diffuse = dynamic_sp_cast<Texture2D>(world->Create(ObjectTypeTexture2D));
	diffuse->Load("textures/suzanne_uvmap.dds");
	textures.diffuse = diffuse;

	surface->GetMesh(0)->SetMaterialTextures(textures);

	sprite->SetSurface(surface);

	Renderer renderer = dynamic_sp_cast<Renderer>(world->Create(ObjectTypeRenderer));
	renderer->SetRenderState(Cull, Back);
	renderer->SetRenderState(DepthTest, Less);

	Shader shader = dynamic_sp_cast<Shader>(world->Create(ObjectTypeShader));
	shader->Load("buildin/shaders/texture");

	Material material = dynamic_sp_cast<Material>(world->Create(ObjectTypeMaterial));
	material->SetShader(shader);
	renderer->AddMaterial(material);

	sprite->SetRenderer(renderer);
}
