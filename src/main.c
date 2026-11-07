#define _USE_MATH_DEFINES

#include <stdio.h>      // input/output
#include <stdint.h>     // _t types
#include <stdbool.h>    // boolen
#include <stdlib.h>     // memory allocation (malloc)
#include <SDL3/SDL.h>   // SDL3 API

#include <upng.h>
#include <array.h>
#include <display.h>
#include <vector.h>
#include <matrix.h>
#include <light.h>
#include <mesh.h>
#include <triangle.h>
#include <texture.h>
#include <camera.h>
#include <clipping.h>

#define MAX_TRIANGLES 10000
triangle_t triangles_to_render[MAX_TRIANGLES];
int num_triangles_to_render = 0;

bool is_running = false;
uint64_t previous_frame_time = 0;
float delta_time = 0;

mat4_t world_matrix;
mat4_t proj_matrix;
mat4_t view_matrix;

void setup(void) {
	// initialize render mode and triangle culling method
	set_render_method(RENDER_WIRE);
	set_cull_method(CULL_BACKFACE);
	
	// initialize the perspective projection matrix
	float aspect_y = (float)get_window_height() / (float)get_window_width();
	float aspect_x = (float)get_window_width() / (float)get_window_height();
	float fov_y = (float)M_PI / 3.0f; // the same as 180/3, or 60 degrees
	float fov_x = atanf(tanf(fov_y / 2.0f) * aspect_x) * 2.0f;
	float z_near = 1.0f;
	float z_far = 20.0f;
	proj_matrix = mat4_make_perspective(fov_y, aspect_y, z_near, z_far);
	
	// initialize frustum planes with a point and normal
	init_frustum_planes(fov_x, fov_y, z_near, z_far);
	
	// loads the cube values in the mesh data structure
	//load_cube_mesh_data();
	load_obj_file_data(SDL_GetBasePath(), "f22.obj");
	load_png_texture_data(SDL_GetBasePath(), "f22.png");
}

void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT:
				is_running = false;
				break;
			case SDL_EVENT_KEY_DOWN:
				if (event.key.key == SDLK_ESCAPE) {
					is_running = false;
					break;
				}
				if (event.key.key == SDLK_1) {
					set_render_method(RENDER_WIRE_VERTEX);
					break;
				}
				if (event.key.key == SDLK_2) {
					set_render_method(RENDER_WIRE);
					break;
				}
				if (event.key.key == SDLK_3) {
					set_render_method(RENDER_FILL_TRIANGLE);
					break;
				}
				if (event.key.key == SDLK_4) {
					set_render_method(RENDER_FILL_TRIANGLE_WIRE);
					break;
				}
				if (event.key.key == SDLK_5) {
					set_render_method(RENDER_TEXTURED);
					break;
				}
				if (event.key.key == SDLK_6) {
					set_render_method(RENDER_TEXTURED_WIRE);
					break;
				}
				if (event.key.key == SDLK_C) {
					set_cull_method(CULL_BACKFACE);
					break;
				}
				if (event.key.key == SDLK_X) {
					set_cull_method(CULL_NONE);
					break;
				}
				if (event.key.key == SDLK_UP) {
					camera.position.y += 3.0f * delta_time;
					break;
				}
				if (event.key.key == SDLK_DOWN) {
					camera.position.y -= 3.0f * delta_time;
					break;
				}
				if (event.key.key == SDLK_A) {
					camera.yaw += 1.0f * delta_time;
					break;
				}
				if (event.key.key == SDLK_D) {
					camera.yaw -= 1.0f * delta_time;
					break;
				}
				if (event.key.key == SDLK_W) {
					camera.forward_velocity = vec3_mul(camera.direction, 5.0f * delta_time);
					camera.position = vec3_add(camera.position, camera.forward_velocity);
					break;
				}
				if (event.key.key == SDLK_S) {
					camera.forward_velocity = vec3_mul(camera.direction, 5.0f * delta_time);
					camera.position = vec3_sub(camera.position, camera.forward_velocity);
					break;
				}
				break;
		}
	}
}

