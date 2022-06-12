#include "plyReader.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

PLYLoader::PLYLoader()
{
	this->totalVertices = 0;
	this->totalFaces = 0;	
	this->vertexXYZ = NULL;
	this->vertexIndex = NULL;
}

bool PLYLoader::loadModel(string filepath) {
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

float* PLYLoader::getVertexXYZ() {
	return this->vertexXYZ;
}

int PLYLoader::getTotalVertices() {
	return this->totalVertices;
}

int PLYLoader::getTotalFaces() {
	return this->totalFaces;
}

int* PLYLoader::getVertexIndex() {
	return this->vertexIndex;
}