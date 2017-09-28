#pragma once
#include <glm/glm.hpp>

#include "camera.h"
#include "skybox.h"
#include "posteffect.h"
#include "internal/base/objectinternal.h"
#include "internal/sprites/spriteinternal.h"

class Framebuffer;
class Framebuffer0;

class CameraInternal : public ICamera, public SpriteInternal {
	DEFINE_FACTORY_METHOD(Camera)
	
	enum RenderPass {
		RenderPassNone = -1,
		RenderPassBackground,
		RenderPassDepth,
		RenderPassOpaque,
		RenderPassTransparent,
		RenderPassCount
	};

public:
	CameraInternal();
	~CameraInternal();

public:
	virtual void SetDepth(int value) { depth_ = value; }
	virtual int GetDepth() const { return depth_;  }

	virtual void SetClearType(ClearType value) { clearType_ = value; }
	virtual ClearType GetClearType() { return clearType_; }

	virtual void SetSkybox(Skybox value) { skybox_ = value; }
	virtual Skybox GetSkybox() { return skybox_; }

	virtual void SetClearColor(const glm::vec3& value) { clearColor_ = value; }
	virtual glm::vec3 GetClearColor() const { return clearColor_; }

	virtual void SetRenderTexture(RenderTexture value);
	virtual RenderTexture GetRenderTexture() { return renderTexture_; }

public:
	virtual void Update();
	virtual void Render();

public:
	virtual void SetAspect(float value) { aspect_ = value; }
	virtual void SetNearClipPlane(float value) { near_ = value; }
	virtual void SetFarClipPlane(float value) { far_ = value; }
	virtual void SetFieldOfView(float value) { near_ = value; }

	virtual float GetAspect() const { return aspect_; }
	virtual float GetNearClipPlane() const { return near_; }
	virtual float GetFarClipPlane() const { return far_; }
	virtual float GetFieldOfView() const { return fieldOfView_; }

	virtual const glm::mat4& GetProjectionMatrix();

private:
	void CreateFramebuffers();
	Framebuffer0* GetActiveFramebuffer();

	void RenderDepthPass(std::vector<Sprite>& sprites);
	int RenderBackgroundPass(std::vector<Sprite>& sprites, int from);
	int RenderOpaquePass(std::vector<Sprite>& sprites, int from);
	int RenderTransparentPass(std::vector<Sprite>& sprites, int from);

	bool IsRenderable(Sprite sprite);

	void RenderSprite(Sprite sprite, Renderer renderer);
	bool GetRenderableSprites(std::vector<Sprite>& sprites);

	void SetForwardBaseLightParameter(std::vector<Sprite>& sprites, Light light);

	void RenderForwardBase(std::vector<Sprite>& sprites, Light light);
	void RenderForwardAdd(std::vector<Sprite>& sprites, std::vector<Light>& lights);

	void OnPostRender();
	void GetLights(Light& forwardBase, std::vector<Light>& forwardAdd);

private:
	int depth_;

	float aspect_;
	float near_, far_;
	float fieldOfView_;
	glm::vec3 clearColor_;
	glm::mat4 projection_;

	Framebuffer0* fb0_;
	Framebuffer* fbDepth_;
	Framebuffer* fbRenderTexture_;
	RenderTexture renderTexture_;
	RenderTexture tempRenderTexture_;

	std::vector<PostEffect*> postEffects_;

	RenderPass pass_;

	Skybox skybox_;
	ClearType clearType_;
};
