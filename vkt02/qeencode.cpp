#include "qeheader.h"

QeHuffmanTree::~QeHuffmanTree() {
	if (tree != nullptr) {
		delete tree;
		tree = nullptr;
	}
}

QeHuffmanTree2::~QeHuffmanTree2() {
	if (codes != nullptr) {
		delete codes;
		codes = nullptr;
	}
	if (codeBits != nullptr) {
		delete codeBits;
		codeBits = nullptr;
	}
	if (values != nullptr) {
		delete values;
		values = nullptr;
	}
}

int QeEncode::readBits(const unsigned char* stream, size_t *bitPointer, size_t readCount, bool bLeft, bool bNegative) {

	size_t bytes;
	unsigned char bits;
	size_t move;
	int ret = 0;
	size_t i;
	for ( i = 0; i < readCount; ++i) {
		bytes = (*bitPointer) >> 3;
		bits = (*bitPointer) % 8;
		move = i;
		if (bLeft) {
			bits = 7 - bits;
			move = readCount - move - 1;
		}
		ret += ((*(stream + bytes) >> bits & 1) << move);
		(*bitPointer)++;
	}

	if (bNegative && readCount>0) {
		if (ret == 0) {
			move = 1;
			move <<= readCount;
			ret = int(-1*(move - 1));
		}
		else {
			i = 0;
			bytes = readBits((unsigned char*)&ret, &i, readCount - 1);
			bits = readBits((unsigned char*)&ret, &i, 1);

			if (bits == 0) {
				move = 1;
				move <<= readCount ;
				ret = int(-1*(move  - bytes - 1));
			}
		}
	}
	return ret;
}

