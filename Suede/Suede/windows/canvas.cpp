#include <QDebug>
#include <QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <gl/glew.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "canvas.h"
#include "suede.h"

#include "light.h"
#include "skybox.h"
#include "camera.h"
#include "shader.h"
#include "skybox.h"
#include "engine.h"
#include "texture.h"
#include "surface.h"

#include "scripts/grayscale.h"
#include "scripts/inversion.h"
#include "scripts/cameracontroller.h"

Canvas::Canvas(QWidget *parent) 
	: QGLWidget(parent), sceneCreated_(false) {
	setMouseTracking(true);
	grayscale_ = new Grayscale;
	inversion_ = new Inversion;
	controller_ = new CameraController;
	timer_ = startTimer(10);
}

Canvas::~Canvas() {
	killTimer(timer_);
	delete grayscale_;
	delete inversion_;
	delete controller_;
	Engine::get()->release();
}

void Canvas::initializeGL() { 
	// TODO: make debug context.
	Engine::get()->initialize();
	Engine::get()->setLogCallback(this);
}

void Canvas::resizeGL(int w, int h) {
	Engine::get()->resize(w, h);
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
	}
}

void Canvas::OnEngineLogMessage(int level, const char* message) {
	emit onEngineLogReceived(level, message);
}

void Canvas::createScene() {
	World world = Engine::get()->world();

	world->GetEnvironment()->SetAmbientColor(glm::vec3(0.15f));
	DirectionalLight light = dsp_cast<DirectionalLight>(world->Create(ObjectTypeDirectionalLight));
	light->SetColor(glm::vec3(0.7f));

	Camera camera = dsp_cast<Camera>(world->Create(ObjectTypeCamera));
	controller_->setCamera(camera);

	//camera->AddPostEffect(inversion_);
	//camera->AddPostEffect(grayscale_);
	//camera->SetPosition(glm::vec3(0, 1, 5));
	
	//glm::quat q(glm::lookAt(glm::vec3(0, 1, 5), glm::vec3(0), glm::vec3(0, 1, 0)));
	//camera->SetRotation(q);

	camera->SetPosition(glm::vec3(0, 25, 0));

	camera->SetClearType(ClearTypeSkybox);
	//camera->SetClearColor(glm::vec3(0, 0, 0.4f));

	Skybox skybox = dsp_cast<Skybox>(world->Create(ObjectTypeSkybox));
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
	
	RenderTexture renderTexture = dsp_cast<RenderTexture>(world->Create(ObjectTypeRenderTexture));

	renderTexture->Load(RenderTextureFormatRgba, width(), height());
	//camera->SetRenderTexture(renderTexture);
	//camera->SetClearColor(glm::vec3(0.0f, 0.0f, 0.4f));

	Sprite sprite = dsp_cast<Sprite>(world->Create(ObjectTypeSprite));
	//sprite->SetParent(camera);
	//light->SetParent(camera);
	sprite->SetPosition(glm::vec3(0, 0, -70));
	sprite->SetEulerAngles(glm::vec3(270, 180, 180));

	/* Mesh.
	Mesh mesh = dynamic_ptr_cast<Mesh>(world->Create("Mesh"));
	SurfaceAttribute attribute;
	attribute.positions.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	attribute.positions.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	attribute.positions.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	attribute.indexes.push_back(0);
	attribute.indexes.push_back(1);
	attribute.indexes.push_back(2);

	surface->SetAttribute(attribute);
	mesh->SetTriangles(3, 0, 0);
	surface->AddMesh(mesh);
	*/
	
	sprite->LoadModel("models/boblampclean.md5mesh");
	sprite->GetAnimation()->SetWrapMode(AnimationWrapModeOnce);
	sprite->GetAnimation()->Play("");
	//Surface surface = sprite->GetSurface();

	//Texture2D albedo = dsp_cast<Texture2D>(world->Create(ObjectTypeTexture2D));
	//albedo->Load("textures/room_uvmap.dds");

	//Texture2D bump = dsp_cast<Texture2D>(world->Create(ObjectTypeTexture2D));
	//bump->Load("textures/bump.bmp");

	//MaterialTextures& textures = surface->GetMesh(0)->GetMaterialTextures();
	//textures.albedo = albedo;
	//textures.bump = bump;

	/*Renderer renderer = sprite->GetRenderer();
	renderer->SetRenderState(Cull, Off);
	renderer->SetRenderState(DepthTest, LessEqual);

	Shader shader = dsp_cast<Shader>(world->Create(ObjectTypeShader));
	shader->Load("buildin/shaders/lit_texture");
	renderer->GetMaterial(0)->SetShader(shader);*/
}
