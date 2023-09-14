//https://www.glfw.org/docs/latest/quick.html
//http://graphics.stanford.edu/data/3Dscanrep/

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>      // C++ file IO (secret: it's a wraper for the c IO)
#include <sstream>      // like a string builder


//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
#include <string>

//static const struct
//{
//    float x, y;
//    float r, g, b;
//} vertices[3] =
//{
//    { -0.6f, -0.4f, 1.f, 0.f, 0.f },        // Red
//    {  0.6f, -0.4f, 0.f, 1.f, 0.f },        // Green
//    {   0.f,  0.6f, 0.f, 0.f, 1.f }         // Blue
//};




struct sVertex
{
	float x, y, z;      // vec2
	float r, g, b;      // vec3
};

//const unsigned int NUM_OF_VERTICES = 6;
//sVertex vertices[NUM_OF_VERTICES] =
//{
//    { -0.6f, -0.4f, 0.0f,   1.0f, 0.0f, 0.0f },        // Red
//    {  0.6f, -0.4f, 0.0f,   0.0f, 1.0f, 0.0f },        // Green
//    {   0.f,  0.6f, 0.0f,   0.0f, 0.0f, 1.0f },         // Blue
//    /* Same, just +1 in the x axis */
//    {  0.4f, -0.4f, 0.0f,   0.0f, 1.0f, 0.0f },        // Red
//    {  1.6f, -0.4f, 0.0f,   0.0f, 0.0f, 1.0f },        // Green
//    {  1.0f,  0.6f, 0.0f,   1.0f, 0.0f, 0.0f }         // Blue
//};

unsigned int g_NumberOfVerticesToDraw = 0;
sVertex* pVertices = NULL;      // 0 or nullptr

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec3 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

glm::vec3 cameraEye = glm::vec3(0.0, 0.1, -0.25f);


static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

// This callback is from the "typing" windows system
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	const float CAMERA_MOVEMENT_SPEED = 0.05f;

	if (key == GLFW_KEY_A && action)
	{
		cameraEye.x -= CAMERA_MOVEMENT_SPEED;
	}
	if (key == GLFW_KEY_D && action)
	{
		cameraEye.x += CAMERA_MOVEMENT_SPEED;
	}

	if (key == GLFW_KEY_W && action)
	{
		cameraEye.z += CAMERA_MOVEMENT_SPEED;
	}
	if (key == GLFW_KEY_S && action)
	{
		cameraEye.z -= CAMERA_MOVEMENT_SPEED;
	}


	if (key == GLFW_KEY_Q && action)
	{
		cameraEye.y -= CAMERA_MOVEMENT_SPEED;
	}
	if (key == GLFW_KEY_E && action)
	{
		cameraEye.y += CAMERA_MOVEMENT_SPEED;
	}
	return;
}

// This most closely matches the ply file for the bunny
struct sVertexPlyFile
{
	float x;
	float y;
	float z;
	float nx, ny, nz;
	float r, g, b, a;
};

struct sTrianglePlyFile
{
	//    sVertexPlyFile verts[3];
	//    unsigned int vertIndexes[3];
		// Vertices of the triangles
	unsigned int v0, v1, v2;
};


unsigned int g_numberOfVertices = 0;
unsigned int g_numberOfTriangles = 0;

