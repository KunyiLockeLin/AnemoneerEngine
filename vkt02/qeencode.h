#pragma once
#include "qeheader.h"

struct QeHuffmanTree
{
	unsigned int* tree2d;
	unsigned int* tree1d;
	unsigned int* lengths;
	unsigned int maxbitlen;
	unsigned int numcodes;

	~QeHuffmanTree() {
		if (tree2d != nullptr) delete tree2d;
		if (tree1d != nullptr) delete tree1d;
		if (lengths != nullptr) delete lengths;
	}
};

class QeEncode
{
public:

	const unsigned short int CCL_ORDER[19]    = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
	const unsigned short int LENGTHBASE[29]   = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258 };
	const unsigned short int LENGTHEXTRA[29]  = { 0, 0, 0, 0, 0, 0, 0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,   4,   5,   5,   5,   5,   0 };
	const unsigned short int DISTANCEBASE[30] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577 };
	const unsigned short int DISTANCEEXTRA[30]= { 0, 0, 0, 0, 1, 1, 2,  2,  3,  3,  4,  4,  5,  5,   6,   6,   7,   7,   8,   8,    9,    9,   10,   10,   11,   11,   12,    12,    13,    13 };
	
	QeEncode(QeGlobalKey& _key) {}
	~QeEncode() {}
	unsigned int readBits(const unsigned char* stream, unsigned int *bitPointer, unsigned int readCount );
	std::vector<unsigned char> decodePNG(unsigned char* buffer, unsigned int size, int* width, int* height);
	std::vector<unsigned char> decodeDeflate(unsigned char* in, unsigned int size);
	void decodeHuffmanLZ77(std::vector<unsigned char> *out, unsigned char* in, unsigned int* bitPointer, unsigned int BYTE);
	void buildFixedHuffmanTree(QeHuffmanTree* treeLL, QeHuffmanTree* treeD);
	void buildDynamicHuffmanTree(QeHuffmanTree* treeLL, QeHuffmanTree* treeD, const unsigned char* in, unsigned int* bitPointer);
	unsigned int huffmanDecodeSymbol(const unsigned char* in, unsigned int* bitPointer, const QeHuffmanTree* tree);
	void buildHuffmanTree(QeHuffmanTree* tree, const unsigned int* bitlen, unsigned int numcodes, unsigned maxbitlen);
	void decodeLitLenDis(std::vector<unsigned char> *out, QeHuffmanTree* treeLL, QeHuffmanTree* treeD, unsigned char* in, unsigned int* bitPointer);
};

