#include <clipping.h>
#include <math.h>

#define NUM_PLANES 6
plane_t frustum_planes[NUM_PLANES];

void init_frustum_planes(float fov, float z_near, float z_far) {
	float cos_half_fov = cosf(fov / 2.0f);
	float sin_half_fov = sinf(fov / 2.0f);
	
	frustum_planes[LEFT_FRUSTUM_PLANE].point = vec3_new(0.0f, 0.0f, 0.0f);
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0.0f;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0.0f, 0.0f, 0.0f);
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0.0f;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[TOP_FRUSTUM_PLANE].point = vec3_new(0.0f, 0.0f, 0.0f);
	frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0.0f;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vec3_new(0.0f, 0.0f, 0.0f);
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0.0f;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0.0f, 0.0f, z_near);
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0.0f;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0.0f;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1.0f;

	frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0.0f, 0.0f, z_far);
	frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0.0f;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0.0f;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1.0f;
}