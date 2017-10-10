#include <QDebug>
#include <QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <gl/glew.h>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "canvas.h"
#include "light.h"
#include "skybox.h"
#include "camera.h"
#include "shader.h"
#include "skybox.h"
#include "engine.h"
#include "texture.h"
#include "surface.h"

#include "scripts/grayscale.h"
#include "scripts/cameracontroller.h"

// https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.cpp
glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest) {
	start = glm::normalize(start);
	dest = glm::normalize(dest);

	float cosTheta = glm::dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}

Canvas::Canvas(QWidget *parent) 
	: QGLWidget(parent), sceneCreated_(false) {
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	grayscale_ = new Grayscale;
	controller_ = new CameraController;
	timer_ = startTimer(10);
}

Canvas::~Canvas() {
	killTimer(timer_);
	delete grayscale_;
	delete controller_;
	Engine::get()->release();
}

void Canvas::initializeGL() { 
	// TODO: make debug context.
	Engine::get()->initialize();
	Engine::get()->setDebugCallback(this);
}

void Canvas::resizeGL(int w, int h) {
	Engine::get()->onResize(w, h);
}

void Canvas::paintGL() {
	if (!sceneCreated_) {
		createScene();
		sceneCreated_ = true;
	}

	Engine::get()->update();
	//QMetaObject::invokeMethod(this, "updateGL", Qt::QueuedConnection);
}

void Canvas::timerEvent(QTimerEvent *event) {
	if (event->timerId() == timer_) {
		update();
	}
}

void Canvas::wheelEvent(QWheelEvent* event) {
	controller_->onMouseWheel(event->delta());
}

void Canvas::mousePressEvent(QMouseEvent *event) {
	controller_->onMousePress(event->button(), event->pos());
	QGLWidget::mousePressEvent(event);
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
	controller_->onMouseRelease(event->button());
	QGLWidget::mouseReleaseEvent(event);
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
	controller_->onMouseMove(event->pos());
	QGLWidget::mouseMoveEvent(event);
}

void Canvas::keyPressEvent(QKeyEvent* event) {
	switch (event->key()) {
	case Qt::Key_Escape:
		qApp->quit();
		break;
	default:
		event->ignore();
		break;
	}
}

void Canvas::OnEngineLogMessage(int level, const char* message) {
	emit onEngineLogReceived(level, message);
}

void Canvas::createScene() {
	World world = Engine::get()->world();

	world->GetEnvironment()->SetAmbientColor(glm::vec3(0.15f));
	DirectionalLight light = dynamic_sp_cast<DirectionalLight>(world->Create(ObjectTypeDirectionalLight));
	light->SetColor(glm::vec3(0.7f));
	light->SetPosition(glm::vec3(0, 7, 5));
	//glm::quat cq(-glm::lookAt(glm::vec3(0, 7, 5), glm::vec3(0), glm::vec3(0, 1, 0)));
	//light->SetRotation(cq);

	Camera camera = dynamic_sp_cast<Camera>(world->Create(ObjectTypeCamera));
	controller_->setCamera(camera);
	camera->AddPostEffect(grayscale_);
	//camera->SetPosition(glm::vec3(0, 1, 5));
	
	//glm::quat q(glm::lookAt(glm::vec3(0, 1, 5), glm::vec3(0), glm::vec3(0, 1, 0)));
	//camera->SetRotation(q);

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
	//sprite->SetEulerAngles(glm::vec3(0, 180, 0));

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
	Texture2D albedo = dynamic_sp_cast<Texture2D>(world->Create(ObjectTypeTexture2D));
	albedo->Load("textures/suzanne_uvmap.dds");

	MaterialTextures& textures = surface->GetMesh(0)->GetMaterialTextures();
	textures.albedo = albedo;

	sprite->SetSurface(surface);

	Renderer renderer = dynamic_sp_cast<Renderer>(world->Create(ObjectTypeRenderer));
	renderer->SetRenderState(Cull, Off);
	renderer->SetRenderState(DepthTest, Less);

	Shader shader = dynamic_sp_cast<Shader>(world->Create(ObjectTypeShader));
	shader->Load("buildin/shaders/texture");

	Material material = dynamic_sp_cast<Material>(world->Create(ObjectTypeMaterial));
	material->SetShader(shader);
	renderer->AddMaterial(material);

	sprite->SetRenderer(renderer);
}
