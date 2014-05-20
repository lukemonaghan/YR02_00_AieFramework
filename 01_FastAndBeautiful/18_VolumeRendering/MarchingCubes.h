#pragma once

#include <glm\ext.hpp>

class MarchingCubes {
public:
	MarchingCubes(void);
	~MarchingCubes(void);
	// returns how many vertices it filled up (3x triangles).
	// a_cubeCount is the size of the cube grid.
	// a_cubeSize is the size of a single cube.
	// a_volume is a 3-D array of float volume data
	// a_volumeThreshold is is a value that determines if a point in the volume is within
	// or outside the surface of the volume.
	// a_vertices is an array of vertices that can contain a_maxVertices number of vertices.
	unsigned int march(const glm::ivec3& a_cubeCount, const glm::vec3& a_cubeSize,  float*** a_volume, float a_volumeThreshold, glm::vec3* a_vertices, unsigned int a_maxVertices);

private:

	// marches for a single cube, returning vertex count
	unsigned int marchCube(const glm::ivec3& a_cubeCorner, const glm::vec3& a_cubeSize,
				float*** a_volume, float a_volumeThreshold, glm::vec3* a_vertices);

	static const glm::ivec3 CUBE_CORNERS[8];
	static const int EDGE_INDICES[12][2];
	static const glm::vec3 EDGE_DIRECTIONS[12];
	static const int CUBE_EDGE_FLAGS[256];
	static const int TRIANGLE_CONNECTION_TABLE[256][16];
};

