#include <memory>
class Orbit;
using OrbitPtr = std::shared_ptr<Orbit>;

#ifndef ORBIT_H
#define ORBIT_H

#include "engine.h"
#include "transform.h"
class Orbit : public Engine
{
private:
	TransformPtr _orbit;
	float _speed;
protected:
	Orbit(TransformPtr orbit, float speed);
public:
	static OrbitPtr Make(TransformPtr orbit, float speed);
	virtual void Update(float dt);
};

#endif

