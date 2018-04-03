#pragma once

#include "Primitive.hpp"

class Model : public Primitive {
	public:
		Model(ID3D11Device *device, DXShader shader);
		~Model();

		void init() {}
		bool load(std::string file, bool tangents = false);

		void setCullMode(D3D11_CULL_MODE cull);
		void setBlend(int blend);
		void setDepth(ID3D11DepthStencilState *depth);
		void setSkin(std::string tex) { m_Shader->setTexture(tex); }
		void setNormalMap(std::string tex) { m_Shader->setNormalMap(tex); }
		void setSpecularMap(std::string tex) { m_Shader->setSpecularMap(tex); }
		void setParallaxMap(std::string tex) { m_Shader->setParallaxMap(tex); }

	private:
		ID3D11InputLayout *m_VertexLayout;
		D3D11_INPUT_ELEMENT_DESC m_VertexElts[64];

		std::wstring m_VertexShader, m_PixelShader;
};

