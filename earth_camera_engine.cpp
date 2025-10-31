#include "earth_camera_engine.h"

Earth_camera_engine::Earth_camera_engine(NodePtr earth_node, NodePtr moon_node, Camera3DPtr earth_cam) : _earth_node(earth_node), _moon_node(moon_node), _earth_cam(earth_cam)
{
}

Earth_camera_enginePtr Earth_camera_engine::Make(NodePtr earth_trf, NodePtr moon_trf, Camera3DPtr earth_cam) {
	return Earth_camera_enginePtr(new Earth_camera_engine(earth_trf, moon_trf, earth_cam));
}

void Earth_camera_engine::Update(float dt)
{/*
	glm::mat4 mm = _moon_node->GetModelMatrix();
	glm::mat4 em = _earth_node->GetModelMatrix();
	_earth_cam->SetCenter(mm[0][0], 0, mm[2][2]);
	_earth_cam->SetEye(em[0][0], 0, em[2][2]);*/
}
