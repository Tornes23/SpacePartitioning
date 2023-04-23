/*!**************************************************************************
\file    mesh_data.cpp

\author  Nestor Uriarte

\par     DP email:  nestor.uriarte@digipen.edu

\par     Course:    CS350

\par     assignemnt 0

\date    Mon Sep 21 12:44:05 2020

\brief	 This file contains the implementation of the
parser functions

The functions included are:
- mesh_data load_obj(const char* filename);
- std::vector<mesh_data> load_objs(const char* filename);

***************************************************************************/


#include "pch.hpp"
#include "mesh_data.hpp"

namespace cs350 {

	/**************************************************************************
	*!
	\fn     load_obj

	\brief 
	Parses an .obj file to a mesh data structure

	\param  const char* filename
	the .obj filename

	\return mesh_data
	The mesh data structure with the parsed data

	*
	**************************************************************************/
	mesh_data load_obj(const char* filename)
	{
		//getting the file and opening it
		std::ifstream infile(filename);

		//if it was properly opened
		assert(infile.is_open() != false);

		//struct that will contain the  parsed data
		mesh_data parsed;

		//while not at the end of the file
		while (!infile.eof())
		{
			//string to store the line start
			std::string lineStart;

			//getting the first word
			infile >> lineStart;

			//if is a v means the following data is a position
			if (lineStart == "v")
			{
				//string to contain the data
				std::string values;

				//struct to store the values
				glm::vec3 vertex;

				//getting the values, converting them to decimal and storing them
				infile >> values;
				vertex.x = static_cast<float>(atof(values.data()));

				infile >> values;
				vertex.y = static_cast<float>(atof(values.data()));

				infile >> values;
				vertex.z = static_cast<float>(atof(values.data()));

				//adding it to the positions
				parsed.positions.push_back(vertex);

			}
			//if is a v means the following data is a texture coordinate
			else if (lineStart == "vt")
			{
				//string to contain the data
				std::string values;

				//struct to store the values
				glm::vec2 uv;

				//getting the values, converting them to decimal and storing them
				infile >> values;
				uv.x = static_cast<float>(atof(values.data()));

				infile >> values;
				uv.y = static_cast<float>(atof(values.data()));

				//adding it to the uvs
				parsed.uvs.push_back(uv);
			}
			//if is a v means the following data is a normal vector
			else if (lineStart == "vn")
			{
				//string to contain the data
				std::string values;

				//struct to store the values
				glm::vec3 normal;

				//getting the values, converting them to decimal and storing them
				infile >> values;
				normal.x = static_cast<float>(atof(values.data()));

				infile >> values;
				normal.y = static_cast<float>(atof(values.data()));

				infile >> values;
				normal.z = static_cast<float>(atof(values.data()));

				//adding it to the normals
				parsed.normals.push_back(normal);
			}
			else if (lineStart == "f")
			{
				//string to contain the data
				std::string values;

				//reading the whole line
				std::getline(infile, values);

				//static arrays to store the data
				int vertexIndex[] = { 0,0,0 };
				int uvIndex[]     = { 0,0,0 };
				int normalIndex[] = { 0,0,0 };

				//structure to store all the data of a face
				glm::vec<3, glm::ivec3> faceData;
				
				//if both uvs and normals are not given the format of the face data is: num  num  num
				if(parsed.uvs.empty() && parsed.normals.empty())
				{
					//parsing the line and getting the data amount
					sscanf_s(values.c_str(), "%d  %d  %d",
									&vertexIndex[0],  
									&vertexIndex[1],  
									&vertexIndex[2]);

					//setting it to -1 to avoid acces violation
					faceData[0].y = -1;
					faceData[1].y = -1;
					faceData[2].y = -1;

					faceData[0].z = -1;
					faceData[1].z = -1;
					faceData[2].z = -1;

				}
				else if (parsed.uvs.empty())//if only the uvs were not given the format is: num//num  num//num  num//num
				{
					//parsing the line and getting the data amount
					sscanf_s(values.c_str(), "%d//%d  %d//%d  %d//%d",
									&vertexIndex[0], &normalIndex[0], 
									&vertexIndex[1], &normalIndex[1], 
									&vertexIndex[2], &normalIndex[2]);

					//setting it to -1 to avoid acces violation
					faceData[0].y = -1;
					faceData[1].y = -1;
					faceData[2].y = -1;
				}
				else//if all the data was given the format is: num/num/num  num/num/num  num/num/num
				{
					//parsing the line and getting the data amount
					sscanf_s(values.c_str(), "%d/%d/%d  %d/%d/%d  %d/%d/%d",
							&vertexIndex[0], &uvIndex[0],&normalIndex[0], 
							&vertexIndex[1], &uvIndex[1],&normalIndex[1], 
							&vertexIndex[2], &uvIndex[2],&normalIndex[2]);
				}

				//changing the index values to be from 1 -> n to 0 -> n-1

				//in this struct of 3 vectors of ints each vector represents a vertex 
				//and the x coordinate is for position, the y for the uvs and the z for the normals
				faceData[0].x = vertexIndex[0] - 1;
				faceData[1].x = vertexIndex[1] - 1;
				faceData[2].x = vertexIndex[2] - 1;

				faceData[0].y = uvIndex[0] - 1;
				faceData[1].y = uvIndex[1] - 1;
				faceData[2].y = uvIndex[2] - 1;

				faceData[0].z = normalIndex[0] - 1;
				faceData[1].z = normalIndex[1] - 1;
				faceData[2].z = normalIndex[2] - 1;

				//storing the data
				parsed.faces.push_back(faceData);

			}
			else//in case is a useless data
			{
				//string to store it
				std::string dummyString;

				//reading the line
				std::getline(infile, dummyString);
			}

		}

		//returning the mesh data with all the information
		return parsed;
	}

