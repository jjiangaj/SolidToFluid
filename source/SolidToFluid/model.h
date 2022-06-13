#ifndef __MODEL_H__
#define __MODEL_H__

#include <string>

using namespace std;

class Model
{
public:
	Model();
	Model(string filepath);
	~Model();

	bool loadModel(string filepath);
	void calcNorm();

	float* getVertexXYZ();
	int getTotalVertices();
	int getTotalFaces();
	int* getVertexIndex();
	float* getVelocity();
	float* getForce();

private:
	float* vertexXYZ;
	int* vertexIndex;
	int totalVertices;
	int totalFaces;
	float* velocities;
	float* forces;
};

#endif