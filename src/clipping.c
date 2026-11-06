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

polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2) {
	polygon_t polygon = {
		.vertices = { v0, v1, v2 },
		.num_vertices = 3
	};
	return polygon;
}

void clip_polygon_against_plane(polygon_t *polygon, int plane) {
	vec3_t plane_point = frustum_planes[plane].point;
	vec3_t plane_normal = frustum_planes[plane].normal;
	
	// declare a static array of inside vertices that will be part of the final polygon returned via parameter
	vec3_t inside_vertices[MAX_NUM_POLY_VERTICES];
	int num_inside_vertices = 0;
	
	// start the current vertex with the first polygon vertex, and the previous with the last polygon vertex
	vec3_t *current_vertex = &polygon->vertices[0];
	vec3_t *previous_vertex = &polygon->vertices[polygon->num_vertices - 1];
	
	// calculate the dot product of the current and previous vertex
	float current_dot = 0;
	float previous_dot = vec3_dot(vec3_sub(*previous_vertex, plane_point), plane_normal);
	
	// loop all the polygon vertices while the current is different than the last one
	while (current_vertex != &polygon->vertices[polygon->num_vertices]) {
		current_dot = vec3_dot(vec3_sub(*current_vertex, plane_point), plane_normal);
		
		// if we changed from inside to outside or from outside to inside
		if (current_dot * previous_dot < 0) {
			// find the interpolation factor t
			float t = previous_dot / (previous_dot - current_dot);
			// calculate the intersection point I = Q1 + t(Q2-Q1)
			vec3_t intersection_point = vec3_clone(current_vertex);					// I =   Qc
			intersection_point = vec3_sub(intersection_point, *previous_vertex);	// I =  (Qc - Qp)
			intersection_point = vec3_mul(intersection_point, t);					// I = t(Qc - Qp)
			intersection_point = vec3_add(intersection_point, *previous_vertex);	// I = Qp + t(Qc - Qp)
			
			// insert the intersection point to the list of ""inside vertices"
			inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
			num_inside_vertices++;
		}
		
		// current vertex is inside the plane
		if (current_dot > 0) {
			// insert the current vertex to the list of "side vertices"
			inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
			num_inside_vertices++;
		}
		
		// move to the next vertex
		previous_dot = current_dot;
		previous_vertex = current_vertex;
		current_vertex++;
	}
	
	// at the end, copy the list of inside vertices into the destination polygon (outparameter)
	for (int i = 0; i < num_inside_vertices; i++) {
		polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
	}
	polygon->num_vertices = num_inside_vertices;
}

void clip_polygon(polygon_t *polygon) {
	clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}

