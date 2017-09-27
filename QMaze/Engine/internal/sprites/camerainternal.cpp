#include <glm/gtc/matrix_transform.hpp>

#include "engine.h"
#include "camera.h"
#include "tools/debug.h"
#include "internal/base/variables.h"
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

	fbDepth_ = Memory::Create<Framebuffer>();
	int w = Engine::get()->contextWidth();
	int h = Engine::get()->contextHeight();
	fbDepth_->Create(w, h);
	RenderTexture depthTexture = Factory::Create<RenderTextureInternal>();
	depthTexture->Load(Depth, w, h);
	fbDepth_->SetDepthTexture(depthTexture);

	fbRenderTexture_ = Memory::Create<Framebuffer>();
	fbRenderTexture_->Create(w, h);
	renderTexture_ = Factory::Create<RenderTextureInternal>();
	renderTexture_->Load(Rgba, w, h);
	fbRenderTexture_->SetRenderTexture(renderTexture_);

	tempRenderTexture_ = Factory::Create<RenderTextureInternal>();
	tempRenderTexture_->Load(Rgba, w, h);

	glClearDepth(1);
}

CameraInternal::~CameraInternal() {
	Memory::Release(fbDepth_);
	Memory::Release(fbRenderTexture_);
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
	if (clearType_ == ClearTypeSkybox && skybox_) {
		skybox_->SetPosition(GetPosition());
	}
}

void CameraInternal::Render(std::vector<Sprite>& sprites) {
	glClearColor(clearColor_.r, clearColor_.g, clearColor_.b, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	World world = Engine::get()->world();

	SortRenderableSprites(sprites);

	RenderDepthPass(sprites);

	bool renderToTexture = false;
	if (fbRenderTexture_->GetRenderTexture(0) != renderTexture_ || !postEffects_.empty()) {
		fbRenderTexture_->Bind();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		renderToTexture = true;
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int from = 0;
	from = RenderBackgroundPass(sprites, from);
	from = RenderOpaquePass(sprites, from);
	from = RenderTransparentPass(sprites, from);

	RenderTexture mainTexture = fbRenderTexture_->GetRenderTexture(0);
	RenderTexture tempTexture = tempRenderTexture_;

	for (int i = 0; i < postEffects_.size(); ++i) {
		if (i + 1 == postEffects_.size()) {
			tempTexture.reset();
		}

		postEffects_[i]->OnRenderImage(mainTexture, tempTexture);
		mainTexture.swap(tempTexture);
	}

	if (renderToTexture) {
		fbRenderTexture_->Unbind();
	}

	pass_ = RenderPassNone;
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
		RenderSprite(sprite, sprite->GetRenderer());
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

void CameraInternal::SortRenderableSprites(std::vector<Sprite>& sprites) {
	// sort sprites by render queue.
	int p = 0;
	for (int i = 0; i < sprites.size(); ++i) {
		Sprite key = sprites[i];
		if (!key->GetRenderer()) {
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
}

void CameraInternal::RenderSprite(Sprite sprite, Renderer renderer) {
	Surface surface = sprite->GetSurface();
	if (!surface) {
		Debug::LogError("No surface to render");
		return;
	}

	Material material = renderer->GetMaterial(0);
	glm::mat4 matrix = projection_ * GetWorldToLocalMatrix() * sprite->GetLocalToWorldMatrix();
	material->SetMatrix(Variables::modelToClipSpaceMatrix, matrix);
	renderer->Render(surface);
}

const glm::mat4& CameraInternal::GetProjectionMatrix() {
	return projection_;
}