void update(void) {
	// 60 fps
	uint64_t elapsed = SDL_GetTicks() - previous_frame_time;
	if(elapsed < FRAME_TARGET_TIME) {
		SDL_Delay((uint32_t)(FRAME_TARGET_TIME - elapsed));
	}
	
	// get a delta time factor converted to seconds to be used to update our game objects
	delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0f;
	
	previous_frame_time = SDL_GetTicks();
	
	// initialize the counter of triangles to render for the current frame
    num_triangles_to_render = 0;
	
	
	// change the mesh scale, rotation values per animation frame
	mesh.rotation.x += 0.0f * delta_time;
	mesh.rotation.y += 0.0f * delta_time;
	mesh.rotation.z += 0.0f * delta_time;
	mesh.translation.z = 5.0f;
	
	// initialize the target looking at the positive z-axis
	vec3_t target = { 0.0f, 0.0f, 1.0f };
	mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
	camera.direction = vec3_from_vec4(mat4_mul_vec4(camera_yaw_rotation, vec4_from_vec3(target)));

	// offset the camera position in the direction where the camera is pointing at
	target = vec3_add(camera.position, camera.direction);
	vec3_t up_direction = { 0.0f, 1.0f, 0.0f };
	
	// create the view matrix
	view_matrix = mat4_look_at(camera.position, target, up_direction);
	
	// create a scale, rotation, and translation matrices that will be used to multiply the mesh vertices
	mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);
	
	// loop all triangle faces of our mesh
	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++) {		
		face_t mesh_face = mesh.faces[i];
		
		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a];
		face_vertices[1] = mesh.vertices[mesh_face.b];
		face_vertices[2] = mesh.vertices[mesh_face.c];
		
		vec4_t transformed_vertices[3];
		
		// loop all three vertices of this current face and apply transformations
		for (int j = 0; j < 3; j++) {
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
			
			// create a world matrix combining scale, rotation, and translation matrices
			world_matrix = mat4_identity();
			
			// order matters, first scale, then rotate, then translate
			world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);
			
			// multiply the world matrix by the original vector
			transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);
			
			// multiply the view matrix by the vector to transform the scene to camera space
			transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);
			
			// save transformed vertex in the array of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}
		
		// backface culling test to see if the current face should be projected
		// check backface culling 				  (clock-wise)
		vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
		vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
		vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */
		
		// get the vector subtraction of B-A and C-A
		vec3_t vector_ab = vec3_sub(vector_b, vector_a);
		vec3_t vector_ac = vec3_sub(vector_c, vector_a);
		vec3_normalize(&vector_ab);
		vec3_normalize(&vector_ac);
		
		// compute the face normal (using cross product to find the perpendicular)
		vec3_t normal = vec3_cross(vector_ab, vector_ac);
		vec3_normalize(&normal);
		
		// find the vector between a point in the triangle and the camera origin
		vec3_t origin = { 0.0f, 0.0f, 0.0f };
		vec3_t camera_ray = vec3_sub(origin, vector_a);
		
		// calculate how aligned the camera ray is with the face normal (using dot product)
		float dot_normal_camera = vec3_dot(normal, camera_ray);
		
		if (is_cull_backface()) {
			// bypass the triangles that are looking away from the camera
			if (dot_normal_camera < 0) {
				continue;
			}
		}
		
		// create a polygon from the original transform triangle to be clipped
		polygon_t polygon = create_polygon_from_triangle(
			vec3_from_vec4(transformed_vertices[0]),
			vec3_from_vec4(transformed_vertices[1]),
			vec3_from_vec4(transformed_vertices[2]),
			mesh_face.a_uv,
			mesh_face.b_uv,
			mesh_face.c_uv
		);
		
		// clip the polygon and returns a new polygon with potential new vertices
		clip_polygon(&polygon);
		
		// break the clipping polygon apart back into individual triangles
		triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
		int num_triangles_after_clipping = 0;
		
		triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);
		
		// loops all the assembled triangles after clipping
		for (int t = 0; t < num_triangles_after_clipping; t++) {
			triangle_t triangle_after_clipping = triangles_after_clipping[t];
			
			vec4_t projected_points[3];
			
			// loop all three vertices to perform projection
			for (int j = 0; j < 3; j++) {
				// project the current vertex
				projected_points[j] = mat4_mul_vec4_project(proj_matrix, triangle_after_clipping.points[j]);
				
				// invert the y values to account for flipped screen y coordinate
				projected_points[j].y *= -1;
				
				// scale into the view
				projected_points[j].x *= (get_window_width() / 2.0f);
				projected_points[j].y *= (get_window_height() / 2.0f);
				
				// translate the projected points to the middle of the screen
				projected_points[j].x += (get_window_width() / 2.0f);
				projected_points[j].y += (get_window_height() / 2.0f);
			}
			
			// calculate the shade intensity based on how aligned is the normal and the inverse of the light ray
			float light_intensity_factor = -vec3_dot(normal, light.direction);
			
			// calculate the triangle color based on the light angle
			uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);
			
			triangle_t triangle_to_render = {
				.points = {
					{ projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
					{ projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
					{ projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w }
				},
				.texcoords = {
					{ triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v },
					{ triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v },
					{ triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v }
				},
				.color = triangle_color
			};
			
			// save the projected triangle in the array of triangle to render
			if (num_triangles_to_render < MAX_TRIANGLES) {
				triangles_to_render[num_triangles_to_render++] = triangle_to_render;
			}
		}
	}	
}