bool LoadTheFile_PlyXYZ(std::string theFileName)
{
	//    property float x
	//    property float y
	//    property float z

		//sVertexPlyFile p;       p.x = 0.0f;     p.y = 1.0f; p.z = 2.0f;
		//std::cout << p.x;

		//sVertexPlyFile q;
		//std::cout << "Type in the x: ";
		//std::cin >> q.x;

		// Input Filestream 
	std::ifstream theBunnyFile("bun_zipper_res2_xyz.ply");
	if (!theBunnyFile.is_open())
	{
		// didn't open the file.
		return false;
	}

	std::string temp;
	while (theBunnyFile >> temp)
	{
		if (temp == "vertex")
		{
			break;
		}
	};
	//element vertex 8171
	theBunnyFile >> g_numberOfVertices;


	while (theBunnyFile >> temp)
	{
		if (temp == "face")
		{
			break;
		}
	};
	//element vertex 8171
	theBunnyFile >> g_numberOfTriangles;

	while (theBunnyFile >> temp)
	{
		if (temp == "end_header")
		{
			break;
		}
	};


	// Allocate enough space to hold the vertices
//    sVertex vertices[8171];                 // Stack

	sVertex x;      // STACK based variable (on the stack)
	sVertex* px;    // Pointer variable.

	//    int y = 5;
	//
	//    int* py = new int();
	//    *py = 5;


		// Dynamically allocate memory on the heap;
	sVertexPlyFile* pTheVerticesFile = new sVertexPlyFile[g_numberOfVertices];

	// -0.036872 0.127727 0.00440925 
	for (unsigned int index = 0; index != g_numberOfVertices; index++)
	{
		sVertexPlyFile tempVertex;
		theBunnyFile >> tempVertex.x;                //std::cin >> a.x;
		theBunnyFile >> tempVertex.y;                //std::cin >> a.y;
		theBunnyFile >> tempVertex.z;                //std::cin >> a.z;

		pTheVerticesFile[index] = tempVertex;
	}


	sTrianglePlyFile* pTheTriangles = new sTrianglePlyFile[g_numberOfTriangles];

	// 3 3495 3549 3548 
	for (unsigned int index = 0; index != g_numberOfTriangles; index++)
	{
		sTrianglePlyFile tempTriangle;

		unsigned int discard;
		theBunnyFile >> discard;            // 3
		theBunnyFile >> tempTriangle.v0;                //std::cin >> a.x;
		theBunnyFile >> tempTriangle.v1;                //std::cin >> a.y;
		theBunnyFile >> tempTriangle.v2;                //std::cin >> a.z;

		pTheTriangles[index] = tempTriangle;
	}


	// Now allocate the array that the shader is going to use
//    g_NumberOfVerticesToDraw = 6;
//    pVertices = new sVertex[g_NumberOfVerticesToDraw];
//
//    pVertices[0] = { -0.6f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f };
//    pVertices[1] = { 0.6f, -0.4f, 0.0f,   0.0f, 1.0f, 0.0f };
//    pVertices[2] = { 0.f,  0.6f, 0.0f,   0.0f, 0.0f, 1.0f };
//    pVertices[3] = { 0.4f, -0.4f, 0.0f,   0.0f, 1.0f, 0.0f };
//    pVertices[4] = { 1.6f, -0.4f, 0.0f,   0.0f, 0.0f, 1.0f };
//    pVertices[5] = { 1.0f,  0.6f, 0.0f,   1.0f, 0.0f, 0.0f };

//    g_NumberOfVerticesToDraw = g_numberOfVertices;
//    pVertices = new sVertex[g_NumberOfVerticesToDraw];
//
//    // Copy from the array we filled from the file to the array that that 
//    //  shader (video card) will use...
//
//    for (unsigned int index = 0; index != g_numberOfVertices; index++)
//    {
//        // Left is the array going to the video
//        // Right is the array from the file
//        pVertices[index].x = pTheVerticesFile[index].x;
//        pVertices[index].y = pTheVerticesFile[index].y;
//        pVertices[index].z = pTheVerticesFile[index].z;
//
//        pVertices[index].r = 1.0f;
//        pVertices[index].g = 1.0f;
//        pVertices[index].b = 1.0f;
//    }
// 
	g_NumberOfVerticesToDraw = g_numberOfTriangles * 3;
	pVertices = new sVertex[g_NumberOfVerticesToDraw];
	//std::cout << "Rounding: " << round(1.5f) << std::endl;
	unsigned int vertIndex = 0;
	float minAxisDist = 10.0f;
	float spotRange = 0.02f;
	const unsigned int spotCount = 50;
	unsigned int spots[spotCount] = {};
	for (unsigned int spotIndex = 0; spotIndex < spotCount; spotIndex++) {
		spots[spotIndex] = rand() % 8000;//g_numberOfVertices;
	}
	for (unsigned int triIndex = 0; triIndex != g_numberOfTriangles; triIndex++)
	{
		// 3 1582 1581 2063 
		pVertices[vertIndex + 0].x = pTheVerticesFile[pTheTriangles[triIndex].v0].x;
		pVertices[vertIndex + 0].y = pTheVerticesFile[pTheTriangles[triIndex].v0].y;
		pVertices[vertIndex + 0].z = pTheVerticesFile[pTheTriangles[triIndex].v0].z;

		minAxisDist = 10.0f;
		for (unsigned int spotIndex = 0; spotIndex < spotCount; spotIndex++) { // Scan through marked spot vertices to get distance from each spot, keeping lowest distance and setting color from that
			if (glm::length(glm::vec3(pVertices[vertIndex + 0].x - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].x, pVertices[vertIndex + 0].y - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].y, pVertices[vertIndex + 0].z - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].z)) < minAxisDist) {
				minAxisDist = glm::length(glm::vec3(pVertices[vertIndex + 0].x - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].x, pVertices[vertIndex + 0].y - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].y, pVertices[vertIndex + 0].z - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].z));
			}
		}
		if (minAxisDist < spotRange) // Subtract axis dist by spot range then ABS to get desired outcome
		{
			pVertices[vertIndex + 0].r = 1.0f; //- abs((minAxisDist - spotRange) * (1 / spotRange));
			pVertices[vertIndex + 0].g = 1.0f - abs((minAxisDist - spotRange) * (1 / spotRange));
			pVertices[vertIndex + 0].b = 1.0f;
		}
		else
		{
			pVertices[vertIndex + 0].r = 1.0f;
			pVertices[vertIndex + 0].g = 1.0f;
			pVertices[vertIndex + 0].b = 1.0f;
		}
		//         if (abs(((int)round(pVertices[vertIndex].y * 100)) % 2) == 1) {
		// 			pVertices[vertIndex + 0].r = 0.0f;
		// 			pVertices[vertIndex + 0].g = 0.5f;
		// 			pVertices[vertIndex + 0].b = 0.5f;
		//         }
		//         else {
		// 			pVertices[vertIndex + 0].r = 0.5f;
		// 			pVertices[vertIndex + 0].g = 0.0f;
		// 			pVertices[vertIndex + 0].b = 0.5f;
		//         }
		// 		if (abs(((int)round(pVertices[vertIndex].z * 100 + pVertices[vertIndex].x * 100)) % 2) == 1) {
		// 			pVertices[vertIndex + 0].r += 0.5f;
		// 			pVertices[vertIndex + 0].g += 0.5f;
		// 			pVertices[vertIndex + 0].b += 0.0f;
		// 		}
		// 		else {
		// 			pVertices[vertIndex + 0].r += 0.0f;
		// 			pVertices[vertIndex + 0].g += 0.2f;
		// 			pVertices[vertIndex + 0].b += 0.5f;
		// 		}
		//         pVertices[vertIndex + 0].r = 1.0f;
		//         pVertices[vertIndex + 0].g = 1.0f;
		//         pVertices[vertIndex + 0].b = 1.0f;

		pVertices[vertIndex + 1].x = pTheVerticesFile[pTheTriangles[triIndex].v1].x;
		pVertices[vertIndex + 1].y = pTheVerticesFile[pTheTriangles[triIndex].v1].y;
		pVertices[vertIndex + 1].z = pTheVerticesFile[pTheTriangles[triIndex].v1].z;

		minAxisDist = 10.0f;
		for (unsigned int spotIndex = 0; spotIndex < spotCount; spotIndex++) { // Scan through marked spot vertices to get distance from each spot, keeping lowest distance and setting color from that
			if (glm::length(glm::vec3(pVertices[vertIndex + 1].x - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].x, pVertices[vertIndex + 1].y - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].y, pVertices[vertIndex + 1].z - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].z)) < minAxisDist) {
				minAxisDist = glm::length(glm::vec3(pVertices[vertIndex + 1].x - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].x, pVertices[vertIndex + 1].y - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].y, pVertices[vertIndex + 1].z - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].z));
			}
		}
		if (minAxisDist < spotRange) // Subtract axis dist by spot range then ABS to get desired outcome
		{
			pVertices[vertIndex + 1].r = 1.0f; //- abs((minAxisDist - spotRange) * (1 / spotRange));
			pVertices[vertIndex + 1].g = 1.0f - abs((minAxisDist - spotRange) * (1 / spotRange));
			pVertices[vertIndex + 1].b = 1.0f;
		}
		else
		{
			pVertices[vertIndex + 1].r = 1.0f;
			pVertices[vertIndex + 1].g = 1.0f;
			pVertices[vertIndex + 1].b = 1.0f;
		}

		// 		if (abs(((int)round(pVertices[vertIndex].y * 100)) % 2) == 1) {
		// 			pVertices[vertIndex + 1].r = 0.0f;
		// 			pVertices[vertIndex + 1].g = 0.5f;
		// 			pVertices[vertIndex + 1].b = 0.5f;
		// 		}
		// 		else {
		// 			pVertices[vertIndex + 1].r = 0.5f;
		// 			pVertices[vertIndex + 1].g = 0.0f;
		// 			pVertices[vertIndex + 1].b = 0.5f;
		// 		}
		// 		if (abs(((int)round(pVertices[vertIndex].z * 100 + pVertices[vertIndex].x * 100)) % 2) == 1) {
		// 			pVertices[vertIndex + 1].r += 0.5f;
		// 			pVertices[vertIndex + 1].g += 0.5f;
		// 			pVertices[vertIndex + 1].b += 0.0f;
		// 		}
		// 		else {
		// 			pVertices[vertIndex + 1].r += 0.0f;
		// 			pVertices[vertIndex + 1].g += 0.2f;
		// 			pVertices[vertIndex + 1].b += 0.5f;
		// 		}
		//         pVertices[vertIndex + 1].r = 1.0f;
		//         pVertices[vertIndex + 1].g = 1.0f;
		//         pVertices[vertIndex + 1].b = 1.0f;

		pVertices[vertIndex + 2].x = pTheVerticesFile[pTheTriangles[triIndex].v2].x;
		pVertices[vertIndex + 2].y = pTheVerticesFile[pTheTriangles[triIndex].v2].y;
		pVertices[vertIndex + 2].z = pTheVerticesFile[pTheTriangles[triIndex].v2].z;

		minAxisDist = 10.0f;
		for (unsigned int spotIndex = 0; spotIndex < spotCount; spotIndex++) { // Scan through marked spot vertices to get distance from each spot, keeping lowest distance and setting color from that
			if (glm::length(glm::vec3(pVertices[vertIndex + 2].x - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].x, pVertices[vertIndex + 2].y - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].y, pVertices[vertIndex + 2].z - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].z)) < minAxisDist) {
				minAxisDist = glm::length(glm::vec3(pVertices[vertIndex + 2].x - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].x, pVertices[vertIndex + 2].y - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].y, pVertices[vertIndex + 2].z - pTheVerticesFile[pTheTriangles[spots[spotIndex]].v0].z));
			}
		}
		if (minAxisDist < spotRange) // Subtract axis dist by spot range then ABS to get desired outcome
		{
			pVertices[vertIndex + 2].r = 1.0f; //- abs((minAxisDist - spotRange) * (1 / spotRange));
			pVertices[vertIndex + 2].g = 1.0f - abs((minAxisDist - spotRange) * (1 / spotRange));
			pVertices[vertIndex + 2].b = 1.0f;
		}
		else
		{
			pVertices[vertIndex + 2].r = 1.0f;
			pVertices[vertIndex + 2].g = 1.0f;
			pVertices[vertIndex + 2].b = 1.0f;
		}

		// 		if ((abs((int)round(pVertices[vertIndex].y * 100)) % 2) == 1) {
		// 			pVertices[vertIndex + 2].r = 0.0f;
		// 			pVertices[vertIndex + 2].g = 0.5f;
		// 			pVertices[vertIndex + 2].b = 0.5f;
		// 		}
		// 		else {
		// 			pVertices[vertIndex + 2].r = 0.5f;
		// 			pVertices[vertIndex + 2].g = 0.0f;
		// 			pVertices[vertIndex + 2].b = 0.5f;
		// 		}
		// 		if (abs(((int)round(pVertices[vertIndex].z * 100 + pVertices[vertIndex].x * 100)) % 2) == 1) {
		// 			pVertices[vertIndex + 2].r += 0.5f;
		// 			pVertices[vertIndex + 2].g += 0.5f;
		// 			pVertices[vertIndex + 2].b += 0.0f;
		// 		}
		// 		else {
		// 			pVertices[vertIndex + 2].r += 0.0f;
		// 			pVertices[vertIndex + 2].g += 0.2f;
		// 			pVertices[vertIndex + 2].b += 0.5f;
		// 		}
		//         pVertices[vertIndex + 2].r = 1.0f;
		//         pVertices[vertIndex + 2].g = 1.0f;
		//         pVertices[vertIndex + 2].b = 1.0f;

		vertIndex += 3;
	}

	return true;
}

