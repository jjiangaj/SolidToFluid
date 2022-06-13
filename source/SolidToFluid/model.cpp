#include "model.h"
#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

Model::Model()
{
	this->totalVertices = 0;
	this->totalFaces = 0;	
	this->vertexXYZ = NULL;
	this->vertexIndex = NULL;
	this->velocities = NULL;
	this->forces = NULL;
}

Model::Model(string filepath)
{
	this->totalVertices = 0;
	this->totalFaces = 0;
	this->vertexXYZ = NULL;
	this->vertexIndex = NULL;
	bool ret = this->loadModel(filepath);
	if (ret) {
		this->calcNorm();
		this->velocities = (float*)calloc(3 * (this->totalVertices), sizeof(float));
	}
}

Model::~Model()
{
	free(this->vertexXYZ);
	free(this->vertexIndex);
	free(this->velocities);
	free(this->forces);
}

bool Model::loadModel(string filepath) {
	std::cout << "Loading " << filepath << " ..." << std::endl;
	fstream fs;
	if (!filepath.empty()) {
		fs.open(filepath, ios::in);
	}
	else {
		std::cout << "File path is null" << std::endl;
		return false;
	}
	if (!fs.is_open()) {
		cout << "Loading " << filepath << " failed" << endl;
		return false;
	}
	string filein;
	while (!fs.eof()) {
		fs >> filein;
		if (filein == "ply" || filein == "PLY") {}
		else if (filein == "element") { //reads # of vertices & faces
			fs >> filein;
			if (filein == "vertex") {
				fs >> this->totalVertices;
			}
			else if (filein == "face") {
				fs >> this->totalFaces;
			}
			getline(fs, filein, '\n');
		}
		else if (filein == "end_header") { //reads data
			getline(fs, filein, '\n');
			if (this->totalVertices == 0) {
				cout << "Vertices number not read" << endl;
				return false;
			}
			this->vertexXYZ = (float*)malloc(6 * (this->totalVertices) * sizeof(float));
			for (int i = 0; i < this->totalVertices; i++) {
				fs >> (this->vertexXYZ)[i * 6 + 0];
				fs >> (this->vertexXYZ)[i * 6 + 1];
				fs >> (this->vertexXYZ)[i * 6 + 2];
				(this->vertexXYZ)[i * 6 + 0] *= 5; //enlarge the original model
				(this->vertexXYZ)[i * 6 + 1] *= 5;
				(this->vertexXYZ)[i * 6 + 2] *= 5;

				(this->vertexXYZ)[i * 6 + 3] = 0; //to store vertex normal
				(this->vertexXYZ)[i * 6 + 4] = 0;
				(this->vertexXYZ)[i * 6 + 5] = 0;

				//go to the next line
				getline(fs, filein, '\n');
			}

			if (this->totalFaces == 0) {
				cout << "Faces number not read" << endl;
				return false;
			}
			int nums; //triangle or quad
			this->vertexIndex = (int*)malloc(3 * (this->totalFaces) * sizeof(int));
			for (int j = 0; j < this->totalFaces; j++) {
				fs >> nums;
				if (nums == 3) {
					fs >> (this->vertexIndex)[j * 3];
					fs >> (this->vertexIndex)[j * 3 + 1];
					fs >> (this->vertexIndex)[j * 3 + 2];
				}
			}
		}
		else
			getline(fs, filein, '\n');
	}
	return true;
}

void Model::calcNorm(){
	for (int i = 0; i < this->totalFaces; i++) {
		int i1 = this->vertexIndex[3 * i + 0] * 6;
		int i2 = this->vertexIndex[3 * i + 1] * 6;
		int i3 = this->vertexIndex[3 * i + 2] * 6;

		glm::vec3 v1 = glm::vec3(this->vertexXYZ[i1] - this->vertexXYZ[i2], this->vertexXYZ[i1 + 1] - this->vertexXYZ[i2 + 1], this->vertexXYZ[i1 + 2] - this->vertexXYZ[i2 + 2]);
		glm::vec3 v2 = glm::vec3(this->vertexXYZ[i1] - this->vertexXYZ[i3], this->vertexXYZ[i1 + 1] - this->vertexXYZ[i3 + 1], this->vertexXYZ[i1 + 2] - this->vertexXYZ[i3 + 2]);
		glm::vec3 norm = glm::normalize(glm::cross(v1, v2));

		this->vertexXYZ[i1 + 3] += norm.x;
		this->vertexXYZ[i1 + 4] += norm.y;
		this->vertexXYZ[i1 + 5] += norm.z;
		this->vertexXYZ[i2 + 3] += norm.x;
		this->vertexXYZ[i2 + 4] += norm.y;
		this->vertexXYZ[i2 + 5] += norm.z;
		this->vertexXYZ[i3 + 3] += norm.x;
		this->vertexXYZ[i3 + 4] += norm.y;
		this->vertexXYZ[i3 + 5] += norm.z;
	}
}

float* Model::getVertexXYZ() {
	return this->vertexXYZ;
}

int Model::getTotalVertices() {
	return this->totalVertices;
}

int Model::getTotalFaces() {
	return this->totalFaces;
}

int* Model::getVertexIndex() {
	return this->vertexIndex;
}

float* Model::getVelocity() {
	return this->velocities;
}

float* Model::getForce() {
	return this->forces;
}