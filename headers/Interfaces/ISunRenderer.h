#pragma once

namespace Ice {

struct RenderEnvironment;

class ISunRenderer {
public:
	virtual void render(const RenderEnvironment&) const noexcept = 0;
	virtual ~ISunRenderer() = default;
};

}