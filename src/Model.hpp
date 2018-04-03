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

	private:
		ID3D11InputLayout *m_VertexLayout;
		D3D11_INPUT_ELEMENT_DESC m_VertexElts[64];

		std::wstring m_VertexShader, m_PixelShader;
};

