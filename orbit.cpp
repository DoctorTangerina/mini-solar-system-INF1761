#include "orbit.h"

Orbit::Orbit(TransformPtr orbit, float speed) : _orbit(orbit), _speed(speed)
{
}

OrbitPtr Orbit::Make(TransformPtr orbit, float speed) {
	return OrbitPtr(new Orbit(orbit, speed));
}

void Orbit::Update(float dt)
{
	_orbit->Rotate((-dt / 30.0f * 180.0f) * _speed, 0, 0, 1);
}
