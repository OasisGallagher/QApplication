#include <glm/gtc/matrix_transform.hpp>

#include "engine.h"
#include "camera.h"
#include "tools/debug.h"
#include "internal/base/variables.h"
#include "internal/resources/resources.h"
#include "internal/base/shaderinternal.h"
#include "internal/base/materialinternal.h"
#include "internal/base/rendererinternal.h"
#include "internal/sprites/camerainternal.h"

CameraInternal::CameraInternal() : SpriteInternal(ObjectTypeCamera), clearType_(ClearTypeColor) {
	aspect_ = 1.3f;
	near_ = 1.f;
	far_ = 100.f;
	fieldOfView_ = 3.141592f / 3.f;

	projection_ = glm::perspective(fieldOfView_, aspect_, near_, far_);

	glClearDepth(1);
}

CameraInternal::~CameraInternal() {
}

void CameraInternal::SetClearType(ClearType type) {
	clearType_ = type;
}

ClearType CameraInternal::GetClearType() {
	return clearType_;
}

void CameraInternal::SetSkybox(Skybox skybox) {
	skybox_ = skybox;
}

Skybox CameraInternal::GetSkybox() {
	return skybox_;
}

void CameraInternal::SetClearColor(const glm::vec3& color) {
	glClearColor(color.r, color.g, color.b, 1);
}

void CameraInternal::Update() {
	ClearFramebuffer();

	World world = Engine::Ptr()->WorldPtr();
	std::vector<Sprite> sprites;
	if (!world->CollectSprites(&sprites, fieldOfView_, aspect_, near_, far_)) {
		return;
	}

	for (int i = 0; i < sprites.size(); ++i) {
		Sprite sprite = sprites[i];
		if (sprite->GetRenderer() && sprite->GetSurface()) {
			RenderSprite(sprite);
		}
	}
}

void CameraInternal::RenderSprite(Sprite sprite) {
	Surface surface = sprite->GetSurface();
	Renderer renderer = sprite->GetRenderer();

	Material material = renderer->GetMaterial(0);

	glm::mat4 matrix = projection_ * GetWorldToLocalMatrix() * sprite->GetLocalToWorldMatrix();
	material->SetMatrix(Variables::modelToClipSpaceMatrix, matrix);
	
	renderer->Render(surface);
}

void CameraInternal::ClearFramebuffer() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (clearType_ == ClearTypeSkybox && skybox_) {
		RenderSprite(skybox_);
	}
}

const glm::mat4& CameraInternal::GetProjectionMatrix() {
	return projection_;
}
