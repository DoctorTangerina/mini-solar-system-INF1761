#include "earth_camera_engine.h"

Earth_camera_engine::Earth_camera_engine(TransformPtr earth_camera_engine, float speed) : _earth_camera_engine(earth_camera_engine), _speed(speed)
{
}

Earth_camera_enginePtr Earth_camera_engine::Make(TransformPtr earth_camera_engine, float speed) {
	return Earth_camera_enginePtr(new Earth_camera_engine(earth_camera_engine, speed));
}

void Earth_camera_engine::Update(float dt)
{
	_earth_camera_engine->Rotate((-dt / 30.0f * 180.0f) * _speed, 0, -1, 0);
}