// Load bathtub
bool LoadTheFile_Ply_XYZ_N_RGBA(std::string theFileName)
{
	//    property float x
	//    property float y
	//    property float z

		//sVertexPlyFile p;       p.x = 0.0f;     p.y = 1.0f; p.z = 2.0f;
		//std::cout << p.x;

		//sVertexPlyFile q;
		//std::cout << "Type in the x: ";
		//std::cin >> q.x;

		// Input Filestream 
	std::ifstream theBunnyFile("bathtub.ply");
	if (!theBunnyFile.is_open())
	{
		// didn't open the file.
		return false;
	}

	std::string temp;
	while (theBunnyFile >> temp)
	{
		if (temp == "vertex")
		{
			break;
		}
	};
	//element vertex 8171
	theBunnyFile >> g_numberOfVertices;


	while (theBunnyFile >> temp)
	{
		if (temp == "face")
		{
			break;
		}
	};
	//element vertex 8171
	theBunnyFile >> g_numberOfTriangles;

	while (theBunnyFile >> temp)
	{
		if (temp == "end_header")
		{
			break;
		}
	};


	// Allocate enough space to hold the vertices
//    sVertex vertices[8171];                 // Stack

	sVertex x;      // STACK based variable (on the stack)
	sVertex* px;    // Pointer variable.

	//    int y = 5;
	//
	//    int* py = new int();
	//    *py = 5;


		// Dynamically allocate memory on the heap;
	sVertexPlyFile* pTheVerticesFile = new sVertexPlyFile[g_numberOfVertices];

	// -0.036872 0.127727 0.00440925 
	for (unsigned int index = 0; index != g_numberOfVertices; index++)
	{
		sVertexPlyFile tempVertex;
		theBunnyFile >> tempVertex.x;                //std::cin >> a.x;
		theBunnyFile >> tempVertex.y;                //std::cin >> a.y;
		theBunnyFile >> tempVertex.z;                //std::cin >> a.z;

		theBunnyFile >> tempVertex.nx;
		theBunnyFile >> tempVertex.ny;
		theBunnyFile >> tempVertex.nz;

		theBunnyFile >> tempVertex.r;       tempVertex.r /= 255.0f;
		theBunnyFile >> tempVertex.g;       tempVertex.g /= 255.0f;
		theBunnyFile >> tempVertex.b;       tempVertex.b /= 255.0f;
		theBunnyFile >> tempVertex.a;       tempVertex.a /= 255.0f;


		pTheVerticesFile[index] = tempVertex;
	}


	sTrianglePlyFile* pTheTriangles = new sTrianglePlyFile[g_numberOfTriangles];

	// 3 3495 3549 3548 
	for (unsigned int index = 0; index != g_numberOfTriangles; index++)
	{
		sTrianglePlyFile tempTriangle;

		unsigned int discard;
		theBunnyFile >> discard;            // 3
		theBunnyFile >> tempTriangle.v0;                //std::cin >> a.x;
		theBunnyFile >> tempTriangle.v1;                //std::cin >> a.y;
		theBunnyFile >> tempTriangle.v2;                //std::cin >> a.z;

		pTheTriangles[index] = tempTriangle;
	}


	// Now allocate the array that the shader is going to use
//    g_NumberOfVerticesToDraw = 6;
//    pVertices = new sVertex[g_NumberOfVerticesToDraw];
//
//    pVertices[0] = { -0.6f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f };
//    pVertices[1] = { 0.6f, -0.4f, 0.0f,   0.0f, 1.0f, 0.0f };
//    pVertices[2] = { 0.f,  0.6f, 0.0f,   0.0f, 0.0f, 1.0f };
//    pVertices[3] = { 0.4f, -0.4f, 0.0f,   0.0f, 1.0f, 0.0f };
//    pVertices[4] = { 1.6f, -0.4f, 0.0f,   0.0f, 0.0f, 1.0f };
//    pVertices[5] = { 1.0f,  0.6f, 0.0f,   1.0f, 0.0f, 0.0f };

//    g_NumberOfVerticesToDraw = g_numberOfVertices;
//    pVertices = new sVertex[g_NumberOfVerticesToDraw];
//
//    // Copy from the array we filled from the file to the array that that 
//    //  shader (video card) will use...
//
//    for (unsigned int index = 0; index != g_numberOfVertices; index++)
//    {
//        // Left is the array going to the video
//        // Right is the array from the file
//        pVertices[index].x = pTheVerticesFile[index].x;
//        pVertices[index].y = pTheVerticesFile[index].y;
//        pVertices[index].z = pTheVerticesFile[index].z;
//
//        pVertices[index].r = 1.0f;
//        pVertices[index].g = 1.0f;
//        pVertices[index].b = 1.0f;
//    }
// 
	g_NumberOfVerticesToDraw = g_numberOfTriangles * 3;
	pVertices = new sVertex[g_NumberOfVerticesToDraw];

	unsigned int vertIndex = 0;
	for (unsigned int triIndex = 0; triIndex != g_numberOfTriangles; triIndex++)
	{
		// 3 1582 1581 2063 
		pVertices[vertIndex + 0].x = pTheVerticesFile[pTheTriangles[triIndex].v0].x;
		pVertices[vertIndex + 0].y = pTheVerticesFile[pTheTriangles[triIndex].v0].y;
		pVertices[vertIndex + 0].z = pTheVerticesFile[pTheTriangles[triIndex].v0].z;

		pVertices[vertIndex + 0].r = pTheVerticesFile[pTheTriangles[triIndex].v0].r;
		pVertices[vertIndex + 0].g = pTheVerticesFile[pTheTriangles[triIndex].v0].g;
		pVertices[vertIndex + 0].b = pTheVerticesFile[pTheTriangles[triIndex].v0].b;

		pVertices[vertIndex + 1].x = pTheVerticesFile[pTheTriangles[triIndex].v1].x;
		pVertices[vertIndex + 1].y = pTheVerticesFile[pTheTriangles[triIndex].v1].y;
		pVertices[vertIndex + 1].z = pTheVerticesFile[pTheTriangles[triIndex].v1].z;

		pVertices[vertIndex + 1].r = pTheVerticesFile[pTheTriangles[triIndex].v1].r;
		pVertices[vertIndex + 1].g = pTheVerticesFile[pTheTriangles[triIndex].v1].g;
		pVertices[vertIndex + 1].b = pTheVerticesFile[pTheTriangles[triIndex].v1].b;

		pVertices[vertIndex + 2].x = pTheVerticesFile[pTheTriangles[triIndex].v2].x;
		pVertices[vertIndex + 2].y = pTheVerticesFile[pTheTriangles[triIndex].v2].y;
		pVertices[vertIndex + 2].z = pTheVerticesFile[pTheTriangles[triIndex].v2].z;

		pVertices[vertIndex + 2].r = pTheVerticesFile[pTheTriangles[triIndex].v2].r;
		pVertices[vertIndex + 2].g = pTheVerticesFile[pTheTriangles[triIndex].v2].g;
		pVertices[vertIndex + 2].b = pTheVerticesFile[pTheTriangles[triIndex].v2].b;

		vertIndex += 3;
	}

	return true;
}


