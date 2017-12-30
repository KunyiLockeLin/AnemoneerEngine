#include "qeheader.h"

unsigned int QeEncode::readBits(const unsigned char* stream, size_t *bitPointer, unsigned int readCount){

	unsigned int ret=0;
	for (unsigned int i = 0; i < readCount; ++i) 
		ret += (((*(stream + (*bitPointer >> 3)) >> ((*bitPointer)++ & 0x7)) & 1) << i);
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
	int lastIndex = index, currentIndex = index, lastKey = 0, currentKey = 0;
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
				const char* key1 = ",\"\n";
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

QeAssetModel* QeEncode::decodeOBJ(char* buffer) {

	std::vector<QeVector3f> normalV;
	std::vector<QeVector2f> texCoordV;

	QeVector3f tempV3;
	QeVector2f tempV2;
	QeVector3i tempV3p, tempV3t, tempV3n;
	char mtlPath[256];
	QeAssetModel* model = new QeAssetModel();
	char *context;
	char* line = strtok_s(buffer, "\n", &context);

	while ( line != nullptr ) {

		if (strncmp(line, "mtllib ", 7) == 0)
			sscanf_s(line, "mtllib %s", mtlPath, (unsigned int)sizeof(mtlPath));

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
			if (strstr(line, "//")) {
				sscanf_s(line, "f %d//%d %d//%d %d//%d", &(tempV3p.x), &(tempV3n.x),
					&(tempV3p.y), &(tempV3n.y), &(tempV3p.z), &(tempV3n.z));
			}
			else if (texCoordV.empty()) {
				sscanf_s(line, "f %d %d %d", &(tempV3p.x), &(tempV3p.y), &(tempV3p.z));
			}
			else if (normalV.empty()) {
				sscanf_s(line, "f %d/%d %d/%d %d/%d", &(tempV3p.x), &(tempV3t.x),
					&(tempV3p.y), &(tempV3t.y), &(tempV3p.z), &(tempV3t.z));
			}
			else {
				sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &(tempV3p.x), &(tempV3t.x),
					&(tempV3n.x), &(tempV3p.y), &(tempV3t.y), &(tempV3n.y), &(tempV3p.z), &(tempV3t.z), &(tempV3n.z));
			}

			tempV3p -= 1;

			if (!texCoordV.empty()) {
				tempV3t -= 1;
				model->vertices[tempV3p.x].texCoord = texCoordV[tempV3t.x];
				model->vertices[tempV3p.y].texCoord = texCoordV[tempV3t.y];
				model->vertices[tempV3p.z].texCoord = texCoordV[tempV3t.z];
			}
			if (!normalV.empty()) {
				tempV3n -= 1;
				model->vertices[tempV3p.x].normal = normalV[tempV3n.x];
				model->vertices[tempV3p.y].normal = normalV[tempV3n.y];
				model->vertices[tempV3p.z].normal = normalV[tempV3n.z];
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

QeAssetModel* QeEncode::decodeGLTF(char* buffer) { return nullptr; }
QeAssetModel* QeEncode::decodeGLB(char* buffer) { return nullptr; }

QeAssetMaterial* QeEncode::decodeMTL(char* buffer) {

	QeAssetMaterial* mtl = new QeAssetMaterial();
	QeDataMaterial mtl1;
	mtl1.ambient.w = 1;
	mtl1.diffuse.w = 1;
	mtl1.specular.w = 1;
	mtl1.emissive.w = 1;
	char diffuseMapPath[512];
	char sv[512] = "";
	char sg[512] = "";
	char sf[512] = "";
	char *context;
	char* line = strtok_s(buffer, "\n", &context);

	while (line != nullptr) {

		if (strncmp(line, "map_Kd ", 7) == 0)
			sscanf_s(line, "map_Kd %s", diffuseMapPath, (unsigned int) sizeof(diffuseMapPath));

		else if (strncmp(line, "Ns ", 3) == 0)	sscanf_s(line, "Ns %f", &mtl1.param.x);

		else if (strncmp(line, "Ka ", 3) == 0)
			sscanf_s(line, "Ka %f %f %f", &(mtl1.ambient.x), &(mtl1.ambient.y), &(mtl1.ambient.z));

		else if (strncmp(line, "Kd ", 3) == 0)
			sscanf_s(line, "Kd %f %f %f", &(mtl1.diffuse.x), &(mtl1.diffuse.y), &(mtl1.diffuse.z));

		else if (strncmp(line, "Ks ", 3) == 0)
			sscanf_s(line, "Ks %f %f %f", &(mtl1.specular.x), &(mtl1.specular.y), &(mtl1.specular.z));

		else if (strncmp(line, "Ke ", 3) == 0)
			sscanf_s(line, "Ke %f %f %f", &(mtl1.emissive.x), &(mtl1.emissive.y), &(mtl1.emissive.z));

		else if (strncmp(line, "Ni ", 3) == 0)	sscanf_s(line, "Ni %f", &(mtl1.param.y));

		else if (strncmp(line, "d ", 2) == 0)	sscanf_s(line, "d %f", &(mtl1.param.z));

		else if (strncmp(line, "sv ", 3) == 0)	sscanf_s(line, "sv %s", sv, (unsigned int) sizeof(sv));

		else if (strncmp(line, "sg ", 3) == 0)	sscanf_s(line, "sg %s", sg, (unsigned int) sizeof(sg));

		else if (strncmp(line, "sf ", 3) == 0)	sscanf_s(line, "sf %s", sf, (unsigned int) sizeof(sf));

		line = strtok_s(NULL, "\n", &context);
	}

	mtl->value = mtl1;
	
	if (strlen(diffuseMapPath) != 0)	mtl->pDiffuseMap = AST->getImage(diffuseMapPath);

	if (strlen(sv) == 0)	mtl->pShaderVert = AST->getShader(AST->getXMLValue(3, AST->CONFIG, "defaultShader", "vert"));
	else					mtl->pShaderVert = AST->getShader(sv);

	if (strlen(sg) == 0)	mtl->pShaderGeom = AST->getShader(AST->getXMLValue(3, AST->CONFIG, "defaultShader", "geom"));
	else					mtl->pShaderGeom = AST->getShader(sg);

	if (strlen(sf) == 0)	mtl->pShaderFrag = AST->getShader(AST->getXMLValue(3, AST->CONFIG, "defaultShader", "frag"));
	else					mtl->pShaderFrag = AST->getShader(sf);

	return mtl;
}

std::vector<unsigned char> QeEncode::decodeJPEG(unsigned char* buffer, int* width, int* height, int* bytes) {
	std::vector<unsigned char> ret;
	return ret;
}

std::vector<unsigned char> QeEncode::decodeBMP(unsigned char* buffer, int* width, int* height, int* bytes) {
	
	std::vector<unsigned char> ret;
	if (strncmp((char*)buffer, "BM", 2) != 0) return ret;


	*width = *(int*)&(buffer[0x12]);
	*height = *(int*)&(buffer[0x16]);
	short int bits = *(short int*)&(buffer[0x1C]);
	*bytes = (bits + 7) / 8;
	int	imageSize = *width * *height * *bytes;

	ret.resize(imageSize);
	memcpy(ret.data(), buffer+ 0x0A, imageSize);
	
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
	char num[4];
	num[0] = buffer[0x13];
	num[1] = buffer[0x12];
	num[2] = buffer[0x11];
	num[3] = buffer[0x10];
	*width = *(int*)&(num);
	num[0] = buffer[0x17];
	num[1] = buffer[0x16];
	num[2] = buffer[0x15];
	num[3] = buffer[0x14];
	*height = *(int*)&(num);
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

		num[0] = buffer[index + 3];
		num[1] = buffer[index + 2];
		num[2] = buffer[index + 1];
		num[3] = buffer[index];
		chunkLength = *(int*)&(num);
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

		switch (filterType){

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
			
			} else {
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
			} else {
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
	// gzip zlib Deflate LZ77 Huffman inflate
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

			while ( (bitPointer & 0x7) != 0 )	++bitPointer;
			size_t bytePos = bitPointer / 8;

			unsigned short int length = *(unsigned short int*)&(in[bytePos]);
			bytePos += 4;

			out.insert( out.end(), in[bytePos], in[bytePos]+ length);
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
	if (BYTE == 1)		buildFixedHuffmanTree(&treeLL, &treeD);							// fixed Huffman 
	else if (BYTE == 2) buildDynamicHuffmanTree(&treeLL, &treeD, in, bitPointer);	// dynamic Huffman 

	return decodeLitLenDis(out, &treeLL, &treeD, in, bitPointer);
}

void QeEncode::buildFixedHuffmanTree(QeHuffmanTree* treeLL, QeHuffmanTree* treeD) {
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

void QeEncode::buildDynamicHuffmanTree(QeHuffmanTree* treeLL, QeHuffmanTree* treeD, const unsigned char* in, size_t* bitPointer){

	unsigned int HLIT = readBits(in, bitPointer, 5) + 257;
	unsigned int HDIST = readBits( in, bitPointer, 5) + 1;
	unsigned int HCLEN = readBits(in, bitPointer, 4) + 4;

	unsigned int i = 0, j = 0;
	unsigned int bitlenCCL[19];
	memset(bitlenCCL, 0, 19 * sizeof(unsigned int));
	//for (i = 0; i < 19; ++i) bitlenCCL[i] = 0;

	for (i = 0; i < 19; ++i)	if (i < HCLEN) bitlenCCL[CCL_ORDER[i]] = readBits( in, bitPointer, 3);
	
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
		} else if (code == 16) {  // not 0, duplicate 3-6
			unsigned int replength = 3 + readBits(in, bitPointer, 2);
			unsigned int value;

			if (i < HLIT + 1) value = bitlenLL[i - 1];
			else value = bitlenD[i - HLIT - 1];
				
			for (j = 0; j < replength; ++j){
				if (i < HLIT) bitlenLL[i] = value;
				else bitlenD[i - HLIT] = value;
				++i;
			}
		} else if (code == 17) {  // 0, duplicate 3-10, 3 bits
			unsigned int replength = 3 + readBits( in, bitPointer, 3 );
				
			for (j = 0; j < replength; ++j){
				if (i < HLIT) bitlenLL[i] = 0;
				else bitlenD[i - HLIT] = 0;
				++i;
			}
		} else if (code == 18) { // 0, duplicate 11-138, 7 bits
			unsigned int replength = 11 + readBits(in, bitPointer, 7);
				
			for (j = 0; j < replength; ++j)	{
				if (i < HLIT) bitlenLL[i] = 0;
				else bitlenD[i - HLIT] = 0;
				++i;
			}
		}
	}
	buildHuffmanTree(treeLL, bitlenLL, 288, 15);
	buildHuffmanTree(treeD, bitlenD, 32, 15);
}

unsigned int QeEncode::huffmanDecodeSymbol(const unsigned char* in, size_t* bitPointer, const QeHuffmanTree* tree){

	unsigned int treepos = 0, ret = 0;

	while(1) {
		ret = tree->tree2d[(treepos << 1) + readBits(in, bitPointer,1)];
		if (ret < tree->numcodes) return ret;
		else treepos = ret - tree->numcodes;
	}
}

void QeEncode::buildHuffmanTree(QeHuffmanTree* tree, const unsigned int* bitlen, unsigned int numcodes, unsigned maxbitlen){
	
	tree->numcodes = (unsigned)numcodes;
	tree->maxbitlen = maxbitlen;

	tree->lengths = new unsigned int[tree->numcodes];
	tree->tree1d = new unsigned int[tree->numcodes];
	unsigned int *blcount = new unsigned int[maxbitlen + 1];
	unsigned int *nextcode = new unsigned int[tree->maxbitlen + 1];

	memcpy(tree->lengths, bitlen, tree->numcodes*sizeof(unsigned int));
	memset(tree->tree1d, 0, tree->numcodes * sizeof(unsigned int));
	memset(blcount, 0, (tree->maxbitlen+1) * sizeof(unsigned int));
	memset(nextcode, 0, (tree->maxbitlen + 1) * sizeof(unsigned int));

	unsigned int i = 0, j = 0;

	for (i = 0; i < tree->numcodes; ++i)	++blcount[tree->lengths[i]];	
	for (i = 1; i <= tree->maxbitlen; ++i)	nextcode[i] = (nextcode[i - 1] + blcount[i - 1]) << 1;
	for (i = 0; i < tree->numcodes; ++i)
		if (tree->lengths[i] != 0)	tree->tree1d[i] = nextcode[tree->lengths[i]]++;
	
	unsigned int nodefilled = 0;
	unsigned int treepos = 0;

	tree->tree2d = new unsigned int[tree->numcodes*2];
	for (i = 0; i < tree->numcodes*2; ++i)	tree->tree2d[i] = UINT_MAX; 

	for (i = 0; i < tree->numcodes; ++i) {
		for (j = 0; j < tree->lengths[i]; ++j) {

			unsigned char bit = (unsigned char)((tree->tree1d[i] >> (tree->lengths[i] - j - 1)) & 1);

			if (tree->tree2d[2 * treepos + bit] == UINT_MAX) {
				if (j + 1 == tree->lengths[i]) {
					tree->tree2d[2 * treepos + bit] = i;
					treepos = 0;
				} else {
					++nodefilled;
					tree->tree2d[2 * treepos + bit] = nodefilled + tree->numcodes;
					treepos = nodefilled;
				}
			}
			else treepos = tree->tree2d[2 * treepos + bit] - tree->numcodes;
		}
	}

	for (i = 0; i < tree->numcodes * 2; ++i)	
		if (tree->tree2d[i] == UINT_MAX)	tree->tree2d[i] = 0;

	if (blcount != nullptr)	blcount;
	if (nextcode != nullptr) nextcode;
}

void QeEncode::decodeLitLenDis(std::vector<unsigned char> *out, QeHuffmanTree* treeLL, QeHuffmanTree* treeD, unsigned char* in, size_t* bitPointer) {
	
	while (1)
	{
		unsigned int codeLL = huffmanDecodeSymbol(in, bitPointer, treeLL);
		if (codeLL < 256)	out->push_back( codeLL ); // literals

		else if (codeLL > 256 && codeLL < 286) { // length
			unsigned int numextrabitsLen = LENGTHEXTRA[codeLL - 257];
			unsigned int length = LENGTHBASE[codeLL - 257] + readBits(in, bitPointer, numextrabitsLen);

			unsigned int codeD = huffmanDecodeSymbol(in, bitPointer, treeD); // 0-29
			unsigned int numextrabitsDis = DISTANCEEXTRA[codeD];
			unsigned int distance = DISTANCEBASE[codeD] + readBits(in, bitPointer, numextrabitsDis);
			
			while (1) {
				if (distance < length) {
					out->insert(out->end(), out->end() - distance, out->end() );
					length -= distance;
				} else {
					out->insert(out->end(), out->end() - distance, out->end() - distance + length);
					break;
				}
			}
		}
		else break; // codeLL = 256 or the others
	}
}