std::string QeEncode::trim(std::string s) {

	if (s.empty())	return s;
	s.erase(0, s.find_first_not_of(" "));
	s.erase(0, s.find_first_not_of("\n"));
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	s.erase(s.find_last_not_of("\n") + 1);
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

QeAssetJSON* QeEncode::decodeJSON(const char* buffer, int &index) {
	/* key
	0: {
	1: }
	2: "
	3: :
	4: [
	5: ]
	6: ,
	*/
	const char keys[] = "{}\":[],";
	QeAssetJSON* node = new QeAssetJSON();
	int lastIndex = index, currentIndex = index, lastKey = 0, currentKey;
	char* newChar = nullptr;
	std::vector<std::string> *vsBuffer = nullptr;
	bool bValue = false;
	int count = 0;
	std::string key;
	while (1) {
		currentIndex = int(strcspn(buffer + lastIndex, keys) + lastIndex);
		currentKey = int(strchr(keys, buffer[currentIndex]) - keys);

		if (currentKey == 0 && lastKey != 3 && lastKey != 4)	count++;

		if (lastKey == currentKey && currentKey == 2) {
			std::string s(buffer + lastIndex, currentIndex - lastIndex);
			s = trim(s);
			if (bValue) {
				bValue = false;

				node->eKeysforValues.push_back(key);
				node->eValues.push_back(s);
			}
			else	key = s;
		}
		else if (lastKey == 3) {
			if (currentKey == 2) bValue = true;

			else if (currentKey == 0) {
				node->eKeysforNodes.push_back(key);
				node->eNodes.push_back(decodeJSON(buffer, currentIndex));
			}
			else if (currentKey != 4) {
				std::string s(buffer + lastIndex, currentIndex - lastIndex);
				s = trim(s);
				node->eKeysforValues.push_back(key);
				node->eValues.push_back(s);
			}
		}
		else if (lastKey == 4) {
			if (currentKey == 0) {
				std::vector<QeAssetJSON*> vjson;
				while (1) {
					vjson.push_back(decodeJSON(buffer, currentIndex));
					lastIndex = currentIndex + 1;
					currentIndex = int(strcspn(buffer + lastIndex, keys) + lastIndex);
					currentKey = int(strchr(keys, buffer[currentIndex]) - keys);

					if (currentKey != 6) break;
				}
				node->eKeysforArrayNodes.push_back(key);
				node->eArrayNodes.push_back(vjson);
			}
			else {
				currentIndex = int(strchr(buffer + lastIndex, ']') - buffer);
				currentKey = 5;
				std::vector<std::string> vs;

				std::string s(buffer + lastIndex, currentIndex - lastIndex);
				char s2[512];
				strncpy_s(s2, s.c_str(), 512);
				char *context = NULL;
				const char* key1 = ",\"\r\n";
				char* pch = strtok_s(s2, key1, &context);

				while (pch != NULL) {
					std::string s(pch);
					s = trim(s);
					if (s.length() > 0) vs.push_back(s);
					pch = strtok_s(NULL, key1, &context);
				}
				node->eKeysforArrayValues.push_back(key);
				node->eArrayValues.push_back(vs);
			}
		}

		lastKey = currentKey;
		lastIndex = currentIndex + 1;

		if (currentKey == 1) {
			count--;
			if (count == 0) break;
		}
	}

	index = currentIndex;
	return node;
}

QeAssetXML* QeEncode::decodeXML(const char* buffer, int &index) {
	/* key
	0: <
	1: >
	2: /
	3: =
	4: "
	5: ?
	6: !
	7: -
	*/
	const char keys[] = "<>/=\"?";

	QeAssetXML* node = new QeAssetXML();
	int lastIndex = index, currentIndex = index, lastKey = 0, currentKey = 0;
	char* newChar = nullptr;
	std::vector<std::string> *vsBuffer = nullptr;
	bool bRoot = true;

	while (1) {
		currentIndex = int(strcspn(buffer + lastIndex, keys) + lastIndex);
		currentKey = int(strchr(keys, buffer[currentIndex]) - keys);

		if (currentKey == 5) {
			lastIndex = currentIndex + 1;
			currentIndex = int(strchr(buffer + lastIndex, '?') - buffer);
			lastIndex = currentIndex + 1;
			bRoot = true;
		}
		else if (currentKey == 0) {

			if (bRoot) bRoot = false;

			else if (buffer[currentIndex + 1] != '/')	node->nexts.push_back(decodeXML(buffer, currentIndex));

			else if (lastKey == 1) {
				std::string s(buffer + lastIndex, currentIndex - lastIndex);
				s = trim(s);
				node->value = s;
			}
		}
		else if (lastKey == 0) {

			if (currentKey == 1) {
				std::string s(buffer + lastIndex, currentIndex - lastIndex);
				s = trim(s);
				node->key = s;
			}
			else if (currentKey == 3) {
				int index = int(strchr(buffer + lastIndex, ' ') - buffer);
				std::string s(buffer + lastIndex, index - lastIndex);
				s = trim(s);
				node->key = s;
				std::string s1(buffer + index, currentIndex - index);
				s1 = trim(s1);
				node->eKeys.push_back(s1);
			}
		}
		else if (lastKey == 2 && currentKey == 1)	break;

		else if (lastKey == 4) {

			if (currentKey == 3) {
				std::string s(buffer + lastIndex, currentIndex - lastIndex);
				s = trim(s);
				node->eKeys.push_back(s);
			}
			else if (currentKey == lastKey) {
				std::string s(buffer + lastIndex, currentIndex - lastIndex);
				s = trim(s);
				node->eVaules.push_back(s);
			}
		}
		lastKey = currentKey;
		lastIndex = currentIndex + 1;
	}
	index = currentIndex;
	return node;
}

/*QeAssetModel* QeEncode::decodeOBJ(char* buffer) {

	std::vector<QeVector3f> normalV;
	std::vector<QeVector2f> texCoordV;

	QeVector3f tempV3;
	QeVector2f tempV2;
	QeVector3i tempV3p, tempV3t, tempV3n;
	char mtlPath[256];
	QeAssetModel* model = new QeAssetModel();
	char *context;
	char* line = strtok_s(buffer, "\n", &context);

	while (line != nullptr) {

		if (strncmp(line, "mtllib ", 7) == 0)	sscanf_s(line, "mtllib %s", mtlPath, (unsigned int)sizeof(mtlPath));

		else if (strncmp(line, "v ", 2) == 0) {
			sscanf_s(line, "v %f %f %f", &(tempV3.x), &(tempV3.y), &(tempV3.z));
			QeVertex vet;
			vet.pos = tempV3;
			vet.color = { 1.0f, 1.0f, 1.0f };
			model->vertices.push_back(vet);
		}
		else if (strncmp(line, "vt ", 3) == 0) {
			sscanf_s(line, "vt %f %f", &(tempV2.x), &(tempV2.y));
			texCoordV.push_back(tempV2);
		}
		else if (strncmp(line, "vn ", 3) == 0) {
			sscanf_s(line, "vn %f %f %f", &(tempV3.x), &(tempV3.y), &(tempV3.z));
			normalV.push_back(tempV3);
		}
		else if (strncmp(line, "f ", 2) == 0) {
			if (strstr(line, "//"))		sscanf_s(line, "f %d//%d %d//%d %d//%d", &(tempV3p.x), &(tempV3n.x), &(tempV3p.y), &(tempV3n.y), &(tempV3p.z), &(tempV3n.z));
			else if (texCoordV.empty())	sscanf_s(line, "f %d %d %d", &(tempV3p.x), &(tempV3p.y), &(tempV3p.z));
			else if (normalV.empty())	sscanf_s(line, "f %d/%d %d/%d %d/%d", &(tempV3p.x), &(tempV3t.x), &(tempV3p.y), &(tempV3t.y), &(tempV3p.z), &(tempV3t.z));
			else						sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &(tempV3p.x), &(tempV3t.x), &(tempV3n.x), &(tempV3p.y), &(tempV3t.y), &(tempV3n.y), &(tempV3p.z), &(tempV3t.z), &(tempV3n.z));

			tempV3p -= 1;

			if (!texCoordV.empty()) {
				tempV3t -= 1;
				model->vertices[tempV3p.x].texCoord.x = texCoordV[tempV3t.x].x;
				model->vertices[tempV3p.y].texCoord.x = texCoordV[tempV3t.y].x;
				model->vertices[tempV3p.z].texCoord.x = texCoordV[tempV3t.z].x;
				model->vertices[tempV3p.x].texCoord.y = texCoordV[tempV3t.x].y;
				model->vertices[tempV3p.y].texCoord.y = texCoordV[tempV3t.y].y;
				model->vertices[tempV3p.z].texCoord.y = texCoordV[tempV3t.z].y;
			}
			if (!normalV.empty()) {
				tempV3n -= 1;
				model->vertices[tempV3p.x].normal.x = normalV[tempV3n.x].x;
				model->vertices[tempV3p.y].normal.x = normalV[tempV3n.y].x;
				model->vertices[tempV3p.z].normal.x = normalV[tempV3n.z].x;
				model->vertices[tempV3p.x].normal.y = normalV[tempV3n.x].y;
				model->vertices[tempV3p.y].normal.y = normalV[tempV3n.y].y;
				model->vertices[tempV3p.z].normal.y = normalV[tempV3n.z].y;
				model->vertices[tempV3p.x].normal.z = normalV[tempV3n.x].z;
				model->vertices[tempV3p.y].normal.z = normalV[tempV3n.y].z;
				model->vertices[tempV3p.z].normal.z = normalV[tempV3n.z].z;
			}
			model->indices.push_back(tempV3p.x);
			model->indices.push_back(tempV3p.y);
			model->indices.push_back(tempV3p.z);
		}
		line = strtok_s(NULL, "\n", &context);
	}

	model->indexSize = model->indices.size();

	if (strlen(mtlPath) != 0) model->pMaterial = AST->getMaterial(mtlPath);

	return model;
}

QeAssetMaterial* QeEncode::decodeMTL(char* buffer) {

	QeAssetMaterial* mtl = new QeAssetMaterial();
	mtl->type = eMaterialPhong;
	QeDataMaterialPhong mtl1;
	mtl1.ambient.w = 1;
	mtl1.diffuse.w = 1;
	mtl1.specular.w = 1;
	mtl1.emissive.w = 1;
	char diffuseMapPath[512];
	char *context;
	char* line = strtok_s(buffer, "\n", &context);

	while (line != nullptr) {

		if (strncmp(line, "map_Kd ", 7) == 0)	sscanf_s(line, "map_Kd %s", diffuseMapPath, (unsigned int) sizeof(diffuseMapPath));
		else if (strncmp(line, "Ns ", 3) == 0)	sscanf_s(line, "Ns %f", &mtl1.param.x);
		else if (strncmp(line, "Ka ", 3) == 0)	sscanf_s(line, "Ka %f %f %f", &(mtl1.ambient.x), &(mtl1.ambient.y), &(mtl1.ambient.z));
		else if (strncmp(line, "Kd ", 3) == 0)	sscanf_s(line, "Kd %f %f %f", &(mtl1.diffuse.x), &(mtl1.diffuse.y), &(mtl1.diffuse.z));
		else if (strncmp(line, "Ks ", 3) == 0)	sscanf_s(line, "Ks %f %f %f", &(mtl1.specular.x), &(mtl1.specular.y), &(mtl1.specular.z));
		else if (strncmp(line, "Ke ", 3) == 0)	sscanf_s(line, "Ke %f %f %f", &(mtl1.emissive.x), &(mtl1.emissive.y), &(mtl1.emissive.z));
		else if (strncmp(line, "Ni ", 3) == 0)	sscanf_s(line, "Ni %f", &(mtl1.param.y));
		else if (strncmp(line, "d ", 2) == 0)	sscanf_s(line, "d %f", &(mtl1.param.z));

		line = strtok_s(NULL, "\n", &context);
	}

	mtl->value.phong = mtl1;
	if (strlen(diffuseMapPath) != 0)	mtl->image.pDiffuseMap = AST->getImage(diffuseMapPath);

	return mtl;
}*/

QeAssetModel* QeEncode::decodeGLTF(QeAssetJSON *json, bool bCubeMap) {
	QeAssetModel* model = new QeAssetModel();

	const char* binData = AST->getJSONValue( json, 2, "buffers", "uri" );
	char *ret = strrchr((char*)binData, '.');
	std::vector<char> buf;
	if (strcmp(ret + 1, "bin") == 0) {
		std::string _filePath = AST->combinePath(binData, eAssetBin);
		buf = AST->loadFile(_filePath.c_str());
		binData = buf.data();
	}
	/*
	componentType			Size in bytes
	5120 (BYTE)				1
	5121 (UNSIGNED_BYTE)	1
	5122 (SHORT)			2
	5123 (UNSIGNED_SHORT)	2
	5125 (UNSIGNED_INT)		4
	5126 (FLOAT)			4

	type 		Number of components
	"SCALAR" 	1
	"VEC2" 		2
	"VEC3" 		3
	"VEC4" 		4
	"MAT2" 		4
	"MAT3" 		9
	"MAT4" 		16
	*/
	std::vector<QeAssetJSON*>* jaccessors = AST->getJSONArrayNodes(json, 1, "accessors");
	std::vector<QeAssetJSON*>* jbufferViews = AST->getJSONArrayNodes(json, 1, "bufferViews");
	std::vector<std::string>* sv;
	size_t size, size1, size2;
	unsigned char index;
	unsigned int offset, count, length, componentType;
	size_t i, j, k;

	unsigned char bufferViews[256];
	memset(bufferViews, 0xFF, 256 * sizeof(char));
	const char* c = AST->getJSONValue(json, 3, "meshes", "primitives", "indices");
	if (c != nullptr) bufferViews[0] = atoi(c); 

	c = AST->getJSONValue(json, 4, "meshes", "primitives", "attributes", "POSITION");
	if (c != nullptr) bufferViews[1] = atoi(c);

	c = AST->getJSONValue(json, 4, "meshes", "primitives", "attributes", "NORMAL");
	if (c != nullptr) bufferViews[2] = atoi(c); 

	c = AST->getJSONValue(json, 4, "meshes", "primitives", "attributes", "TEXCOORD_0");
	if (c != nullptr) bufferViews[3] = atoi(c);

	c = AST->getJSONValue(json, 4, "meshes", "primitives", "attributes", "TANGENT");
	if (c != nullptr) bufferViews[4] = atoi(c);

	c = AST->getJSONValue(json, 4, "meshes", "primitives", "attributes", "JOINTS_0");
	if (c != nullptr) bufferViews[5] = atoi(c);

	c = AST->getJSONValue(json, 4, "meshes", "primitives", "attributes", "WEIGHTS_0");
	if (c != nullptr) bufferViews[6] = atoi(c);

	sv = AST->getJSONArrayValues(json, 2, "nodes", "scale");
	if (sv == nullptr || sv->empty())	model->scale = { 1.0f, 1.0f, 1.0f };
	else				model->scale = { float(atof((*sv)[0].c_str())), float(atof((*sv)[1].c_str())), float(atof((*sv)[2].c_str())) };

	c = AST->getJSONValue(json, 2, "skins", "inverseBindMatrices");
	if (c != nullptr) {

		// skeletal animation
		bufferViews[7] = atoi(c);
		std::vector<std::string>* jboneID = AST->getJSONArrayValues(json, 2, "skins", "joints");
		size = jboneID->size();
		model->jointsAnimation.resize(size);

		std::vector<QeAssetJSON*>* jboneName = AST->getJSONArrayNodes(json, 1, "nodes");

		for ( i = 0; i < size; ++i) {
			model->jointsAnimation[i].id = atoi((*jboneID)[i].c_str());
			model->jointsAnimation[i].name = AST->getJSONValue((*jboneName)[model->jointsAnimation[i].id], 1, "name");
			
			if (strncmp(model->jointsAnimation[i].name, BONE_ROOT_NAME, 13) == 0) model->rootJoint = &model->jointsAnimation[i];

			/*sv = AST->getJSONArrayValues((*jboneName)[model->jointsAnimation[i].id], 1, "translation");
			if (sv != nullptr) {
				model->jointsAnimation[i].translation.x = float(atof((*sv)[0].c_str()));
				model->jointsAnimation[i].translation.y = float(atof((*sv)[1].c_str()));
				model->jointsAnimation[i].translation.z = float(atof((*sv)[2].c_str()));
			}
			sv = AST->getJSONArrayValues((*jboneName)[model->jointsAnimation[i].id], 1, "rotation");
			if (sv != nullptr) {
				model->jointsAnimation[i].rotation.x = float(atof((*sv)[0].c_str()));
				model->jointsAnimation[i].rotation.y = float(atof((*sv)[1].c_str()));
				model->jointsAnimation[i].rotation.z = float(atof((*sv)[2].c_str()));
				model->jointsAnimation[i].rotation.w = float(atof((*sv)[3].c_str()));
			}
			model->jointsAnimation[i].transform = MATH->transform(model->jointsAnimation[i].translation, model->jointsAnimation[i].rotation, QeVector3f(1,1,1));
			sv = AST->getJSONArrayValues((*jboneName)[model->jointsAnimation[i].id], 1, "scale");
			if (sv != nullptr) {
				model->jointsAnimation[i].scale.x = float(atof((*sv)[0].c_str()));
				model->jointsAnimation[i].scale.y = float(atof((*sv)[1].c_str()));
				model->jointsAnimation[i].scale.z = float(atof((*sv)[2].c_str()));
			}*/
		}
		//QeMatrix4x4f mat;
		//setChildrenJointTranform(model->rootJoint, mat);

		for (i = 0; i < size; ++i) {

			sv = AST->getJSONArrayValues((*jboneName)[model->jointsAnimation[i].id], 1, "children");
			if (sv != nullptr) {
				size1 = sv->size();
				model->jointsAnimation[i].children.resize(size1);
				for (j = 0; j < size1; ++j) {
					index = atoi((*sv)[j].c_str());
					size2 = model->jointsAnimation.size();
					for (k = 0; k<size2 ;++k ) {
						if( index == model->jointsAnimation[k].id)
							model->jointsAnimation[i].children[j] = &model->jointsAnimation[k];
					}
				}
			}
		}

		std::vector<QeAssetJSON*>* jchannels = AST->getJSONArrayNodes(json, 2, "animations", "channels");
		std::vector<QeAssetJSON*>* jsmaplers = AST->getJSONArrayNodes(json, 2, "animations", "samplers");
		size1 = jchannels->size();
		const char* path = nullptr;

		for ( i = 0; i < size1; ++i) {
			j = atoi(AST->getJSONValue((*jchannels)[i], 2, "target", "node"));

			for (k = 0; k< size ; ++k) {
				if (model->jointsAnimation[k].id == j) {

					path = AST->getJSONValue((*jchannels)[i], 2, "target", "path");
					index = atoi(AST->getJSONValue((*jsmaplers)[i], 1, "input"));
					count = atoi(AST->getJSONValue((*jaccessors)[index], 1, "count"));
					offset = atoi(AST->getJSONValue((*jbufferViews)[index], 1, "byteOffset"));
					length = atoi(AST->getJSONValue((*jbufferViews)[index], 1, "byteLength"));

					if (strncmp(path, "translation", 11) == 0) {
						model->jointsAnimation[k].translationInput.resize(count);
						memcpy(model->jointsAnimation[k].translationInput.data(), binData + offset, length);
					}
					else if (strncmp(path, "rotation", 8) == 0) {
						model->jointsAnimation[k].rotationInput.resize(count);
						memcpy(model->jointsAnimation[k].rotationInput.data(), binData + offset, length);
					}
					//else if (strncmp(path, "scale", 5) == 0) {
					//	model->jointsAnimation[k].scaleInput.resize(count);
					//	memcpy(model->jointsAnimation[k].scaleInput.data(), binData + offset, length);
					//}
					index = atoi(AST->getJSONValue((*jsmaplers)[i], 1, "output"));
					count = atoi(AST->getJSONValue((*jaccessors)[index], 1, "count"));
					offset = atoi(AST->getJSONValue((*jbufferViews)[index], 1, "byteOffset"));
					length = atoi(AST->getJSONValue((*jbufferViews)[index], 1, "byteLength"));
					
					if (strncmp(path, "translation", 11) == 0) {
						model->jointsAnimation[k].translationOutput.resize(count);
						memcpy(model->jointsAnimation[k].translationOutput.data(), binData + offset, length);
					}
					else if (strncmp(path, "rotation", 8) == 0) {
						model->jointsAnimation[k].rotationOutput.resize(count);
						memcpy(model->jointsAnimation[k].rotationOutput.data(), binData + offset, length);
					}
					//else if (strncmp(path, "scale", 5) == 0) {
					//	model->jointsAnimation[k].scaleOutput.resize(count);
					//	memcpy(model->jointsAnimation[k].scaleOutput.data(), binData + offset, length);
					//}
					break;
				}
			}
		}

		// If an action doesn't move for EMPTY_FRAMES frames, it means next action.
		const unsigned char EMPTY_FRAMES = 5;
		unsigned char notMoveFrameCounts = 0;

		size = model->jointsAnimation[0].translationInput.size();
		//size1 = model->jointsAnimation[0].rotationInput.size();
		//if (size1 > size) size = size1;
		//size1 = model->jointsAnimation[0].scaleInput.size();
		//if (size1 > size) size = size1;

		size1 = model->jointsAnimation.size();
		model->animationStartFrames.push_back(0);

		for (i = 1; i < size ; ++i) {
			if (notMoveFrameCounts == EMPTY_FRAMES) {
				notMoveFrameCounts = 0;
				model->animationEndFrames.push_back(unsigned int(i - EMPTY_FRAMES)-1);
				model->animationStartFrames.push_back(unsigned int(i));
			}
			for ( j = 0 ; j < size1 ; ++j ) {
				if ( /*(i< model->jointsAnimation[j].translationOutput.size()) &&*/ (model->jointsAnimation[j].translationOutput[i] != model->jointsAnimation[j].translationOutput[i-1]) ) {
					notMoveFrameCounts = 0;
					break;
				}
				if ( /*(i< model->jointsAnimation[j].rotationOutput.size()) &&*/ (model->jointsAnimation[j].rotationOutput[i] != model->jointsAnimation[j].rotationOutput[i-1]) ) {
					notMoveFrameCounts = 0;
					break;
				}
				//if ((i< model->jointsAnimation[j].scaleOutput.size()) && (model->jointsAnimation[j].scaleOutput[i] != model->jointsAnimation[j].scaleOutput[i-1]) ) {
				//	notMoveFrameCounts = 0;
				//	break;
				//}
			}
			if (j == size1) ++notMoveFrameCounts;
		}
		model->animationEndFrames.push_back(unsigned int(i)-1);
		model->animationNum = unsigned char(model->animationEndFrames.size());
	}
	size = jbufferViews->size();

	for (i = 0; i < size; ++i) {
		index = atoi(AST->getJSONValue((*jaccessors)[i], 1, "bufferView"));
		count = atoi(AST->getJSONValue((*jaccessors)[i], 1, "count"));
		componentType = atoi(AST->getJSONValue((*jaccessors)[i], 1, "componentType"));
		offset = atoi(AST->getJSONValue((*jbufferViews)[i], 1, "byteOffset"));
		length = atoi(AST->getJSONValue((*jbufferViews)[i], 1, "byteLength"));

		if (index == bufferViews[0]) { // indices
			if (componentType == 5121) {
				unsigned char* dataPos = (unsigned char*)(binData + offset);
				model->indexSize = count;
				for (j = 0; j < count; ++j) model->indices.push_back(*(dataPos + j));
			}
			else if (componentType == 5123) {
				unsigned short int* dataPos = (unsigned short int*)(binData + offset);
				model->indexSize = count;
				for (j = 0; j < count; ++j) model->indices.push_back(*(dataPos + j));
			}
			else if (componentType == 5125) {
				unsigned int* dataPos = (unsigned int*)(binData + offset);
				model->indexSize = count;
				for (j = 0; j < count; ++j) model->indices.push_back(*(dataPos + j));
		}}
		else if (index == bufferViews[1]) { // position
			QeVector3f* dataPos = (QeVector3f*)(binData + offset);
			if (model->vertices.size() < count)	model->vertices.resize(count);
			for (j = 0; j < count; ++j) {
				model->vertices[j].pos = *(dataPos + j);
				model->vertices[j].color = { 1.0f, 1.0f, 1.0f, 1.0f };
		}}
		else if (index == bufferViews[2]) { // normal
			QeVector3f* dataPos = (QeVector3f*)(binData + offset);
			if (model->vertices.size() < count)	model->vertices.resize(count);
			for (j = 0; j < count; ++j) model->vertices[j].normal = *(dataPos + j);
		}
		else if (index == bufferViews[3]) { // texcoord
			QeVector2f* dataPos = (QeVector2f*)(binData + offset);
			if (model->vertices.size() < count)	model->vertices.resize(count);
			for (j = 0; j < count; ++j) model->vertices[j].texCoord = *(dataPos + j);
		}
		else if (index == bufferViews[4]) { // tangent
			QeVector4f* dataPos = (QeVector4f*)(binData + offset);
			if (model->vertices.size() < count)	model->vertices.resize(count);
			for (j = 0; j < count; ++j) model->vertices[j].tangent = *(dataPos + j);
		}
		else if (index == bufferViews[5]) { // joint
			QeVector4s* dataPos = (QeVector4s*)(binData + offset);
			if (model->vertices.size() < count)	model->vertices.resize(count);
			for (j = 0; j < count; ++j)	model->vertices[j].joint = *(dataPos + j);

			//for (j = 0; j < count; ++j) {
			//	model->vertices[j].joint[0] = (dataPos + j)->x;
			//	model->vertices[j].joint[1] = (dataPos + j)->y;
			//	model->vertices[j].joint[2] = (dataPos + j)->z;
			//	model->vertices[j].joint[3] = (dataPos + j)->w;
			//}
			//model->joints.resize(count);
			//for (j = 0; j < count; ++j) model->joints[j] = *(dataPos + j);
			//memcpy(model->joints.data(), dataPos, length);
		}
		else if (index == bufferViews[6]) { // weight
			QeVector4f* dataPos = (QeVector4f*)(binData + offset);
			if (model->vertices.size() < count)	model->vertices.resize(count);
			float totalWeight = 1;
			int lastWeight = 0;
			for (j = 0; j < count; ++j) {
				model->vertices[j].weight = *(dataPos + j);
				totalWeight = 1;
				if (model->vertices[j].weight.x > 0) {
					totalWeight -= model->vertices[j].weight.x;
					lastWeight = 0;
				}
				if (model->vertices[j].weight.y > 0) {
					totalWeight -= model->vertices[j].weight.y;
					lastWeight = 1;
				}
				if (model->vertices[j].weight.z > 0) {
					totalWeight -= model->vertices[j].weight.z;
					lastWeight = 2;
				}
				if (model->vertices[j].weight.w > 0) {
					totalWeight -= model->vertices[j].weight.w;
					lastWeight = 3;
				}
				*(((float*)(&model->vertices[j].weight)) + lastWeight) += totalWeight;
			}
		}
		else if (index == bufferViews[7]) { // inverseBindMatrices  bone matrices
			QeMatrix4x4f* dataPos = (QeMatrix4x4f*)(binData + offset);
			for (j = 0; j < count; ++j) model->jointsAnimation[j].inverseBindMatrix = *(dataPos + j);
		}
	}

	// material
	QeAssetMaterial *pMaterial = new QeAssetMaterial();
	//pMaterial->type = eMaterialPBR;
	model->pMaterial = pMaterial;
	
	int textureIndex = atoi(AST->getJSONValue(json, 4, "materials", "pbrMetallicRoughness", "baseColorTexture", "index"));
	std::vector<QeAssetJSON*>* imageJSON = AST->getJSONArrayNodes(json, 1, "images");
	const char* texturePath = AST->getJSONValue((*imageJSON)[textureIndex], 1, "uri");
	
	if(bCubeMap)pMaterial->image.pCubeMap = AST->getImage(texturePath, bCubeMap);
	else		pMaterial->image.pBaseColorMap = AST->getImage(texturePath, bCubeMap);
	
	textureIndex = atoi(AST->getJSONValue(json, 3, "materials", "normalTexture", "index"));
	imageJSON = AST->getJSONArrayNodes(json, 1, "images");
	texturePath = AST->getJSONValue((*imageJSON)[textureIndex], 1, "uri");

	pMaterial->image.pNormalMap = AST->getImage(texturePath, bCubeMap);

	std::vector<std::string>* baseColorJ = AST->getJSONArrayValues(json, 3, "materials", "pbrMetallicRoughness", "baseColorFactor");
	//QeDataMaterialPBR mtl;
	QeDataMaterial mtl;

	if (baseColorJ != nullptr) {
		mtl.baseColor.x = float(atof((*baseColorJ)[0].c_str()));
		mtl.baseColor.y = float(atof((*baseColorJ)[1].c_str()));
		mtl.baseColor.z = float(atof((*baseColorJ)[2].c_str()));
		mtl.baseColor.w = float(atof((*baseColorJ)[3].c_str()));
	}
	else {
		mtl.baseColor.x = 1.0f;
		mtl.baseColor.y = 1.0f;
		mtl.baseColor.z = 1.0f;
		mtl.baseColor.w = 1.0f;
	}
	const char* value = AST->getJSONValue(json, 3, "materials", "pbrMetallicRoughness", "metallicFactor");
	if (value == nullptr)	mtl.metallicRoughness.x = 1.0f;
	else					mtl.metallicRoughness.x = float(atof(value));

	value = AST->getJSONValue(json, 3, "materials", "pbrMetallicRoughness", "roughnessFactor");
	if (value == nullptr)	mtl.metallicRoughness.y = 1.0f;
	else					mtl.metallicRoughness.y = float(atof(value));

	pMaterial->value = mtl;

	return model;
}

//QeAssetModel* QeEncode::decodeGLB(char* buffer) { return nullptr; }
QeAssetParticleRule* QeEncode::decodeParticle(QeAssetXML* node) {
	QeAssetParticleRule* particle = new QeAssetParticleRule();

	//const char* c = AST->getXMLValue(node, 1, "image");
	//if (c != nullptr) particle->image = AST->getImage(c);
	particle->image = AST->getXMLValue( node, 1, "image");
	AST->getXMLfValue(particle->count_life.x, *node, 2, "count", "num");
	AST->getXMLfValue(particle->count_life.y, *node, 2, "count", "range");
	AST->getXMLfValue(particle->count_life.z, *node, 2, "life", "second");
	AST->getXMLfValue(particle->count_life.w, *node, 2, "life", "range");
	AST->getXMLfValue(particle->init_pos_lw.x, *node, 2, "init_pos", "length");
	AST->getXMLfValue(particle->init_pos_lw.y, *node, 2, "init_pos", "length_range");
	AST->getXMLfValue(particle->init_pos_lw.z, *node, 2, "init_pos", "width");
	AST->getXMLfValue(particle->init_pos_lw.w, *node, 2, "init_pos", "width_range");
	AST->getXMLfValue(particle->init_pos_hr.x, *node, 2, "init_pos", "height");
	AST->getXMLfValue(particle->init_pos_hr.y, *node, 2, "init_pos", "height_range");
	AST->getXMLfValue(particle->init_pos_hr.z, *node, 2, "init_pos", "radius");
	AST->getXMLfValue(particle->init_pos_hr.w, *node, 2, "init_pos", "radius_range");
	AST->getXMLfValue(particle->init_speed_xy.x, *node, 2, "init_speed", "x");
	AST->getXMLfValue(particle->init_speed_xy.y, *node, 2, "init_speed", "x_range");
	AST->getXMLfValue(particle->init_speed_xy.z, *node, 2, "init_speed", "y");
	AST->getXMLfValue(particle->init_speed_xy.w, *node, 2, "init_speed", "y_range");
	AST->getXMLfValue(particle->init_speed_z_force_x.x, *node, 2, "init_speed", "z");
	AST->getXMLfValue(particle->init_speed_z_force_x.y, *node, 2, "init_speed", "z_range");
	AST->getXMLfValue(particle->init_speed_z_force_x.z, *node, 2, "force", "x");
	AST->getXMLfValue(particle->init_speed_z_force_x.w, *node, 2, "force", "x_range");
	AST->getXMLfValue(particle->force_yz.x, *node, 2, "force", "y");
	AST->getXMLfValue(particle->force_yz.y, *node, 2, "force", "y_range");
	AST->getXMLfValue(particle->force_yz.z, *node, 2, "force", "z");
	AST->getXMLfValue(particle->force_yz.w, *node, 2, "force", "z_range");
	AST->getXMLfValue(particle->rotate_xy.x, *node, 2, "rotate", "x");
	AST->getXMLfValue(particle->rotate_xy.y, *node, 2, "rotate", "x_range");
	AST->getXMLfValue(particle->rotate_xy.z, *node, 2, "rotate", "y");
	AST->getXMLfValue(particle->rotate_xy.w, *node, 2, "rotate", "y_range");
	AST->getXMLfValue(particle->rotate_z_color_r.x, *node, 2, "rotate", "z");
	AST->getXMLfValue(particle->rotate_z_color_r.y, *node, 2, "rotate", "z_range");
	AST->getXMLfValue(particle->rotate_z_color_r.z, *node, 2, "color", "r");
	AST->getXMLfValue(particle->rotate_z_color_r.w, *node, 2, "color", "r_range");
	AST->getXMLfValue(particle->color_gb.x, *node, 2, "color", "g");
	AST->getXMLfValue(particle->color_gb.y, *node, 2, "color", "g_range");
	AST->getXMLfValue(particle->color_gb.z, *node, 2, "color", "b");
	AST->getXMLfValue(particle->color_gb.w, *node, 2, "color", "b_range");
	AST->getXMLfValue(particle->alpha_born_size_x.x, *node, 1, "alpha");
	AST->getXMLfValue(particle->alpha_born_size_x.y, *node, 1, "reborn");
	AST->getXMLfValue(particle->alpha_born_size_x.z, *node, 2, "size", "x");
	AST->getXMLfValue(particle->alpha_born_size_x.w, *node, 2, "size", "x_range");
	AST->getXMLfValue(particle->size_y_init_pos_d.x, *node, 2, "size", "y");
	AST->getXMLfValue(particle->size_y_init_pos_d.y, *node, 2, "size", "y_range");
	AST->getXMLfValue(particle->size_y_init_pos_d.z, *node, 2, "init_pos", "degree");
	AST->getXMLfValue(particle->size_y_init_pos_d.w, *node, 2, "init_pos", "degree_range");

	return particle;
}

std::vector<unsigned char> QeEncode::decodeJPEG(unsigned char* buffer, size_t size, int* width, int* height, int* bytes) {

	std::vector<unsigned char> ret;
	// YCbCr(YUV), DCT(Discrete Cosine Transform), Quantization, Zig-zag(Entropy Coding), RLC(Run Length Coding), Canonical Huffman Code

	unsigned char startKey[2]	 = { 0xFF, 0xD8 }; // SOI  Start of Image
	unsigned char APP0[2]		 = { 0xFF, 0xE0 }; // APP0 Application
	unsigned char quanKey[2]	 = { 0xFF, 0xDB }; // DQT  Define Quantization Table
	unsigned char frameKey[2]	 = { 0xFF, 0xC0 }; // SOF0 Start of Frame
	unsigned char huffmanKey[2]  = { 0xFF, 0xC4 }; // DHT  Difine Huffman Table
	unsigned char scanKey[2]	 = { 0xFF, 0xDA }; // SOS  Start of Scan
	unsigned char endKey[2]		 = { 0xFF, 0xD9 }; // EOI  End of Image

	if (memcmp(buffer, startKey, 2) != 0) return ret;

	unsigned short int length = 0;
	char* key;
	unsigned char* data = nullptr;
	size_t index = 2;
	unsigned char buf[2];
	std::vector <unsigned char> dataHuffman;
	QeHuffmanTree2 DC[2]; // DC00, DC01
	QeHuffmanTree2 AC[2]; // AC10, AC11
	unsigned char colorNum;
	unsigned char* mcusType = nullptr;
	unsigned char* mcusQuan = nullptr;
	unsigned char* mcusSize = nullptr;
	unsigned char* huffmanTreeIndex = nullptr;
	unsigned int mcuWidth = 0;
	unsigned int mcuHeight = 0;
	unsigned int totalmcuSize = 0;
	unsigned char* quanData[4] = {nullptr, nullptr, nullptr, nullptr };

	size_t i = 0, j = 0;
	while (1) {

		key = (char*)(buffer + index);
		//if (memcmp(key, endKey, 2) == 0)	break;

		buf[0] = *(buffer + index + 3);
		buf[1] = *(buffer + index + 2);
		length = *(unsigned short int*)(buf) - 2;
		data = (buffer + index + 4);

		//else if (memcmp(key, APP0, 2) == 0) {}
		if (memcmp(key, quanKey, 2) == 0)	quanData[data[0] % 16] = data + 1;
		else if (memcmp(key, frameKey, 2) == 0) {
			unsigned char colorBits = data[0];
			buf[1] = data[1];
			buf[0] = data[2];
			*width = *(short int*)&buf;
			buf[1] = data[3];
			buf[0] = data[4];
			*height = *(short int*)&buf;
			colorNum = data[5];
			
			char bits = 0;
			if (colorNum == 1)	bits = colorBits;
			else				bits = colorBits * 3;
			*bytes = (bits + 7) / 8;

			mcusType = new unsigned char[colorNum];
			mcusQuan = new unsigned char[colorNum];
			mcusSize = new unsigned char[colorNum];
			int maxSize = 0;
			for (i = 0; i<colorNum;++i ) {
				j = 0;
				mcusType[i] = data[6 + i * 3];
				int x = readBits(&data[7 + i * 3], &j, 4);
				int y = readBits(&data[7 + i * 3], &j, 4);
				mcusSize[i] = x* y;
				if (mcusSize[i] > maxSize) {
					maxSize = mcusSize[i];
					mcuWidth  = ((*width) +7)/8/x;
					mcuHeight = ((*height)+7)/8/y;
				}
				mcusQuan[i] = data[8 + i * 3];
			}
			totalmcuSize = mcuWidth*mcuHeight;
		}
		else if (memcmp(key, huffmanKey, 2) == 0) {

			QeHuffmanTree2 *tree = nullptr;
			
			if (data[0] / 16)	tree = &AC[data[0] % 16];
			else				tree = &DC[data[0] % 16];

			size_t length = 0;
			for (i = 0; i<16; ++i)	length += data[1 + i];

			tree->size = length;
			tree->codeBits = new unsigned int[length];
			tree->values = new unsigned int[length];
			tree->codes = new unsigned int[length];
			tree->codes[0] = 0;
			
			int itreeIndex = 0;
			int ntreeIndex = 1;
			while (data[1+itreeIndex] == 0) ++itreeIndex;
			tree->codeBits[0] = itreeIndex+1;
			i = 0;
			while (1) {
				tree->values[i] = data[17 + i];
				if ((i+1) == length) break;

				while (data[1+itreeIndex] <= ntreeIndex) {
					++itreeIndex;
					ntreeIndex = 0;
				}
				tree->codes[i + 1] = tree->codes[i] + 1;
				tree->codeBits[i + 1] = itreeIndex+1;

				if (ntreeIndex == 0)	tree->codes[i + 1] <<= (tree->codeBits[i + 1]- tree->codeBits[i]);
		
				++ntreeIndex;
				++i;
			}
		}
		else if (memcmp(key, scanKey, 2) == 0) {
			
			huffmanTreeIndex = new unsigned char[data[0]];
			for (i = 0;i<data[0];++i ) huffmanTreeIndex[i] = data[2 + i * 2];

			unsigned char* dataPos = data + length;
			size_t lengthData = size - (dataPos-buffer);
			size_t lengthData1 = 0;
			unsigned char* dataPos1 = nullptr;
			while (1) {
				dataPos1 = (unsigned char*)memchr(dataPos+ lengthData1, 0xFF, lengthData);

				if (dataPos1[1] == 0) {
					lengthData1 = dataPos1 - dataPos + 1;
					dataHuffman.insert(dataHuffman.end(), dataPos, dataPos + lengthData1);
					dataPos += (lengthData1+1);
					lengthData -= (lengthData1+1);
					lengthData1 = 0;
				}
				else {
					lengthData1 = dataPos1 - dataPos;
					dataHuffman.insert(dataHuffman.end(), dataPos, dataPos + lengthData1);
					break;
				}
			}
			break;
		}
		index += (length + 2 * 2);
	}

	// MCU(Minimum Coded Unit)
	short int** mcuDatas = new short int*[colorNum];

	for (i = 0; i < colorNum; ++i) {
		mcuDatas[i] = new short int[64 * totalmcuSize*mcusSize[i]];
		memset(mcuDatas[i],0, 64 * totalmcuSize*mcusSize[i]* sizeof(mcuDatas[0][0]));
	}

	size_t bitPointer = 0;

	for (i = 0; i < totalmcuSize; ++i) { // decode Huffman
		for (j = 0; j < colorNum; ++j) {
			index = 0;
			unsigned char dc = readBits(&huffmanTreeIndex[j], &index, 4);
			unsigned char ac = readBits(&huffmanTreeIndex[j], &index, 4);
			getHuffmanDecodeSymbolfromDCAC( mcuDatas[j] + mcusSize[j]*i*64, mcusSize[j], dataHuffman.data(), &bitPointer, &DC[dc], &AC[ac]);
		}
	}
	for (i = 0; i < colorNum; ++i) { // DCn=DCn-1+Diff
		index = mcusSize[i] * totalmcuSize;
		for (j = 1; j < index; ++j)	mcuDatas[i][j * 64] += mcuDatas[i][(j - 1) * 64];
	}
	int k = 0;
	for (i = 0; i < colorNum; ++i) { // reverse Quantization
		index = mcusSize[i] * totalmcuSize;
		for (j = 0; j < index; ++j) {
			for (k = 0; k<64; ++k)		mcuDatas[i][j * 64 + k] *= quanData[mcusQuan[i]][k];
		}
	}
	short int buffer1[64];
	for (i = 0; i < colorNum; ++i) {	// reverse Zig-zag
		index = mcusSize[i] * totalmcuSize;
		for (j = 0; j < index; ++j) {
			memcpy(buffer1, &mcuDatas[i][j * 64], 64 * sizeof(mcuDatas[0][0]));
			for (k = 0; k<64; ++k)	mcuDatas[i][j * 64 + k] = buffer1[ZIGZAGTABLE[k]];
		}
	}
	for (i = 0; i < colorNum; ++i) {	// odd lines change signed
		index = mcusSize[i] * totalmcuSize;
		for (j = 0; j < index; ++j) {
			for (k = 0; k<64; ++k)	if (k / 8 % 2 == 1)	mcuDatas[i][j * 64 + k] *= -1;
		}
	}
	int buffer2[64];
	for (i = 0; i < colorNum; ++i) {	// IDCT
		index = mcusSize[i] * totalmcuSize;
		for (j = 0; j < index; ++j) {
			for (k = 0; k < 64; ++k)	buffer2[k] = mcuDatas[i][j * 64 + k];
			FastIntegerIDCT(buffer2);
			for (k = 0; k < 64; ++k)	mcuDatas[i][j * 64 + k] = buffer2[k];
		}
	}
	// YCrCb to RGB
	double cY, cCb, cCr;
	ret.resize( *width * *height* *bytes );
	size_t x = 0;
	size_t y = 0;
	if (mcusSize[0] == 1) {
		for (i = 0; i < totalmcuSize; ++i) {
			for (j = 0; j < 64; ++j) {
				
				index = i * 64 + j;
				cY  = mcuDatas[0][index];
				cCb = mcuDatas[1][index];
				cCr = mcuDatas[2][index];

				x = i % mcuWidth * 8 + j % 8;
				y = i / mcuWidth * 8 + 7-(j / 8);
				index = y* *width + x;

				ret[index*3]	 = unsigned char(MATH->clamp(int(cY + 1.402 * cCr + 128), 0, 255));							// R
				ret[index*3 + 1] = unsigned char(MATH->clamp(int(cY - 0.3441363 * cCb - 0.71413636 * cCr + 128), 0, 255));	// G
				ret[index*3 + 2] = unsigned char(MATH->clamp(int(cY + 1.772 * cCb + 128), 0, 255));							// B
			}
		}
	} else if(mcusSize[0] == 4){
		for (i = 0; i < totalmcuSize; ++i) {
			for (j = 0; j < 4; ++j) {
				for (k = 0; k < 64; ++k) {

					index = (i * 4 + j) * 64 + k;
					cY = mcuDatas[0][index];
					index = index/4;
					cCb = mcuDatas[1][index];
					cCr = mcuDatas[2][index];

					x = i % mcuWidth * 16 + j % 2 * 8 + k % 8;
					y = i / mcuWidth * 16 + j / 2 * 8 + 7-(k / 8);
					index = y* *width + x;
					
					ret[index*3]		= unsigned char(MATH->clamp(int(cY + 1.402 * cCr +128), 0, 255));							// R
					ret[index*3 + 1]	= unsigned char(MATH->clamp(int(cY - 0.3441363 * cCb - 0.71413636 * cCr + 128), 0, 255));	// G
					ret[index*3 + 2]	= unsigned char(MATH->clamp(int(cY + 1.772 * cCb +128), 0, 255));							// B
				}
			}
		}
	}
	if (mcusType != nullptr) delete[] mcusType;
	if (mcusQuan != nullptr) delete[] mcusQuan;
	if (mcusSize != nullptr) delete[] mcusSize;
	if (huffmanTreeIndex != nullptr) delete[] huffmanTreeIndex;

	if (mcuDatas != nullptr) {
		for (i = 0; i < colorNum; ++i)	if(mcuDatas[i] != nullptr) delete mcuDatas[i];
		delete[] mcuDatas;
	}
	return ret;
}

void QeEncode::FastIntegerIDCT(int* i8x8) {
	int i, b2[64];
	for (i = 0; i<8; i++) idct1(i8x8 + i * 8, b2 + i, 9, 1 << 8);
	for (i = 0; i<8; i++) idct1(b2 + i * 8, i8x8 + i, 12, 1 << 11);
}

void QeEncode::idct1(int *x, int *y, int ps, int half){

	int p, n;
	x[0] <<= 9, x[1] <<= 7, x[3] *= 181, x[4] <<= 9, x[5] *= 181, x[7] <<= 7;
	xmul(&x[6], &x[2], 277, 669, 0, &p, &n);
	xadd3(&x[0], &x[4], &x[6], &x[2], half, &p, &n);
	xadd3(&x[1], &x[7], &x[3], &x[5], 0, &p, &n);
	xmul(&x[5], &x[3], 251, 50, 6, &p, &n);
	xmul(&x[1], &x[7], 213, 142, 6, &p, &n);
	y[0 * 8] = (x[0] + x[1]) >> ps;
	y[1 * 8] = (x[4] + x[5]) >> ps;
	y[2 * 8] = (x[2] + x[3]) >> ps;
	y[3 * 8] = (x[6] + x[7]) >> ps;
	y[4 * 8] = (x[6] - x[7]) >> ps;
	y[5 * 8] = (x[2] - x[3]) >> ps;
	y[6 * 8] = (x[4] - x[5]) >> ps;
	y[7 * 8] = (x[0] - x[1]) >> ps;
}

void QeEncode::xadd3(int* xa, int* xb, int* xc, int* xd, int h, int* p, int* n) {
	*p  = *xa + *xb;			
	*n = *xa - *xb;
	*xa = *p + *xc + h;		
	*xb = *n + *xd + h;
	*xc = *p - *xc + h;		
	*xd = *n - *xd + h;
}
void QeEncode::xmul(int* xa, int* xb, int k1, int k2, int sh, int* p, int* n) {
	*n  = k1*(*xa + *xb);		
	*p = *xa;
	*xa = (*n + (k2 - k1)**xb) >> sh;
	*xb = (*n - (k2 + k1)* *p) >> sh;
}

void QeEncode::getHuffmanDecodeSymbolfromDCAC( short int* out, unsigned char blocks, const unsigned char* in, size_t* bitPointer, const QeHuffmanTree2* dc, const QeHuffmanTree2* ac) {

	int value1 = 0, value2 = 0, value3 = 0;
	size_t index = 0, index1 = 0;

	int j = 0;
	for (int i = 0; i < blocks; ++i) {
		value1 = getHuffmanDecodeSymbol(in, bitPointer, dc);
		value2 = readBits(in, bitPointer, value1, true, true);
		out[index] = value2;
		++index;

		while (index%64 != 0) {

			value1 = getHuffmanDecodeSymbol(in, bitPointer, ac);
			if (value1 == 0)	break;

			index1 = 0;
			value2 = readBits((unsigned char*)&value1, &index1, 4);
			value3 = readBits((unsigned char*)&value1, &index1, 4);
			index += value3;

			if (value2 > 0)	out[index] = readBits(in, bitPointer, value2, true, true);
			else			out[index] = 0;
			++index;
		}
		index = (index + 63) / 64 * 64;
	}
}

unsigned int QeEncode::getHuffmanDecodeSymbol(const unsigned char* in, size_t* bitPointer, const QeHuffmanTree2* tree){

	unsigned char codeBits = 0;
	unsigned int key = 0;
	size_t oBitPointer = *bitPointer;

	for (int i = 0; i < tree->size; ++i) {
		if (tree->codeBits[i] != codeBits) {
			*bitPointer = oBitPointer;
			codeBits = tree->codeBits[i];
			key = readBits(in, bitPointer, codeBits, true);
		}
		if (key == tree->codes[i]) return tree->values[i];
	}
	*bitPointer = oBitPointer;
	return 0;
}

std::vector<unsigned char> QeEncode::decodeBMP(unsigned char* buffer, int* width, int* height, int* bytes) {

	std::vector<unsigned char> ret;
	if (strncmp((char*)buffer, "BM", 2) != 0) return ret;

	*width = *(int*)(buffer + 0x12);
	*height = *(int*)(buffer + 0x16);
	short int *bits = (short int*)(buffer + 0x1C);
	*bytes = (*bits + 7) / 8;
	int	imageSize = *width * *height * *bytes;

	ret.resize(imageSize);
	memcpy(ret.data(), buffer + 0x0A, imageSize);

	return ret;
}

std::vector<unsigned char> QeEncode::decodePNG(unsigned char* buffer, int* width, int* height, int* bytes) {
	
	std::vector<unsigned char> ret;
	unsigned char headerKey[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
	if (memcmp(buffer, headerKey, 8) != 0) return ret;

	//IHDR 0x08 - 0x20
	//PLTE
	//IDAT
	//IEND 0x00 0x00 0x00 0x00 0x49 0x45 0x4E 0x44 0xAE 0x42 0x60 0x82
	char buf[4];
	buf[0] = buffer[0x13];
	buf[1] = buffer[0x12];
	buf[2] = buffer[0x11];
	buf[3] = buffer[0x10];
	*width = *(int*)&buf;
	buf[0] = buffer[0x17];
	buf[1] = buffer[0x16];
	buf[2] = buffer[0x15];
	buf[3] = buffer[0x14];
	*height = *(int*)&buf;
	int bitDepth = buffer[0x18];
	int colorType = buffer[0x19];
	int bits = 0;

	switch (colorType)
	{
	case 0: bits = 1; break;	// grey
	case 2: bits = 3; break;	// RGB
	case 3: bits = 1; break;	// palette
	case 4: bits = 2; break;	// grey + alpha
	case 6: bits = 4; break;	// RGBA
	}
	bits *= bitDepth;

	*bytes = (bits + 7) / 8;
	int CompressionMethod = buffer[0x1A];
	int FilterMethod = buffer[0x1B];
	int InterlaceMethod = buffer[0x1C];

	int index = 0x21;
	int chunkLength = 0;
	char* chunkKey = nullptr;
	char* chunkData = nullptr;

	unsigned char endKey[12] = { 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82 };
	std::vector<unsigned char> dataIDAT;

	while (1) {
		if (memcmp(buffer + index, endKey, 12) == 0) break;

		buf[0] = buffer[index + 3];
		buf[1] = buffer[index + 2];
		buf[2] = buffer[index + 1];
		buf[3] = buffer[index];
		chunkLength = *(int*)&(buf);
		chunkKey = (char*)buffer + index + 4;

		if (strncmp(chunkKey, "IDAT", 4) == 0) {
			size_t oldSize = dataIDAT.size();
			dataIDAT.resize(oldSize + chunkLength);
			memcpy(dataIDAT.data() + oldSize, chunkKey + 4, chunkLength);
		}
		index += (chunkLength + 4 * 3);
	}

	std::vector<unsigned char> dataDecode = decodeDeflate(dataIDAT.data(), (unsigned int)dataIDAT.size());

	// decode filter scanline
	unsigned char* recon = 0;
	unsigned char* scanline = 0;
	unsigned char* prevline = 0;
	unsigned int bytewidth = *bytes;
	unsigned int linebytes = ((*width) * bits + 7) / 8;

	size_t j = 0;
	size_t outindex;
	size_t inindex;
	unsigned char filterType;
	ret.resize(linebytes* *height);

	for (index = 0; index < *height; ++index) {

		outindex = linebytes * index;
		inindex = (1 + linebytes) * index;
		filterType = dataDecode[inindex];
		++inindex;
		recon = &ret[outindex];
		scanline = &dataDecode[inindex];

		switch (filterType) {

		case 0: // none
			for (j = 0; j != linebytes; ++j) recon[j] = scanline[j];
			break;
		case 1: //sub left X-A
			for (j = 0; j != bytewidth; ++j)		recon[j] = scanline[j];
			for (j = bytewidth; j < linebytes; ++j) recon[j] = scanline[j] + recon[j - bytewidth];
			break;
		case 2: // sub up X-B
			if (prevline)	for (j = 0; j != linebytes; ++j) recon[j] = scanline[j] + prevline[j];
			else			for (j = 0; j != linebytes; ++j) recon[j] = scanline[j];
			break;
		case 3: // average X-(A+B)/2
			if (prevline) {
				for (j = 0; j != bytewidth; ++j)		recon[j] = scanline[j] + (prevline[j] >> 1);
				for (j = bytewidth; j < linebytes; ++j) recon[j] = scanline[j] + ((recon[j - bytewidth] + prevline[j]) >> 1);
			}
			else {
				for (j = 0; j != bytewidth; ++j)		recon[j] = scanline[j];
				for (j = bytewidth; j < linebytes; ++j) recon[j] = scanline[j] + (recon[j - bytewidth] >> 1);
			}
			break;
		case 4: // peath
			if (prevline) {
				for (j = 0; j != bytewidth; ++j)		recon[j] = (scanline[j] + prevline[j]);
				for (j = bytewidth; j < linebytes; ++j) {

					unsigned char paethPredictor = 0;
					short int a = recon[j - bytewidth];
					short int b = prevline[j];
					short int c = prevline[j - bytewidth];
					short int pa = abs(b - c);
					short int pb = abs(a - c);
					short int pc = abs(a + b - c - c);

					if (pc < pa && pc < pb) paethPredictor = (unsigned char)c;
					else if (pb < pa)		paethPredictor = (unsigned char)b;
					else					paethPredictor = (unsigned char)a;

					recon[j] = (scanline[j] + paethPredictor);
				}
			}
			else {
				for (j = 0; j != bytewidth; ++j)		recon[j] = scanline[j];
				for (j = bytewidth; j < linebytes; ++j)	recon[j] = (scanline[j] + recon[j - bytewidth]);
			}
			break;
		}
		prevline = &ret[outindex];
	}
	return ret;
}

std::vector<unsigned char> QeEncode::decodeDeflate(unsigned char* in, unsigned int size) {
	// gzip, zlib, Deflate, LZ77, Canonical Huffman Code, inflate
	// 78 01 - fastest
	// 78 5E - fast 
	// 78 9C - default
	// 78 DA - Best Compression, slowest
	std::vector<unsigned char> out;
	if (in[0] != 0x78) return out;
	if (in[1] != 0x01 && in[1] != 0x5E && in[1] != 0x9C && in[1] != 0xDA) return out;

	in += 2;
	size_t bitPointer = 0;
	unsigned int BFINAL = 0;
	unsigned int BYTE = 0;

	while (!BFINAL) {
		BFINAL = readBits(in, &bitPointer, 1);
		BYTE = readBits(in, &bitPointer, 2);

		if (BYTE == 0) { // No compression

			while ((bitPointer & 0x7) != 0)	++bitPointer;
			size_t bytePos = bitPointer / 8;

			unsigned short int length = *(unsigned short int*)&(in[bytePos]);
			bytePos += 4;

			out.insert(out.end(), in[bytePos], in[bytePos] + length);
			bitPointer = bytePos * 8;
		}
		else if (BYTE == 1 || BYTE == 2) decodeHuffmanLZ77(&out, in, &bitPointer, BYTE);// fixed or dynamic Huffman

		else return out;
	}
	return out;
}

void QeEncode::decodeHuffmanLZ77(std::vector<unsigned char> *out, unsigned char* in, size_t* bitPointer, unsigned int BYTE) {

	// literal/length, distance for LZ77
	QeHuffmanTree treeLL, treeD;
	// decodeCCL
	if (BYTE == 1)		buildFixedLZ77HuffmanTree(&treeLL, &treeD);							// fixed Huffman 
	else if (BYTE == 2) buildDynamicLZ77HuffmanTree(&treeLL, &treeD, in, bitPointer);	// dynamic Huffman 

	return decodeLitLenDis(out, &treeLL, &treeD, in, bitPointer);
}

void QeEncode::buildFixedLZ77HuffmanTree(QeHuffmanTree* treeLL, QeHuffmanTree* treeD) {
	// 0-255=literals(ASCII), 256=end, 257-285=lengths, 286-287=unused
	unsigned int bitlenLL[288];
	unsigned int i = 0;
	for (i = 0; i < 144; ++i) bitlenLL[i] = 8;
	for (i = 144; i < 256; ++i) bitlenLL[i] = 9;
	for (i = 256; i < 280; ++i) bitlenLL[i] = 7;
	for (i = 280; i < 288; ++i) bitlenLL[i] = 8;
	buildHuffmanTree(treeLL, bitlenLL, 288, 15);

	// distance 32, 30-31 are unused
	unsigned int bitlenD[32];
	for (i = 0; i < 32; ++i) bitlenD[i] = 5;
	buildHuffmanTree(treeD, bitlenD, 288, 15);
}

void QeEncode::buildDynamicLZ77HuffmanTree(QeHuffmanTree* treeLL, QeHuffmanTree* treeD, const unsigned char* in, size_t* bitPointer) {

	unsigned int HLIT = readBits(in, bitPointer, 5) + 257;
	unsigned int HDIST = readBits(in, bitPointer, 5) + 1;
	unsigned int HCLEN = readBits(in, bitPointer, 4) + 4;

	unsigned int i = 0, j = 0;
	unsigned int bitlenCCL[19];
	memset(bitlenCCL, 0, 19 * sizeof(unsigned int));
	//for (i = 0; i < 19; ++i) bitlenCCL[i] = 0;

	for (i = 0; i < 19; ++i)	if (i < HCLEN) bitlenCCL[CCL_ORDER[i]] = readBits(in, bitPointer, 3);

	QeHuffmanTree treeCCL;
	buildHuffmanTree(&treeCCL, bitlenCCL, 19, 7);

	unsigned int bitlenLL[288];
	unsigned int bitlenD[32];
	memset(bitlenLL, 0, 288 * sizeof(unsigned int));
	memset(bitlenD, 0, 32 * sizeof(unsigned int));

	i = 0;
	while (i < HLIT + HDIST) {

		unsigned int code = huffmanDecodeSymbol(in, bitPointer, &treeCCL);
		if (code < 16) { // No duplicate or duplicate < 3
			if (i < HLIT) bitlenLL[i] = code;
			else bitlenD[i - HLIT] = code;
			++i;
		}
		else if (code == 16) {  // not 0, duplicate 3-6
			unsigned int replength = 3 + readBits(in, bitPointer, 2);
			unsigned int value;

			if (i < HLIT + 1) value = bitlenLL[i - 1];
			else value = bitlenD[i - HLIT - 1];

			for (j = 0; j < replength; ++j) {
				if (i < HLIT) bitlenLL[i] = value;
				else bitlenD[i - HLIT] = value;
				++i;
			}
		}
		else if (code == 17) {  // 0, duplicate 3-10, 3 bits
			unsigned int replength = 3 + readBits(in, bitPointer, 3);

			for (j = 0; j < replength; ++j) {
				if (i < HLIT) bitlenLL[i] = 0;
				else bitlenD[i - HLIT] = 0;
				++i;
			}
		}
		else if (code == 18) { // 0, duplicate 11-138, 7 bits
			unsigned int replength = 11 + readBits(in, bitPointer, 7);

			for (j = 0; j < replength; ++j) {
				if (i < HLIT) bitlenLL[i] = 0;
				else bitlenD[i - HLIT] = 0;
				++i;
			}
		}
	}
	buildHuffmanTree(treeLL, bitlenLL, 288, 15);
	buildHuffmanTree(treeD, bitlenD, 32, 15);
}

unsigned int QeEncode::huffmanDecodeSymbol(const unsigned char* in, size_t* bitPointer, const QeHuffmanTree* tree) {

	unsigned int treepos = 0, ret = 0;

	while (1) {
		ret = tree->tree[(treepos << 1) + readBits(in, bitPointer, 1)];
		if (ret < tree->numcodes) return ret;
		else treepos = ret - tree->numcodes;
	}
}

void QeEncode::buildHuffmanTree(QeHuffmanTree* tree, const unsigned int* bitlen, unsigned int numcodes, unsigned maxbitlen) {

	tree->numcodes = (unsigned)numcodes;

	unsigned int* tree1d = new unsigned int[numcodes];
	unsigned int *blcount = new unsigned int[maxbitlen + 1];
	unsigned int *nextcode = new unsigned int[maxbitlen + 1];

	memset(tree1d, 0, numcodes * sizeof(unsigned int));
	memset(blcount, 0, (maxbitlen + 1) * sizeof(unsigned int));
	memset(nextcode, 0, (maxbitlen + 1) * sizeof(unsigned int));

	unsigned int i = 0, j = 0;

	for (i = 0; i < numcodes; ++i)	++blcount[bitlen[i]];
	for (i = 1; i <=maxbitlen; ++i)	nextcode[i] = (nextcode[i - 1] + blcount[i - 1]) << 1;
	for (i = 0; i < numcodes; ++i)
		if (bitlen[i] != 0)	tree1d[i] = nextcode[bitlen[i]]++;

	unsigned int nodefilled = 0;
	unsigned int treepos = 0;

	tree->tree = new unsigned int[tree->numcodes * 2];
	for (i = 0; i < numcodes * 2; ++i)	tree->tree[i] = UINT_MAX;

	for (i = 0; i < numcodes; ++i) {
		for (j = 0; j < bitlen[i]; ++j) {

			unsigned char bit = (unsigned char)((tree1d[i] >> (bitlen[i] - j - 1)) & 1);

			if (tree->tree[2 * treepos + bit] == UINT_MAX) {
				if (j + 1 == bitlen[i]) {
					tree->tree[2 * treepos + bit] = i;
					treepos = 0;
				}
				else {
					++nodefilled;
					tree->tree[2 * treepos + bit] = nodefilled + tree->numcodes;
					treepos = nodefilled;
				}
			}
			else treepos = tree->tree[2 * treepos + bit] - tree->numcodes;
		}
	}

	for (i = 0; i < tree->numcodes * 2; ++i)
		if (tree->tree[i] == UINT_MAX)	tree->tree[i] = 0;

	if (tree1d != nullptr)	 delete[] tree1d;
	if (blcount != nullptr)	 delete[] blcount;
	if (nextcode != nullptr) delete[] nextcode;
}

void QeEncode::decodeLitLenDis(std::vector<unsigned char> *out, QeHuffmanTree* treeLL, QeHuffmanTree* treeD, unsigned char* in, size_t* bitPointer) {

	while (1) {
		unsigned int codeLL = huffmanDecodeSymbol(in, bitPointer, treeLL);
		if (codeLL < 256)	out->push_back(codeLL); // literals

		else if (codeLL > 256 && codeLL < 286) { // length
			unsigned int numextrabitsLen = LENGTHEXTRA[codeLL - 257];
			unsigned int length = LENGTHBASE[codeLL - 257] + readBits(in, bitPointer, numextrabitsLen);

			unsigned int codeD = huffmanDecodeSymbol(in, bitPointer, treeD); // 0-29
			unsigned int numextrabitsDis = DISTANCEEXTRA[codeD];
			unsigned int distance = DISTANCEBASE[codeD] + readBits(in, bitPointer, numextrabitsDis);

			while (1) {
				if (distance < length) {
					out->insert(out->end(), out->end() - distance, out->end());
					length -= distance;
				}
				else {
					out->insert(out->end(), out->end() - distance, out->end() - distance + length);
					break;
				}
			}
		}
		else break; // codeLL = 256 or the others
	}
}

std::vector<std::string> QeEncode::split(const char* s, const char* delim) {
	
	std::vector<std::string> tokens;
	char dup[256];
	strncpy_s(dup, s, 256);
	char *context = NULL;
	char * token = strtok_s(dup, delim, &context);
	while (token != NULL) {
		tokens.push_back(std::string(token));
		token = strtok_s(NULL, delim, &context);
	}
	return tokens;
}