int main(void)
{
	srand(time(NULL));
	std::cout << "About to blow you mind with OpenGL!" << std::endl;

	GLFWwindow* window;
	GLuint vertex_buffer, vertex_shader, fragment_shader;//v , program;
	GLint mvp_location;// , vpos_location, vcol_location;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);


	if (!LoadTheFile_PlyXYZ("Hey"))
		//if (!LoadTheFile_Ply_XYZ_N_RGBA("Hey"))
	{

		std::cout << "Error: didn't load the file." << std::endl;
		std::cout << "How sad." << std::endl;
		return -1;
	}


	// NOTE: OpenGL error checks have been omitted for brevity

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int size_of_sVertex = sizeof(sVertex);
	//    unsigned int number_of_bytes_in_array = size_of_sVertex * NUM_OF_VERTICES;
	unsigned int number_of_bytes_in_array = size_of_sVertex * g_NumberOfVerticesToDraw;

	//    glBufferData(GL_ARRAY_BUFFER, number_of_bytes_in_array, vertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, number_of_bytes_in_array, pVertices, GL_STATIC_DRAW);


	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	mvp_location = glGetUniformLocation(program, "MVP");

	GLint vpos_location = glGetAttribLocation(program, "vPos");
	GLint vcol_location = glGetAttribLocation(program, "vCol");

	//    int sizeofsVertex = sizeof(sVertex);
	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location,
		3,                        // 2 floats 
		GL_FLOAT,
		GL_FALSE,
		sizeof(sVertex),                   //  sizeof(vertices[0]),
		(void*)offsetof(sVertex, x));    //  (void*)0);

	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(sVertex),                  //  sizeof(vertices[0]),
		(void*)offsetof(sVertex, r));    //  (void*)(sizeof(float) * 2));







	//    glm::vec3 cameraEye = glm::vec3(10.0, 5.0, -15.0f);
	float yaxisRotation = 0.0f;

	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		//        mat4x4 m, p, mvp;
		glm::mat4 m, p, v, mvp;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// While drawing a pixel, see if the pixel that's already there is closer or not?
		glEnable(GL_DEPTH_TEST);

		//         mat4x4_identity(m);
		m = glm::mat4(1.0f);

		//mat4x4_rotate_Z(m, m, (float) glfwGetTime());
