#include <glm/gtc/matrix_transform.hpp>

#include "light.h"
#include "engine.h"
#include "camera.h"
#include "variables.h"
#include "tools/debug.h"
#include "internal/memory/factory.h"
#include "internal/base/framebuffer.h"
#include "internal/resources/resources.h"
#include "internal/base/shaderinternal.h"
#include "internal/base/textureinternal.h"
#include "internal/base/materialinternal.h"
#include "internal/base/rendererinternal.h"
#include "internal/sprites/camerainternal.h"

CameraInternal::CameraInternal() 
	: SpriteInternal(ObjectTypeCamera), clearType_(ClearTypeColor)
	, depth_(0), aspect_(1.3f), near_(1.f), far_(100.f)
	, fieldOfView_(3.141592f / 3.f), projection_(glm::perspective(fieldOfView_, aspect_, near_, far_))
	, pass_(RenderPassNone), fbRenderTexture_(nullptr) {
	CreateFramebuffers();
	CreateDepthRenderer();
	glClearDepth(1);
}

CameraInternal::~CameraInternal() {
	Memory::Release(fb0_);
	Memory::Release(fbDepth_);
	Memory::Release(fbRenderTexture_);
}

void CameraInternal::SetClearColor(const glm::vec3 & value) {
	fb0_->SetClearColor(value);
}

glm::vec3 CameraInternal::GetClearColor() {
	return fb0_->GetClearColor();
}

void CameraInternal::SetRenderTexture(RenderTexture value) {
	if (value) {
		fbRenderTexture_->SetRenderTexture(value);
	}
	else {
		fbRenderTexture_->SetRenderTexture(renderTexture_);
	}
}

void CameraInternal::Update() {
	if (clearType_ == ClearTypeSkybox) {
		Skybox skybox = skybox_;
		if (!skybox) {
			skybox = Engine::get()->world()->GetEnvironment()->GetSkybox();
		}

		if (!skybox) {
			Debug::LogError("skybox does not exist.");
			return;
		}

		skybox->SetPosition(GetPosition());
	}
}

void CameraInternal::Render() {
	std::vector<Sprite> sprites;
	if (!GetRenderableSprites(sprites)) {
		return;
	}

	RenderDepthPass(sprites);

	Light forwardBase;
	std::vector<Light> forwardAdd;
	GetLights(forwardBase, forwardAdd);

	Framebuffer0* active = GetActiveFramebuffer();
	active->Bind();

	if (forwardBase) {
		RenderForwardBase(sprites, forwardBase);
	}

	if (!forwardAdd.empty()) {
		RenderForwardAdd(sprites, forwardAdd);
	}

	OnPostRender();

	active->Unbind();

	pass_ = RenderPassNone;
}

void CameraInternal::CreateFramebuffers() {
	int w = Engine::get()->screen()->GetContextWidth();
	int h = Engine::get()->screen()->GetContextHeight();

	fb0_ = Memory::Create<Framebuffer0>();
	fb0_->Create(w, h);

	fbDepth_ = Memory::Create<Framebuffer>();
	fbDepth_->Create(w, h);
	RenderTexture depthTexture = Factory::Create<RenderTextureInternal>();
	depthTexture->Load(Depth, w, h);
	fbDepth_->SetDepthTexture(depthTexture);

	fbRenderTexture_ = Memory::Create<Framebuffer>();
	fbRenderTexture_->Create(w, h);
	renderTexture_ = Factory::Create<RenderTextureInternal>();
	renderTexture_->Load(Rgba, w, h);
	fbRenderTexture_->SetRenderTexture(renderTexture_);
	
	fbRenderTexture2_ = Memory::Create<Framebuffer>();
	fbRenderTexture2_->Create(w, h);
	renderTexture2_ = Factory::Create<RenderTextureInternal>();
	renderTexture2_->Load(Rgba, w, h);
	fbRenderTexture2_->SetRenderTexture(renderTexture2_);
}

void CameraInternal::CreateDepthRenderer() {
	depthRenderer_ = Factory::Create<RendererInternal>();
	Shader shader = Factory::Create<ShaderInternal>();
	shader->Load("buildin/shaders/depth");

	Material material = Factory::Create<MaterialInternal>();
	material->SetShader(shader);

	depthRenderer_->AddMaterial(material);
	depthRenderer_->SetRenderState(DepthWrite, On);
	depthRenderer_->SetRenderState(DepthTest, Less);
}

Framebuffer0* CameraInternal::GetActiveFramebuffer() {
	Framebuffer0* active = nullptr;
	if (fbRenderTexture_->GetRenderTexture(0) != renderTexture_ || !postEffects_.empty()) {
		active = fbRenderTexture_;
	}
	else {
		active = fb0_;
	}

	return active;
}

