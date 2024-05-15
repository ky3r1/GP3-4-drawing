#include "character.h"

DirectX::XMFLOAT4X4 Character::get_world_transform()
{
	DirectX::XMMATRIX C, S, R, T;
	DirectX::XMFLOAT4X4 world;
	C = { DirectX::XMLoadFloat4x4(&coordinate_system_transforms[coordinate_system]) * DirectX::XMMatrixScaling(
		scale_factor,
		scale_factor,
		scale_factor) };

	S = { DirectX::XMMatrixScaling(
		scale.x,
		scale.y,
		scale.z) };

	R = { DirectX::XMMatrixRotationRollPitchYaw(
		rotate.x,
		rotate.y,
		rotate.z) };

	T = { DirectX::XMMatrixTranslation(
		position.x,
		position.y,
		position.z) };

	DirectX::XMStoreFloat4x4(&world, C * S * R * T);
	return 	world;
}
