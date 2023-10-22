///////////////////////////////////////////////////////////////////////////////
// meshes.h
// ========
// create meshes for various 3D primitives: plane, pyramid, cube, cylinder, torus, sphere
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 7th, 2022
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

class Meshes
{
	// Stores the GL data relative to a given mesh
	struct GLMesh
	{
		GLuint vao;         // Handle for the vertex array object
		GLuint vbos[2];     // Handles for the vertex buffer objects
		GLuint nVertices;	// Number of vertices for the mesh
		GLuint nIndices;    // Number of indices for the mesh
	};

public:
	GLMesh gBoxMesh;
	GLMesh gPlaneMesh;

public:
	void CreateMeshes();
	void DestroyMeshes();

private:
	void UCreatePlaneMesh(GLMesh& mesh);
	void UCreateBoxMesh(GLMesh& mesh);


	void UDestroyMesh(GLMesh& mesh);

	void CalculateTriangleNormal(glm::vec3 px, glm::vec3 py, glm::vec3 pz);
}; 
