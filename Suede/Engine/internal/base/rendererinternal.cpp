#include "variables.h"
#include "renderstate.h"
#include "rendererinternal.h"

RendererInternal::RendererInternal(ObjectType type) : ObjectInternal(type), queue_(RenderQueueGeometry) {
	std::fill(states_, states_ + RenderStateCount, nullptr);
}

RendererInternal::~RendererInternal() {
	for (int i = 0; i < RenderStateCount; ++i) {
		Memory::Release(states_[i]);
	}
}

void RendererInternal::Render(Surface surface) {
	BindRenderStates();
	DrawCall(surface);
	UnbindRenderStates();
}

void RendererInternal::SetRenderState(RenderStateType type, RenderStateParameter parameter0, RenderStateParameter parameter1) {
	RenderState* state = nullptr;
	switch (type) {
		case Cull:
			state = Memory::Create<CullState>(parameter0);
			break;
		case DepthTest:
			state = Memory::Create<DepthTestState>(parameter0);
			break;
		case Blend:
			state = Memory::Create<BlendState>(parameter0, parameter1);
			break;
		case DepthWrite:
			state = Memory::Create<DepthWriteState>(parameter0);
			break;
		case RasterizerDiscard:
			state = Memory::Create<RasterizerDiscardState>(parameter0);
		default:
			Debug::LogError("invalid render capacity " + std::to_string(type));
			break;
	}

	Memory::Release(states_[type]);
	states_[type] = state;
}

void RendererInternal::DrawCall(Surface surface) {
	surface->Bind();
	for (int i = 0; i < surface->GetMeshCount(); ++i) {
		Mesh mesh = surface->GetMesh(i);

		for (int j = 0; j < GetMaterialCount(); ++j) {
			DrawMesh(mesh, GetMaterial(j));
		}
	}

	surface->Unbind();
}

void RendererInternal::DrawMesh(Mesh mesh, Material material) {
	MaterialTextures& textures = mesh->GetMaterialTextures();

	if (textures.bump) {
		material->SetTexture(Variables::bumpTexture, textures.bump);
	}

	if (textures.albedo) {
		material->SetTexture(Variables::mainTexture, textures.albedo);
	}

	if (textures.specular) {
		material->SetTexture(Variables::specularTexture, textures.specular);
	}

	material->Bind();
	unsigned vertexCount, baseVertex, baseIndex;
	mesh->GetTriangles(vertexCount, baseVertex, baseIndex);

	glDrawElementsBaseVertex(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned)* baseIndex), baseVertex);
	material->Unbind();
}

void RendererInternal::BindRenderStates() {
	for (int i = 0; i < RenderStateCount; ++i) {
		if (states_[i] != nullptr) {
			states_[i]->Bind();
		}
	}
}

void RendererInternal::UnbindRenderStates() {
	for (int i = 0; i < RenderStateCount; ++i) {
		if (states_[i] != nullptr) {
			states_[i]->Unbind();
		}
	}
}

void SkinnedSurfaceRendererInternal::Render(Surface surface) {
	for (int i = 0; i < GetMaterialCount(); ++i) {
		GetMaterial(i)->SetMatrix4(Variables::boneToRootSpaceMatrices, *skeleton_->GetBoneToRootSpaceMatrices());
	}

	RendererInternal::Render(surface);
}
