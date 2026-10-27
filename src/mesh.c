#include <mesh.h>
#include <array.h>
#include <string.h>
#include <stdio.h>

mesh_t mesh = {
	.vertices = NULL,
	.faces = NULL,
	.rotation = { 0.0f, 0.0f, 0.0f, },
	.scale = { 1.0f, 1.0f, 1.0f, },
	.translation = { 0.0f, 0.0f, 0.0f, }
};


vec3_t cube_vertices[N_CUBE_VERTICES] = {
	{ .x = -1, .y = -1, .z = -1 }, 	// 1
	{ .x = -1, .y =  1, .z = -1 }, 	// 2
	{ .x =  1, .y =  1, .z = -1 }, 	// 3
	{ .x =  1, .y = -1, .z = -1 }, 	// 4
	{ .x =  1, .y =  1, .z =  1 }, 	// 5
	{ .x =  1, .y = -1, .z =  1 }, 	// 6
	{ .x = -1, .y =  1, .z =  1 }, 	// 7
	{ .x = -1, .y = -1, .z =  1 } 	// 8
};

face_t cube_faces[N_CUBE_FACES] = {
	// front
	{ .a = 1, .b = 2, .c = 3, .color = 0xFF0000FF },
	{ .a = 1, .b = 3, .c = 4, .color = 0xFF0000FF },
	
	// right
	{ .a = 4, .b = 3, .c = 5, .color = 0x00FF00FF },
	{ .a = 4, .b = 5, .c = 6, .color = 0x00FF00FF },
	
	// back
	{ .a = 6, .b = 5, .c = 7, .color = 0x0000FFFF },
	{ .a = 6, .b = 7, .c = 8, .color = 0x0000FFFF },
	
	// left
	{ .a = 8, .b = 7, .c = 2, .color = 0xFFFF00FF },
	{ .a = 8, .b = 2, .c = 1, .color = 0xFFFF00FF },
	
	// top
	{ .a = 2, .b = 7, .c = 5, .color = 0xFF00FFFF },
	{ .a = 2, .b = 5, .c = 3, .color = 0xFF00FFFF },
	
	// bottom
	{ .a = 6, .b = 8, .c = 1, .color = 0x00FFFFFF },
	{ .a = 6, .b = 1, .c = 4, .color = 0x00FFFFFF }
};

void load_cube_mesh_data(void) {
	for (int i = 0; i < N_CUBE_VERTICES; i++) {
		vec3_t cube_vertex = cube_vertices[i];
		array_push(mesh.vertices, cube_vertex);
	}
	for (int i = 0; i < N_CUBE_FACES; i++) {
		face_t cube_face = cube_faces[i];
		array_push(mesh.faces, cube_face);
	}
}

void load_obj_file_data(const char *base_path, const char* filename) {
	char full_path[1024];
	snprintf(full_path, sizeof full_path, "%s%s", base_path, filename);
	
	FILE *file;
	file = fopen(full_path, "r");
	if (!file) {
		printf("failed to open file");
	}
	char line[1024];
	
	while (fgets(line, 1024, file)) {
		// vertex information
		// compare line with "v" and how many characters
		// if it returns 0 both are equal
		if (strncmp(line, "v ", 2) == 0) {
			vec3_t vertex;
			sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
			array_push(mesh.vertices, vertex);
		}
		
		// face information
		if (strncmp(line, "f ", 2) == 0) {
			int vertex_indices[3];
			int texture_indices[3];
			int normal_indices[3];
			sscanf(line, 
				"f %d/%d/%d %d/%d/%d %d/%d/%d", 
				&vertex_indices[0], &texture_indices[0], &normal_indices[0],
				&vertex_indices[1], &texture_indices[1], &normal_indices[1],
				&vertex_indices[2], &texture_indices[2], &normal_indices[2]
			);
			face_t face = {
				.a = vertex_indices[0],
				.b = vertex_indices[1],
				.c = vertex_indices[2]
			};
			array_push(mesh.faces, face);
		}
	}
}