	/**************************************************************************
	*!
	\fn     load_objs

	\brief 
	

	\param  const char* filename
	

	\return std::vector<mesh_data>
	 vector with the meshes that were parsed from the obj

	*
	**************************************************************************/
	std::vector<mesh_data> load_objs(const char* filename)
	{

		std::ifstream infile(filename);

		assert(infile.is_open() != false);

		mesh_data tempMesh;
		std::vector<mesh_data> meshes;

		while (!infile.eof())
		{
			std::string lineStart;

			infile >> lineStart;

			if (lineStart == "v")
			{
				std::string values;
				glm::vec3 vertex;

				infile >> values;
				vertex.x = static_cast<float>(atof(values.data()));

				infile >> values;
				vertex.y = static_cast<float>(atof(values.data()));

				infile >> values;
				vertex.z = static_cast<float>(atof(values.data()));

				tempMesh.positions.push_back(vertex);
			}
			else if (lineStart == "vt")
			{
				std::string values;
				glm::vec2 uv;

				infile >> values;
				uv.x = static_cast<float>(atof(values.data()));

				infile >> values;
				uv.y = static_cast<float>(atof(values.data()));

				tempMesh.uvs.push_back(uv);
			}
			else if (lineStart == "vn")
			{
				std::string values;
				glm::vec3 normal;

				infile >> values;
				normal.x = static_cast<float>(atof(values.data()));

				infile >> values;
				normal.y = static_cast<float>(atof(values.data()));

				infile >> values;
				normal.z = static_cast<float>(atof(values.data()));

				tempMesh.normals.push_back(normal);
			}
			else if (lineStart == "f")
			{
				//parse face still dunno if necessary
			}
			else if (lineStart == "g")
			{
				meshes.push_back(tempMesh);
				tempMesh.normals.clear();
				tempMesh.positions.clear();
				tempMesh.uvs.clear();
			}
			else
			{
				std::string dummyString;

				std::getline(infile, dummyString);
			}

		}

		return meshes;
	}
}
