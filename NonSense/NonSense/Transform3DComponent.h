#include "Component.h"

class Transform3DComponent : public Component
{
public:
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 direction = XMFLOAT3(0.0f, 0.0f, -1.0f);

	XMFLOAT3 velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float pitch = 0.0f; // x 각 모델 좌표계 기준
	float yaw = 0.0f; // y각
	float roll = 0.0f; // z각
	~Transform3DComponent() {}
	void start();
	void update();
};
