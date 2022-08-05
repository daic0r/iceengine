#pragma once

namespace Ice {

class Animation
{
protected:
	int m_nDuration;
    
public:
	Animation() = default;
    virtual ~Animation() {}
    
	virtual void animate(int nCurrentTime) = 0;
	virtual bool done() = 0;
};

}
