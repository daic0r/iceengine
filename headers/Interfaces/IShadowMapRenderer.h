#pragma once

namespace Ice {

class IShadowMapRenderer {
public:
	virtual void renderShadowDepthTexture() const noexcept = 0;
	virtual void clear() noexcept = 0;
};

}