//        glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
//                                        -0.5f, // (float)glfwGetTime(),
//                                        glm::vec3(0.0f, 0.0, 1.0f));

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		//std::cout << deltaTime << std::endl;
		lastTime = currentTime;

		//        yaxisRotation += 0.01f;
		//        yaxisRotation += ( (2.0 * 3.1415) * deltaTime );

		glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
			yaxisRotation, // (float)glfwGetTime(),
			glm::vec3(0.0f, 1.0, 0.0f));


		//        m = m * rotateZ;
		m = m * rotateY;
		//        m = m * rotateZ;

				//mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		p = glm::perspective(0.6f,
			ratio,
			0.1f,
			1000.0f);

		v = glm::mat4(1.0f);

		// Spin Camera Around Target
		//cameraEye.x = (sin(deltaTime));
		//cameraEye.z = (cos(deltaTime));


		//glm::vec3 cameraEye = glm::vec3(0.0, 0.1, -0.25f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.1f, 0.0f);
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

		//        cameraEye.z += 0.001f;

		v = glm::lookAt(cameraEye,
			cameraTarget,
			upVector);

		//mat4x4_mul(mvp, p, m);
		mvp = p * v * m;

		glUseProgram(program);
		//glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

		//        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT /*GL_LINE*/ /*GL_FILL*/);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE /*GL_LINE*/ /*GL_FILL*/);
		//        glPointSize(10.0f);

		glDrawArrays(GL_TRIANGLES, 0, g_NumberOfVerticesToDraw);

		glfwSwapBuffers(window);
		glfwPollEvents();

		// Update the title screen
		std::stringstream ssTitle;
		ssTitle << "Camera (x,y,z): "
			<< cameraEye.x << ", "
			<< cameraEye.y << ", "
			<< cameraEye.z << ")";
		//        glfwSetWindowTitle(window, "HEY!");

		std::string theTitle = ssTitle.str();

		glfwSetWindowTitle(window, theTitle.c_str());


	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}