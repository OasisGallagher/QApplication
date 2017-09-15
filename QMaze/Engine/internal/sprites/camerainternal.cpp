#include <glm/gtc/matrix_transform.hpp>

#include "engine.h"
#include "camera.h"
#include "tools/debug.h"
#include "internal/base/variables.h"
#include "internal/memory/factory.h"
#include "internal/resources/resources.h"
#include "internal/base/shaderinternal.h"
#include "internal/base/textureinternal.h"
#include "internal/base/materialinternal.h"
#include "internal/base/rendererinternal.h"
#include "internal/sprites/camerainternal.h"

CameraInternal::CameraInternal() : SpriteInternal(ObjectTypeCamera), clearType_(ClearTypeColor) {
	aspect_ = 1.3f;
	near_ = 1.f;
	far_ = 100.f;
	fieldOfView_ = 3.141592f / 3.f;

	projection_ = glm::perspective(fieldOfView_, aspect_, near_, far_);

	pass_ = RenderPassNone;

	glGenFramebuffers(1, &depthFramebuffer_);
	depthTexture_ = Factory::Create<RenderTextureInternal>();

	// TODO: restore framebuffer.
	GLint oldFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFramebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer_);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	depthTexture_->Load(RenderTextureFormatDepth, viewport[2], viewport[3]);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture_, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, oldFramebuffer);

	// 
	glGenFramebuffers(1, &renderTextureFramebuffer_);

	glClearDepth(1);
}

CameraInternal::~CameraInternal() {
	glDeleteFramebuffers(1, &depthFramebuffer_);
	glDeleteFramebuffers(1, &renderTextureFramebuffer_);
}

void CameraInternal::SetClearType(ClearType value) {
	clearType_ = value;
}

ClearType CameraInternal::GetClearType() {
	return clearType_;
}

void CameraInternal::SetSkybox(Skybox value) {
	skybox_ = value;
}

Skybox CameraInternal::GetSkybox() {
	return skybox_;
}

void CameraInternal::SetClearColor(const glm::vec3& value) {
	glClearColor(value.r, value.g, value.b, 1);
}

void CameraInternal::SetRenderTexture(RenderTexture value) {
	renderTexture_ = value;

	// TODO: restore framebuffer.
	GLint oldFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFramebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, renderTextureFramebuffer_);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture_->GetNativePointer(), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, oldFramebuffer);
}

void CameraInternal::Update() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (clearType_ == ClearTypeSkybox && skybox_) {
		skybox_->SetPosition(GetPosition());
	}

	// TODO: get world instance.
	World world = Engine::Ptr()->WorldPtr();
	std::vector<Sprite> sprites;
	if (!world->CollectSprites(&sprites, fieldOfView_, aspect_, near_, far_)) {
		return;
	}

	SortRenderableSprites(sprites);

	/*int (CameraInternal::*passes[RenderPassCount])(std::vector<Sprite>&, int) = {
		&CameraInternal::RenderBackgroundPass,
		&CameraInternal::RenderDepthPass,
		&CameraInternal::RenderOpaquePass,
		&CameraInternal::RenderTransparentPass,
	};*/

	GLint oldFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFramebuffer);
	
	int from = 0;
	from = RenderBackgroundPass(sprites, from);
	from = RenderDepthPass(sprites, from);

	glBindFramebuffer(GL_FRAMEBUFFER, renderTexture_ ? renderTextureFramebuffer_ : 0);

	from = RenderOpaquePass(sprites, from);
	from = RenderTransparentPass(sprites, from);

	/*for (int i = RenderPassBackground; i < RenderPassCount; ++i) {
		pass_ = (RenderPass)i;
		from = (this->*passes[i])(sprites, from);
	}*/

	glBindFramebuffer(GL_READ_FRAMEBUFFER, renderTextureFramebuffer_);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, 1024, 768, 0, 0, 1024, 768, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	pass_ = RenderPassNone;
}

int CameraInternal::RenderBackgroundPass(std::vector<Sprite>& sprites, int from) {
	pass_ = RenderPassBackground;
	return 0;
}

int CameraInternal::RenderDepthPass(std::vector<Sprite>& sprites, int from) {
	pass_ = RenderPassDepth;

	// TODO: restore framebuffer.
	GLint oldFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFramebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer_);

	for (int i = 0; i < sprites.size(); ++i) {
		Sprite sprite = sprites[i];
		RenderSprite(sprite);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, oldFramebuffer);

	return from;
}

int CameraInternal::RenderOpaquePass(std::vector<Sprite>& sprites, int from) {
	pass_ = RenderPassOpaque;
	for (int i = 0; i < sprites.size(); ++i) {
		Sprite sprite = sprites[i];
		RenderSprite(sprite);
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

void CameraInternal::RenderSprite(Sprite sprite) {
	Surface surface = sprite->GetSurface();
	if (!surface) {
		Debug::LogError("No surface to render");
		return;
	}

	Renderer renderer = sprite->GetRenderer();

	Material material = renderer->GetMaterial(0);

	glm::mat4 matrix = projection_ * GetWorldToLocalMatrix() * sprite->GetLocalToWorldMatrix();
	material->SetMatrix(Variables::modelToClipSpaceMatrix, matrix);
	
	renderer->Render(surface);
}

const glm::mat4& CameraInternal::GetProjectionMatrix() {
	return projection_;
}