void render(void) {
	clear_color_buffer(0x525365FF);                             // set each pixel to yellow (RGBA)
    clear_z_buffer();
	
    draw_grid(0x333333FF);	
	
	// loop all projected triangles and render them
	for(int i = 0; i < num_triangles_to_render; i++) {
		triangle_t triangle = triangles_to_render[i];
		
		// draw filled triangle
		if (should_render_filled_triangles()) {
			draw_filled_triangle(
				(int)triangle.points[0].x, (int)triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
				(int)triangle.points[1].x, (int)triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
				(int)triangle.points[2].x, (int)triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
				triangle.color
			);
		}
		
		// draw textured triangle
		if (should_render_textured_triangles()) {
			draw_textured_triangle(
				(int)triangle.points[0].x, (int)triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
				(int)triangle.points[1].x, (int)triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
				(int)triangle.points[2].x, (int)triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
				mesh_texture
			);
		}
		
		// draw unfilled triangle (wireframe)
		if (should_render_wireframe()) {
			draw_triangle(
				(int)triangle.points[0].x, (int)triangle.points[0].y,
				(int)triangle.points[1].x, (int)triangle.points[1].y,
				(int)triangle.points[2].x, (int)triangle.points[2].y,
				0xFFFFFFFF
			);
		}
		
		// draw vertex points
		if (should_render_wire_vertex()) {
			draw_rect((int)triangle.points[0].x - 3, (int)triangle.points[0].y - 3, 6, 6, 0xFFFF00FF); // vertex A
			draw_rect((int)triangle.points[1].x - 3, (int)triangle.points[1].y - 3, 6, 6, 0xFFFF00FF); // vertex B
			draw_rect((int)triangle.points[2].x - 3, (int)triangle.points[2].y - 3, 6, 6, 0xFFFF00FF); // vertex C
		}
	}
	
	render_color_buffer();                                      // update & render texture
}

void free_resources(void) {
	upng_free(png_texture);
	png_texture = NULL;
	free(mesh_texture);
	mesh_texture = NULL;
	array_free(mesh.faces);
	mesh.faces = NULL;
	array_free(mesh.vertices);
	mesh.vertices = NULL;
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    is_running = initialize_window();
    
    setup();
	
    while(is_running) {
        process_input();
        update();
        render();
    }
    
    // clean-up
    destroy_window();
	free_resources();
    return 0;
}
