#pragma once
#include <memory>
class Earth_camera_engine;
using Earth_camera_enginePtr = std::shared_ptr<Earth_camera_engine>;

#ifndef EARTH_CAMERA_ENGINE_H
#define EARTH_CAMERA_ENGINE_H

#include "engine.h"
#include "transform.h"
class Earth_camera_engine : public Engine
{
private:
	TransformPtr _earth_camera_engine;
	float _speed;
protected:
	Earth_camera_engine(TransformPtr earth_camera_engine, float speed);
public:
	static Earth_camera_enginePtr Make(TransformPtr earth_camera_engine, float speed);
	virtual void Update(float dt);
};

#endif

