#include <memory>
class ClipPlane;
using ClipPlanePtr = std::shared_ptr<ClipPlane>;

#ifndef CLIPPLANE_H
#define CLIPPLANE_H
#include "appearance.h"
class ClipPlane : public Appearance {
	std::string m_name;
	std::vector<glm::vec4> m_planes;
	bool active = true;
protected:
	ClipPlane(const std::string& name);
	ClipPlane(const std::string& name,
		float a, float b, float c, float d);
public:
	static ClipPlanePtr Make(const std::string& name);
	static ClipPlanePtr Make(const std::string& name,
		float a, float b, float c, float d);
	virtual ~ClipPlane();
	void AddPlane(float a, float b, float c, float d);
	void toggleClipping();
	virtual void Load(StatePtr st);
	virtual void Unload(StatePtr st);
};

#endif