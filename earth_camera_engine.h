#pragma once
#include <memory>
class Earth_camera_engine;
using Earth_camera_enginePtr = std::shared_ptr<Earth_camera_engine>;

#ifndef EARTH_CAMERA_ENGINE_H
#define EARTH_CAMERA_ENGINE_H

#include "engine.h"
#include "transform.h"
#include "camera3d.h"
class Earth_camera_engine : public Engine
{
private:
	NodePtr _earth_node;
	NodePtr _moon_node;
	Camera3DPtr _earth_cam;
protected:
	Earth_camera_engine(NodePtr earth_trf, NodePtr moon_trf, Camera3DPtr earth_cam);
public:
	static Earth_camera_enginePtr Make(NodePtr earth_trf, NodePtr moon_trf, Camera3DPtr earth_cam);
	virtual void Update(float dt);
};

#endif

