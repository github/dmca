class Camera
{
public:
	rw::Camera *m_rwcam;
	rw::V3d m_position;
	rw::V3d m_target;
	rw::V3d m_up;
	rw::V3d m_localup;

	float m_fov, m_aspectRatio;
	float m_near, m_far;


	void setTarget(rw::V3d target);
	float getHeading(void);

	void turn(float yaw, float pitch);
	void orbit(float yaw, float pitch);
	void dolly(float dist);
	void zoom(float dist);
	void pan(float x, float y);

	void update(void);
	float distanceTo(rw::V3d v);
	Camera(void);
};
