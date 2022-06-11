#ifndef __PLYREADER_H__
#define __PLYREADER_H__

#include <string>
using namespace std;

class PLYLoader 
{
public:
	PLYLoader();
	bool loadModel(string filepath);
	float* getVertexXYZ();
	int getTotalVertices();
	int getTotalFaces();
	int* getVertexIndex();

private:
	float* vertexXYZ;
	int* vertexIndex;
	int totalVertices;
	int totalFaces;
};

#endif