void CameraInternal::SetForwardBaseLightParameter(std::vector<Sprite>& sprites, Light light) {
	for (int i = 0; i < sprites.size(); ++i) {
		Renderer renderer = sprites[i]->GetRenderer();
		int materialCount = renderer->GetMaterialCount();
		for (int i = 0; i < materialCount; ++i) {
			Material material = renderer->GetMaterial(i);

			material->SetVector3(Variables::cameraPosition, GetPosition());
			material->SetVector3(Variables::ambientLightColor, Engine::get()->world()->GetEnvironment()->GetAmbientColor());
			material->SetVector3(Variables::lightColor, light->GetColor());
			material->SetVector3(Variables::lightPosition, light->GetPosition());
			material->SetVector3(Variables::lightDirection, light->GetRotation() * glm::vec3(0, 0, -1));
		}
	}
}

void CameraInternal::RenderForwardBase(std::vector<Sprite>& sprites, Light light) {
	SetForwardBaseLightParameter(sprites, light);

	int from = 0;
	from = RenderBackgroundPass(sprites, from);
	from = RenderOpaquePass(sprites, from);
	from = RenderTransparentPass(sprites, from);
}

void CameraInternal::RenderForwardAdd(std::vector<Sprite>& sprites, std::vector<Light>& lights) {

}

int CameraInternal::RenderBackgroundPass(std::vector<Sprite>& sprites, int from) {
	pass_ = RenderPassBackground;
	return 0;
}

void CameraInternal::RenderDepthPass(std::vector<Sprite>& sprites) {
	pass_ = RenderPassDepth;
	
	fbDepth_->Bind();

	for (int i = 0; i < sprites.size(); ++i) {
		Sprite sprite = sprites[i];
		RenderSprite(sprite, depthRenderer_);
	}

	fbDepth_->Unbind();
}

int CameraInternal::RenderOpaquePass(std::vector<Sprite>& sprites, int from) {
	pass_ = RenderPassOpaque;
	for (int i = 0; i < sprites.size(); ++i) {
		Sprite sprite = sprites[i];
		RenderSprite(sprite, sprite->GetRenderer());
	}

	return 0;
}

int CameraInternal::RenderTransparentPass(std::vector<Sprite>& sprites, int from) {
	pass_ = RenderPassTransparent;
	return 0;
}

void CameraInternal::GetLights(Light& forwardBase, std::vector<Light>& forwardAdd) {
	World world = Engine::get()->world();
	std::vector<Sprite> lights;
	if (!world->GetSprites(ObjectTypeLights, lights)) {
		return;
	}

	forwardBase = dsp_cast<Light>(lights.front());
	for (int i = 1; i < lights.size(); ++i) {
		forwardAdd.push_back(dsp_cast<Light>(lights[i]));
	}
}

void CameraInternal::OnPostRender() {
	if (postEffects_.empty()) { return; }

	Framebuffer* framebuffers[] = { fbRenderTexture_, fbRenderTexture2_ };
	RenderTexture textures[] = { fbRenderTexture_->GetRenderTexture(0), renderTexture2_ };

	int index = 1;
	for (int i = 0; i < postEffects_.size(); ++i) {
		Framebuffer0* active = framebuffers[index];
		
		if (i + 1 == postEffects_.size()) {
			active = fb0_;
			textures[index].reset();
		}

		active->Bind();
		postEffects_[i]->OnRenderImage(textures[1 - index], textures[index]);
		active->Unbind();

		index = 1 - index;
	}

	fb0_->Unbind();
}

bool CameraInternal::IsRenderable(Sprite sprite) {
	return sprite->GetSurface() && sprite->GetRenderer();
}

bool CameraInternal::GetRenderableSprites(std::vector<Sprite>& sprites) {
	World world = Engine::get()->world();
	world->GetSprites(ObjectTypeSprite, sprites);

	// sort sprites by render queue.
	int p = 0;
	for (int i = 0; i < sprites.size(); ++i) {
		Sprite key = sprites[i];
		if (!IsRenderable(key)) {
			continue;
		}

		int j = p - 1;

		for (; j >= 0 && sprites[j]->GetRenderer()->GetRenderQueue() > key->GetRenderer()->GetRenderQueue(); --j) {
			sprites[j + 1] = sprites[j];
		}

		sprites[j + 1] = key;
		++p;
	}

	sprites.erase(sprites.begin() + p, sprites.end());
	return !sprites.empty();
}

void CameraInternal::RenderSprite(Sprite sprite, Renderer renderer) {
	Surface surface = sprite->GetSurface();
	
	Material material = renderer->GetMaterial(0);
	glm::mat4 localToWorldMatrix = sprite->GetLocalToWorldMatrix();
	glm::mat4 localToClipSpaceMatrix = projection_ * GetWorldToLocalMatrix() * localToWorldMatrix;
	material->SetMatrix4(Variables::localToClipSpaceMatrix, localToClipSpaceMatrix);
	material->SetMatrix4(Variables::localToWorldSpaceMatrix, localToWorldMatrix);
	renderer->Render(surface);
}

const glm::mat4& CameraInternal::GetProjectionMatrix() {
	return projection_;
}
