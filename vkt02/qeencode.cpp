#include "qeheader.h"

unsigned int QeEncode::readBits(const unsigned char* stream, unsigned int *bitPointer, unsigned int readCount){

	unsigned int ret=0;
	for (unsigned int i = 0; i < readCount; ++i) 
		ret += (((*(stream + (*bitPointer >> 3)) >> ((*bitPointer)++ & 0x7)) & 1) << i);
	return ret;
}


std::vector<unsigned char> QeEncode::decodePNG(unsigned char* buffer, unsigned int size, int* width, int* height, int* bytes) {

	unsigned char headerKey[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

	std::vector<unsigned char> ret;
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
	unsigned int bitPointer = 0;
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

void QeEncode::decodeHuffmanLZ77(std::vector<unsigned char> *out, unsigned char* in, unsigned int* bitPointer, unsigned int BYTE) {

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

void QeEncode::buildDynamicHuffmanTree(QeHuffmanTree* treeLL, QeHuffmanTree* treeD, const unsigned char* in, unsigned int* bitPointer){

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

unsigned int QeEncode::huffmanDecodeSymbol(const unsigned char* in, unsigned int* bitPointer, const QeHuffmanTree* tree){

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

void QeEncode::decodeLitLenDis(std::vector<unsigned char> *out, QeHuffmanTree* treeLL, QeHuffmanTree* treeD, unsigned char* in, unsigned int* bitPointer) {
	
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