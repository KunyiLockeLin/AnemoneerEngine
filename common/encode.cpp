#include "common.h"

struct QeHuffmanTree {
    unsigned int *tree = nullptr;
    unsigned int numcodes = 0;

    ~QeHuffmanTree() {
        if (tree != nullptr) {
            delete tree;
            tree = nullptr;
        }
    }
};

struct QeHuffmanTree2 {
    unsigned int *codes = nullptr;
    unsigned int *codeBits = nullptr;
    unsigned int *values = nullptr;
    size_t size = 0;

    ~QeHuffmanTree2() {
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
};

const unsigned short int CCL_ORDER[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
const unsigned short int LENGTHBASE[29] = {3,  4,  5,  6,  7,  8,  9,  10, 11,  13,  15,  17,  19,  23, 27,
                                           31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
const unsigned short int LENGTHEXTRA[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
const unsigned short int DISTANCEBASE[30] = {1,    2,    3,    4,    5,    7,    9,    13,    17,    25,
                                             33,   49,   65,   97,   129,  193,  257,  385,   513,   769,
                                             1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
const unsigned short int DISTANCEEXTRA[30] = {0, 0, 0, 0, 1, 1, 2, 2,  3,  3,  4,  4,  5,  5,  6,
                                              6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
const unsigned char ZIGZAGTABLE[64] = {0,  1,  5,  6,  14, 15, 27, 28, 2,  4,  7,  13, 16, 26, 29, 42, 3,  8,  12, 17, 25, 30,
                                       41, 43, 9,  11, 18, 24, 31, 40, 44, 53, 10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38,
                                       46, 51, 55, 60, 21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63};

void xadd3(int *xa, int *xb, int *xc, int *xd, int h, int *p, int *n) {
    *p = *xa + *xb;
    *n = *xa - *xb;
    *xa = *p + *xc + h;
    *xb = *n + *xd + h;
    *xc = *p - *xc + h;
    *xd = *n - *xd + h;
}
void xmul(int *xa, int *xb, int k1, int k2, int sh, int *p, int *n) {
    *n = k1 * (*xa + *xb);
    *p = *xa;
    *xa = (*n + (k2 - k1) * *xb) >> sh;
    *xb = (*n - (k2 + k1) * *p) >> sh;
}

void buildHuffmanTree(QeHuffmanTree *tree, const unsigned int *bitlen, unsigned int numcodes, unsigned maxbitlen) {
    tree->numcodes = (unsigned)numcodes;

    unsigned int *tree1d = new unsigned int[numcodes];
    unsigned int *blcount = new unsigned int[maxbitlen + 1];
    unsigned int *nextcode = new unsigned int[maxbitlen + 1];

    memset(tree1d, 0, numcodes * sizeof(unsigned int));
    memset(blcount, 0, (maxbitlen + 1) * sizeof(unsigned int));
    memset(nextcode, 0, (maxbitlen + 1) * sizeof(unsigned int));

    unsigned int i = 0, j = 0;

    for (i = 0; i < numcodes; ++i) ++blcount[bitlen[i]];
    for (i = 1; i <= maxbitlen; ++i) nextcode[i] = (nextcode[i - 1] + blcount[i - 1]) << 1;
    for (i = 0; i < numcodes; ++i)
        if (bitlen[i] != 0) tree1d[i] = nextcode[bitlen[i]]++;

    unsigned int nodefilled = 0;
    unsigned int treepos = 0;

    tree->tree = new unsigned int[tree->numcodes * 2];
    for (i = 0; i < numcodes * 2; ++i) tree->tree[i] = UINT_MAX;

    for (i = 0; i < numcodes; ++i) {
        for (j = 0; j < bitlen[i]; ++j) {
            unsigned char bit = (unsigned char)((tree1d[i] >> (bitlen[i] - j - 1)) & 1);

            if (tree->tree[2 * treepos + bit] == UINT_MAX) {
                if (j + 1 == bitlen[i]) {
                    tree->tree[2 * treepos + bit] = i;
                    treepos = 0;
                } else {
                    ++nodefilled;
                    tree->tree[2 * treepos + bit] = nodefilled + tree->numcodes;
                    treepos = nodefilled;
                }
            } else
                treepos = tree->tree[2 * treepos + bit] - tree->numcodes;
        }
    }

    for (i = 0; i < tree->numcodes * 2; ++i)
        if (tree->tree[i] == UINT_MAX) tree->tree[i] = 0;

    if (tree1d != nullptr) delete[] tree1d;
    if (blcount != nullptr) delete[] blcount;
    if (nextcode != nullptr) delete[] nextcode;
}

void buildFixedLZ77HuffmanTree(QeHuffmanTree *treeLL, QeHuffmanTree *treeD) {
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

unsigned int huffmanDecodeSymbol(const unsigned char *in, size_t *bitPointer, const QeHuffmanTree *tree) {
    unsigned int treepos = 0, ret = 0;

    while (1) {
        ret = tree->tree[(treepos << 1) + ENCODE->readBits(in, bitPointer, 1)];
        if (ret < tree->numcodes)
            return ret;
        else
            treepos = ret - tree->numcodes;
    }
}

void buildDynamicLZ77HuffmanTree(QeHuffmanTree *treeLL, QeHuffmanTree *treeD, const unsigned char *in, size_t *bitPointer) {
    unsigned int HLIT = ENCODE->readBits(in, bitPointer, 5) + 257;
    unsigned int HDIST = ENCODE->readBits(in, bitPointer, 5) + 1;
    unsigned int HCLEN = ENCODE->readBits(in, bitPointer, 4) + 4;

    unsigned int i = 0, j = 0;
    unsigned int bitlenCCL[19];
    memset(bitlenCCL, 0, 19 * sizeof(unsigned int));
    // for (i = 0; i < 19; ++i) bitlenCCL[i] = 0;

    for (i = 0; i < 19; ++i)
        if (i < HCLEN) bitlenCCL[CCL_ORDER[i]] = ENCODE->readBits(in, bitPointer, 3);

    QeHuffmanTree treeCCL;
    buildHuffmanTree(&treeCCL, bitlenCCL, 19, 7);

    unsigned int bitlenLL[288];
    unsigned int bitlenD[32];
    memset(bitlenLL, 0, 288 * sizeof(unsigned int));
    memset(bitlenD, 0, 32 * sizeof(unsigned int));

    i = 0;
    while (i < HLIT + HDIST) {
        unsigned int code = huffmanDecodeSymbol(in, bitPointer, &treeCCL);
        if (code < 16) {  // No duplicate or duplicate < 3
            if (i < HLIT)
                bitlenLL[i] = code;
            else
                bitlenD[i - HLIT] = code;
            ++i;
        } else if (code == 16) {  // not 0, duplicate 3-6
            unsigned int replength = 3 + ENCODE->readBits(in, bitPointer, 2);
            unsigned int value;

            if (i < HLIT + 1)
                value = bitlenLL[i - 1];
            else
                value = bitlenD[i - HLIT - 1];

            for (j = 0; j < replength; ++j) {
                if (i < HLIT)
                    bitlenLL[i] = value;
                else
                    bitlenD[i - HLIT] = value;
                ++i;
            }
        } else if (code == 17) {  // 0, duplicate 3-10, 3 bits
            unsigned int replength = 3 + ENCODE->readBits(in, bitPointer, 3);

            for (j = 0; j < replength; ++j) {
                if (i < HLIT)
                    bitlenLL[i] = 0;
                else
                    bitlenD[i - HLIT] = 0;
                ++i;
            }
        } else if (code == 18) {  // 0, duplicate 11-138, 7 bits
            unsigned int replength = 11 + ENCODE->readBits(in, bitPointer, 7);

            for (j = 0; j < replength; ++j) {
                if (i < HLIT)
                    bitlenLL[i] = 0;
                else
                    bitlenD[i - HLIT] = 0;
                ++i;
            }
        }
    }
    buildHuffmanTree(treeLL, bitlenLL, 288, 15);
    buildHuffmanTree(treeD, bitlenD, 32, 15);
}

void decodeLitLenDis(std::vector<unsigned char> *out, QeHuffmanTree *treeLL, QeHuffmanTree *treeD, unsigned char *in,
                     size_t *bitPointer) {
    while (1) {
        unsigned int codeLL = huffmanDecodeSymbol(in, bitPointer, treeLL);
        if (codeLL < 256)
            out->push_back(codeLL);  // literals

        else if (codeLL > 256 && codeLL < 286) {  // length
            unsigned int numextrabitsLen = LENGTHEXTRA[codeLL - 257];
            unsigned int length = LENGTHBASE[codeLL - 257] + ENCODE->readBits(in, bitPointer, numextrabitsLen);

            unsigned int codeD = huffmanDecodeSymbol(in, bitPointer, treeD);  // 0-29
            unsigned int numextrabitsDis = DISTANCEEXTRA[codeD];
            unsigned int distance = DISTANCEBASE[codeD] + ENCODE->readBits(in, bitPointer, numextrabitsDis);

            while (1) {
                if (distance < length) {
                    out->insert(out->end(), out->end() - distance, out->end());
                    length -= distance;
                } else {
                    out->insert(out->end(), out->end() - distance, out->end() - distance + length);
                    break;
                }
            }
        } else
            break;  // codeLL = 256 or the others
    }
}

void decodeHuffmanLZ77(std::vector<unsigned char> *out, unsigned char *in, size_t *bitPointer, unsigned int BYTE) {
    // literal/length, distance for LZ77
    QeHuffmanTree treeLL, treeD;
    // decodeCCL
    if (BYTE == 1)
        buildFixedLZ77HuffmanTree(&treeLL, &treeD);  // fixed Huffman
    else if (BYTE == 2)
        buildDynamicLZ77HuffmanTree(&treeLL, &treeD, in,
                                    bitPointer);  // dynamic Huffman

    return decodeLitLenDis(out, &treeLL, &treeD, in, bitPointer);
}

unsigned int getHuffmanDecodeSymbol(const unsigned char *in, size_t *bitPointer, const QeHuffmanTree2 *tree) {
    unsigned char codeBits = 0;
    unsigned int key = 0;
    size_t oBitPointer = *bitPointer;

    for (int i = 0; i < tree->size; ++i) {
        if (tree->codeBits[i] != codeBits) {
            *bitPointer = oBitPointer;
            codeBits = tree->codeBits[i];
            key = ENCODE->readBits(in, bitPointer, codeBits, true);
        }
        if (key == tree->codes[i]) return tree->values[i];
    }
    *bitPointer = oBitPointer;
    return 0;
}

void getHuffmanDecodeSymbolfromDCAC(short int *out, unsigned char blocks, const unsigned char *in, size_t *bitPointer,
                                    const QeHuffmanTree2 *dc, const QeHuffmanTree2 *ac) {
    int value1 = 0, value2 = 0, value3 = 0;
    size_t index = 0, index1 = 0;

    int j = 0;
    for (int i = 0; i < blocks; ++i) {
        value1 = getHuffmanDecodeSymbol(in, bitPointer, dc);
        value2 = ENCODE->readBits(in, bitPointer, value1, true, true);
        out[index] = value2;
        ++index;

        while (index % 64 != 0) {
            value1 = getHuffmanDecodeSymbol(in, bitPointer, ac);
            if (value1 == 0) break;

            index1 = 0;
            value2 = ENCODE->readBits((unsigned char *)&value1, &index1, 4);
            value3 = ENCODE->readBits((unsigned char *)&value1, &index1, 4);
            index += value3;

            if (value2 > 0)
                out[index] = ENCODE->readBits(in, bitPointer, value2, true, true);
            else
                out[index] = 0;
            ++index;
        }
        index = (index + 63) / 64 * 64;
    }
}

void idct1(int *x, int *y, int ps, int half) {
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

void FastIntegerIDCT(int *i8x8) {
    int i, b2[64];
    for (i = 0; i < 8; i++) idct1(i8x8 + i * 8, b2 + i, 9, 1 << 8);
    for (i = 0; i < 8; i++) idct1(b2 + i * 8, i8x8 + i, 12, 1 << 11);
}

QeEncode::QeEncode() {}

int QeEncode::readBits(const unsigned char *stream, size_t *bitPointer, size_t readCount, bool bLeft, bool bNegative) {
    size_t bytes;
    unsigned char bits;
    size_t move;
    int ret = 0;
    size_t i;
    for (i = 0; i < readCount; ++i) {
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

    if (bNegative && readCount > 0) {
        if (ret == 0) {
            move = 1;
            move <<= readCount;
            ret = int(-1 * (move - 1));
        } else {
            i = 0;
            bytes = readBits((unsigned char *)&ret, &i, readCount - 1);
            bits = readBits((unsigned char *)&ret, &i, 1);

            if (bits == 0) {
                move = 1;
                move <<= readCount;
                ret = int(-1 * (move - bytes - 1));
            }
        }
    }
    return ret;
}

std::string QeEncode::trim(std::string s) {
    if (!s.length()) return s;
    s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
    s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
    return s;
}

QeAssetJSON *QeEncode::decodeJSON(const char *buffer, int &index) {
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
    QeAssetJSON *node = new QeAssetJSON();
    int lastIndex = index, currentIndex = index, lastKey = 0, currentKey;
    bool bValue = false;
    int count = 0;
    std::string key;
    while (1) {
        currentIndex = int(strcspn(buffer + lastIndex, keys) + lastIndex);
        currentKey = int(strchr(keys, buffer[currentIndex]) - keys);

        if (currentKey == 0 && lastKey != 3 && lastKey != 4) count++;

        if (lastKey == currentKey && currentKey == 2) {
            std::string s(buffer + lastIndex, currentIndex - lastIndex);
            s = trim(s);
            if (bValue) {
                bValue = false;

                node->data->eKeysforValues.push_back(key);
                node->data->eValues.push_back(s);
            } else
                key = s;
        } else if (lastKey == 3) {
            if (currentKey == 2)
                bValue = true;

            else if (currentKey == 0) {
                node->data->eKeysforNodes.push_back(key);
                node->data->eNodes.push_back(decodeJSON(buffer, currentIndex));
            } else if (currentKey != 4) {
                std::string s(buffer + lastIndex, currentIndex - lastIndex);
                s = trim(s);
                node->data->eKeysforValues.push_back(key);
                node->data->eValues.push_back(s);
            }
        } else if (lastKey == 4) {
            if (currentKey == 0) {
                std::vector<QeAssetJSON *> vjson;
                while (1) {
                    vjson.push_back(decodeJSON(buffer, currentIndex));
                    lastIndex = currentIndex + 1;
                    currentIndex = int(strcspn(buffer + lastIndex, keys) + lastIndex);
                    currentKey = int(strchr(keys, buffer[currentIndex]) - keys);

                    if (currentKey != 6) break;
                }
                node->data->eKeysforArrayNodes.push_back(key);
                node->data->eArrayNodes.push_back(vjson);
            } else {
                currentIndex = int(strchr(buffer + lastIndex, ']') - buffer);
                currentKey = 5;
                std::vector<std::string> vs;

                std::string s(buffer + lastIndex, currentIndex - lastIndex);
                char s2[512];
                strncpy_s(s2, s.c_str(), 512);
                char *context = NULL;
                const char *key1 = ",\"\r\n";
                char *pch = strtok_s(s2, key1, &context);

                while (pch != NULL) {
                    std::string s(pch);
                    s = trim(s);
                    if (s.length() > 0) vs.push_back(s);
                    pch = strtok_s(NULL, key1, &context);
                }
                node->data->eKeysforArrayValues.push_back(key);
                node->data->eArrayValues.push_back(vs);
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

AeXMLNode *QeEncode::decodeXML(const char *buffer, int &index, AeXMLNode *parent) {
    /* key
    0: <
    1: >
    2: /
    3: =
    */
    const char keys[] = "<>/=";
    int currentIndex = index, lastElemetIndex = 0;
    AeXMLNode *node = new AeXMLNode();
    node->data->parent = parent;

    while (true) {
        currentIndex = int(strcspn(buffer + currentIndex, keys) + currentIndex);
        switch (buffer[currentIndex]) {
            case '<': {
                if (buffer[currentIndex + 1] == '/') {
                    index = int(strchr(buffer + currentIndex, '>') - buffer);
                    return node;
                } else if (node->data->key.length()) {
                    node->data->nexts.push_back(decodeXML(buffer, currentIndex, node));
                    ++currentIndex;
                } else if (buffer[currentIndex + 1] == '?') {
                    currentIndex += 2;
                    int index = int(strchr(buffer + currentIndex, '?') - buffer);
                    int length = index - currentIndex;
                    std::string s(buffer + currentIndex, length);
                    s = trim(s);
                    node->data->version = s;
                    currentIndex = index + 3;
                } else if (strncmp(buffer + currentIndex + 1, "!--", 3) == 0) {
                    currentIndex += 4;
                    int index = int(strstr(buffer + currentIndex, "--") - buffer);
                    int length = index - currentIndex;
                    if (length > 0) {
                        std::string s(buffer + currentIndex, length);
                        s = trim(s);
                        node->data->comments.emplace_back(s);
                    }
                    currentIndex = index + 3;
                } else {
                    currentIndex += 1;
                    int index1 = int(strchr(buffer + currentIndex, '>') - buffer);
                    int index2 = int(strchr(buffer + currentIndex, ' ') - buffer);
                    int length = 0;
                    if (index1 < index2) {
                        length = index1 - currentIndex;
                        int index3 = int(strchr(buffer + currentIndex, '/') - buffer);
                        if (index3 < index1) --length;
                    } else {
                        length = index2 - currentIndex;
                    }
                    std::string s(buffer + currentIndex, length);
                    s = trim(s);
                    node->data->key = s;
                    currentIndex += length;
                    lastElemetIndex = currentIndex;
                }
            } break;
            case '>': {
                int index = int(strchr(buffer + currentIndex, '<') - buffer);
                int length = index - currentIndex - 1;
                if (length) {
                    std::string s(buffer + currentIndex + 1, length);
                    s = trim(s);
                    if (s.length()) node->data->value = s;
                }
                ++currentIndex;
            } break;
            case '/': {
                index = int(strchr(buffer + currentIndex, '>') - buffer);
                return node;
            } break;
            case '=': {
                AeNode node1;
                node1.key = std::string(buffer + lastElemetIndex, currentIndex - lastElemetIndex);
                node1.key = trim(node1.key);
                int index = int(strchr(buffer + currentIndex, '"') - buffer);
                int index1 = int(strchr(buffer + index + 1, '"') - buffer);
                node1.value = std::string(buffer + index + 1, index1 - index - 1);
                node1.value = trim(node1.value);
                node->data->elements.emplace_back(node1);
                currentIndex = index1 + 1;
                lastElemetIndex = currentIndex;
            } break;
            default:
                break;
        }
    }
    return nullptr;
}

std::vector<unsigned char> QeEncode::decodeJPEG(unsigned char *buffer, size_t size, int *width, int *height, int *bytes) {
    std::vector<unsigned char> ret;
    // YCbCr(YUV), DCT(Discrete Cosine Transform), Quantization, Zig-zag(Entropy
    // Coding), RLC(Run Length Coding), Canonical Huffman Code

    unsigned char startKey[2] = {0xFF, 0xD8};    // SOI  Start of Image
    unsigned char APP0[2] = {0xFF, 0xE0};        // APP0 Application
    unsigned char quanKey[2] = {0xFF, 0xDB};     // DQT  Define Quantization Table
    unsigned char frameKey[2] = {0xFF, 0xC0};    // SOF0 Start of Frame
    unsigned char huffmanKey[2] = {0xFF, 0xC4};  // DHT  Difine Huffman Table
    unsigned char scanKey[2] = {0xFF, 0xDA};     // SOS  Start of Scan
    unsigned char endKey[2] = {0xFF, 0xD9};      // EOI  End of Image

    if (memcmp(buffer, startKey, 2) != 0) return ret;

    unsigned short int length = 0;
    char *key;
    unsigned char *data = nullptr;
    size_t index = 2;
    unsigned char buf[2];
    std::vector<unsigned char> dataHuffman;
    QeHuffmanTree2 DC[2];  // DC00, DC01
    QeHuffmanTree2 AC[2];  // AC10, AC11
    unsigned char colorNum;
    unsigned char *mcusType = nullptr;
    unsigned char *mcusQuan = nullptr;
    unsigned char *mcusSize = nullptr;
    unsigned char *huffmanTreeIndex = nullptr;
    unsigned int mcuWidth = 0;
    unsigned int mcuHeight = 0;
    unsigned int totalmcuSize = 0;
    unsigned char *quanData[4] = {nullptr, nullptr, nullptr, nullptr};

    size_t i = 0, j = 0;
    while (1) {
        key = (char *)(buffer + index);
        // if (memcmp(key, endKey, 2) == 0)	break;

        buf[0] = *(buffer + index + 3);
        buf[1] = *(buffer + index + 2);
        length = *(unsigned short int *)(buf)-2;
        data = (buffer + index + 4);

        // else if (memcmp(key, APP0, 2) == 0) {}
        if (memcmp(key, quanKey, 2) == 0)
            quanData[data[0] % 16] = data + 1;
        else if (memcmp(key, frameKey, 2) == 0) {
            unsigned char colorBits = data[0];
            buf[1] = data[1];
            buf[0] = data[2];
            *width = *(short int *)&buf;
            buf[1] = data[3];
            buf[0] = data[4];
            *height = *(short int *)&buf;
            colorNum = data[5];

            char bits = 0;
            if (colorNum == 1)
                bits = colorBits;
            else
                bits = colorBits * 3;
            *bytes = (bits + 7) / 8;

            mcusType = new unsigned char[colorNum];
            mcusQuan = new unsigned char[colorNum];
            mcusSize = new unsigned char[colorNum];
            int maxSize = 0;
            for (i = 0; i < colorNum; ++i) {
                j = 0;
                mcusType[i] = data[6 + i * 3];
                int x = readBits(&data[7 + i * 3], &j, 4);
                int y = readBits(&data[7 + i * 3], &j, 4);
                mcusSize[i] = x * y;
                if (mcusSize[i] > maxSize) {
                    maxSize = mcusSize[i];
                    mcuWidth = ((*width) + 7) / 8 / x;
                    mcuHeight = ((*height) + 7) / 8 / y;
                }
                mcusQuan[i] = data[8 + i * 3];
            }
            totalmcuSize = mcuWidth * mcuHeight;
        } else if (memcmp(key, huffmanKey, 2) == 0) {
            QeHuffmanTree2 *tree = nullptr;

            if (data[0] / 16)
                tree = &AC[data[0] % 16];
            else
                tree = &DC[data[0] % 16];

            size_t length = 0;
            for (i = 0; i < 16; ++i) length += data[1 + i];

            tree->size = length;
            tree->codeBits = new unsigned int[length];
            tree->values = new unsigned int[length];
            tree->codes = new unsigned int[length];
            tree->codes[0] = 0;

            int itreeIndex = 0;
            int ntreeIndex = 1;
            while (data[1 + itreeIndex] == 0) ++itreeIndex;
            tree->codeBits[0] = itreeIndex + 1;
            i = 0;
            while (1) {
                tree->values[i] = data[17 + i];
                if ((i + 1) == length) break;

                while (data[1 + itreeIndex] <= ntreeIndex) {
                    ++itreeIndex;
                    ntreeIndex = 0;
                }
                tree->codes[i + 1] = tree->codes[i] + 1;
                tree->codeBits[i + 1] = itreeIndex + 1;

                if (ntreeIndex == 0) tree->codes[i + 1] <<= (tree->codeBits[i + 1] - tree->codeBits[i]);

                ++ntreeIndex;
                ++i;
            }
        } else if (memcmp(key, scanKey, 2) == 0) {
            huffmanTreeIndex = new unsigned char[data[0]];
            for (i = 0; i < data[0]; ++i) huffmanTreeIndex[i] = data[2 + i * 2];

            unsigned char *dataPos = data + length;
            size_t lengthData = size - (dataPos - buffer);
            size_t lengthData1 = 0;
            unsigned char *dataPos1 = nullptr;
            while (1) {
                dataPos1 = (unsigned char *)memchr(dataPos + lengthData1, 0xFF, lengthData);

                if (dataPos1[1] == 0) {
                    lengthData1 = dataPos1 - dataPos + 1;
                    dataHuffman.insert(dataHuffman.end(), dataPos, dataPos + lengthData1);
                    dataPos += (lengthData1 + 1);
                    lengthData -= (lengthData1 + 1);
                    lengthData1 = 0;
                } else {
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
    short int **mcuDatas = new short int *[colorNum];

    for (i = 0; i < colorNum; ++i) {
        mcuDatas[i] = new short int[64 * totalmcuSize * mcusSize[i]];
        memset(mcuDatas[i], 0, 64 * totalmcuSize * mcusSize[i] * sizeof(mcuDatas[0][0]));
    }

    size_t bitPointer = 0;

    for (i = 0; i < totalmcuSize; ++i) {  // decode Huffman
        for (j = 0; j < colorNum; ++j) {
            index = 0;
            unsigned char dc = readBits(&huffmanTreeIndex[j], &index, 4);
            unsigned char ac = readBits(&huffmanTreeIndex[j], &index, 4);
            getHuffmanDecodeSymbolfromDCAC(mcuDatas[j] + mcusSize[j] * i * 64, mcusSize[j], dataHuffman.data(), &bitPointer,
                                           &DC[dc], &AC[ac]);
        }
    }
    for (i = 0; i < colorNum; ++i) {  // DCn=DCn-1+Diff
        index = mcusSize[i] * totalmcuSize;
        for (j = 1; j < index; ++j) mcuDatas[i][j * 64] += mcuDatas[i][(j - 1) * 64];
    }
    int k = 0;
    for (i = 0; i < colorNum; ++i) {  // reverse Quantization
        index = mcusSize[i] * totalmcuSize;
        for (j = 0; j < index; ++j) {
            for (k = 0; k < 64; ++k) mcuDatas[i][j * 64 + k] *= quanData[mcusQuan[i]][k];
        }
    }
    short int buffer1[64];
    for (i = 0; i < colorNum; ++i) {  // reverse Zig-zag
        index = mcusSize[i] * totalmcuSize;
        for (j = 0; j < index; ++j) {
            memcpy(buffer1, &mcuDatas[i][j * 64], 64 * sizeof(mcuDatas[0][0]));
            for (k = 0; k < 64; ++k) mcuDatas[i][j * 64 + k] = buffer1[ZIGZAGTABLE[k]];
        }
    }
    for (i = 0; i < colorNum; ++i) {  // odd lines change signed
        index = mcusSize[i] * totalmcuSize;
        for (j = 0; j < index; ++j) {
            for (k = 0; k < 64; ++k)
                if (k / 8 % 2 == 1) mcuDatas[i][j * 64 + k] *= -1;
        }
    }
    int buffer2[64];
    for (i = 0; i < colorNum; ++i) {  // IDCT
        index = mcusSize[i] * totalmcuSize;
        for (j = 0; j < index; ++j) {
            for (k = 0; k < 64; ++k) buffer2[k] = mcuDatas[i][j * 64 + k];
            FastIntegerIDCT(buffer2);
            for (k = 0; k < 64; ++k) mcuDatas[i][j * 64 + k] = buffer2[k];
        }
    }
    // YCrCb to RGB
    double cY, cCb, cCr;
    ret.resize(*width * *height * *bytes);
    size_t x = 0;
    size_t y = 0;
    if (mcusSize[0] == 1) {
        for (i = 0; i < totalmcuSize; ++i) {
            for (j = 0; j < 64; ++j) {
                index = i * 64 + j;
                cY = mcuDatas[0][index];
                cCb = mcuDatas[1][index];
                cCr = mcuDatas[2][index];

                x = i % mcuWidth * 8 + j % 8;
                y = i / mcuWidth * 8 + 7 - (j / 8);
                index = y * *width + x;

                ret[index * 3] = unsigned char(MATH->clamp(int(cY + 1.402 * cCr + 128), 0, 255));                             // R
                ret[index * 3 + 1] = unsigned char(MATH->clamp(int(cY - 0.3441363 * cCb - 0.71413636 * cCr + 128), 0, 255));  // G
                ret[index * 3 + 2] = unsigned char(MATH->clamp(int(cY + 1.772 * cCb + 128), 0, 255));                         // B
            }
        }
    } else if (mcusSize[0] == 4) {
        for (i = 0; i < totalmcuSize; ++i) {
            for (j = 0; j < 4; ++j) {
                for (k = 0; k < 64; ++k) {
                    index = (i * 4 + j) * 64 + k;
                    cY = mcuDatas[0][index];
                    index = index / 4;
                    cCb = mcuDatas[1][index];
                    cCr = mcuDatas[2][index];

                    x = i % mcuWidth * 16 + j % 2 * 8 + k % 8;
                    y = i / mcuWidth * 16 + j / 2 * 8 + 7 - (k / 8);
                    index = y * *width + x;

                    ret[index * 3] = unsigned char(MATH->clamp(int(cY + 1.402 * cCr + 128), 0, 255));  // R
                    ret[index * 3 + 1] =
                        unsigned char(MATH->clamp(int(cY - 0.3441363 * cCb - 0.71413636 * cCr + 128), 0, 255));  // G
                    ret[index * 3 + 2] = unsigned char(MATH->clamp(int(cY + 1.772 * cCb + 128), 0, 255));        // B
                }
            }
        }
    }
    if (mcusType != nullptr) delete[] mcusType;
    if (mcusQuan != nullptr) delete[] mcusQuan;
    if (mcusSize != nullptr) delete[] mcusSize;
    if (huffmanTreeIndex != nullptr) delete[] huffmanTreeIndex;

    if (mcuDatas != nullptr) {
        for (i = 0; i < colorNum; ++i)
            if (mcuDatas[i] != nullptr) delete mcuDatas[i];
        delete[] mcuDatas;
    }
    return ret;
}

std::vector<unsigned char> QeEncode::decodeBMP(unsigned char *buffer, int *width, int *height, int *bytes) {
    std::vector<unsigned char> ret;
    if (strncmp((char *)buffer, "BM", 2) != 0) return ret;

    *width = *(int *)(buffer + 0x12);
    *height = *(int *)(buffer + 0x16);
    short int *bits = (short int *)(buffer + 0x1C);
    *bytes = (*bits + 7) / 8;
    int imageSize = *width * *height * *bytes;

    ret.resize(imageSize);
    memcpy(ret.data(), buffer + 0x0A, imageSize);

    return ret;
}

std::vector<unsigned char> QeEncode::decodePNG(unsigned char *buffer, int *width, int *height, int *bytes) {
    std::vector<unsigned char> ret;
    unsigned char headerKey[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    if (memcmp(buffer, headerKey, 8) != 0) return ret;

    // IHDR 0x08 - 0x20
    // PLTE
    // IDAT
    // IEND 0x00 0x00 0x00 0x00 0x49 0x45 0x4E 0x44 0xAE 0x42 0x60 0x82
    char buf[4];
    buf[0] = buffer[0x13];
    buf[1] = buffer[0x12];
    buf[2] = buffer[0x11];
    buf[3] = buffer[0x10];
    *width = *(int *)&buf;
    buf[0] = buffer[0x17];
    buf[1] = buffer[0x16];
    buf[2] = buffer[0x15];
    buf[3] = buffer[0x14];
    *height = *(int *)&buf;
    int bitDepth = buffer[0x18];
    int colorType = buffer[0x19];
    int bits = 0;

    switch (colorType) {
        case 0:
            bits = 1;
            break;  // grey
        case 2:
            bits = 3;
            break;  // RGB
        case 3:
            bits = 1;
            break;  // palette
        case 4:
            bits = 2;
            break;  // grey + alpha
        case 6:
            bits = 4;
            break;  // RGBA
    }
    bits *= bitDepth;

    *bytes = (bits + 7) / 8;
    int CompressionMethod = buffer[0x1A];
    int FilterMethod = buffer[0x1B];
    int InterlaceMethod = buffer[0x1C];

    int index = 0x21;
    int chunkLength = 0;
    char *chunkKey = nullptr;
    char *chunkData = nullptr;

    unsigned char endKey[12] = {0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
    std::vector<unsigned char> dataIDAT;

    while (1) {
        if (memcmp(buffer + index, endKey, 12) == 0) break;

        buf[0] = buffer[index + 3];
        buf[1] = buffer[index + 2];
        buf[2] = buffer[index + 1];
        buf[3] = buffer[index];
        chunkLength = *(int *)&(buf);
        chunkKey = (char *)buffer + index + 4;

        if (strncmp(chunkKey, "IDAT", 4) == 0) {
            size_t oldSize = dataIDAT.size();
            dataIDAT.resize(oldSize + chunkLength);
            memcpy(dataIDAT.data() + oldSize, chunkKey + 4, chunkLength);
        }
        index += (chunkLength + 4 * 3);
    }

    std::vector<unsigned char> dataDecode = decodeDeflate(dataIDAT.data(), (unsigned int)dataIDAT.size());

    // decode filter scanline
    unsigned char *recon = 0;
    unsigned char *scanline = 0;
    unsigned char *prevline = 0;
    unsigned int bytewidth = *bytes;
    unsigned int linebytes = ((*width) * bits + 7) / 8;

    size_t j = 0;
    size_t outindex;
    size_t inindex;
    unsigned char filterType;
    ret.resize(linebytes * *height);

    for (index = 0; index < *height; ++index) {
        outindex = linebytes * index;
        inindex = (1 + linebytes) * index;
        filterType = dataDecode[inindex];
        ++inindex;
        recon = &ret[outindex];
        scanline = &dataDecode[inindex];

        switch (filterType) {
            case 0:  // none
                for (j = 0; j != linebytes; ++j) recon[j] = scanline[j];
                break;
            case 1:  // sub left X-A
                for (j = 0; j != bytewidth; ++j) recon[j] = scanline[j];
                for (j = bytewidth; j < linebytes; ++j) recon[j] = scanline[j] + recon[j - bytewidth];
                break;
            case 2:  // sub up X-B
                if (prevline)
                    for (j = 0; j != linebytes; ++j) recon[j] = scanline[j] + prevline[j];
                else
                    for (j = 0; j != linebytes; ++j) recon[j] = scanline[j];
                break;
            case 3:  // average X-(A+B)/2
                if (prevline) {
                    for (j = 0; j != bytewidth; ++j) recon[j] = scanline[j] + (prevline[j] >> 1);
                    for (j = bytewidth; j < linebytes; ++j) recon[j] = scanline[j] + ((recon[j - bytewidth] + prevline[j]) >> 1);
                } else {
                    for (j = 0; j != bytewidth; ++j) recon[j] = scanline[j];
                    for (j = bytewidth; j < linebytes; ++j) recon[j] = scanline[j] + (recon[j - bytewidth] >> 1);
                }
                break;
            case 4:  // peath
                if (prevline) {
                    for (j = 0; j != bytewidth; ++j) recon[j] = (scanline[j] + prevline[j]);
                    for (j = bytewidth; j < linebytes; ++j) {
                        unsigned char paethPredictor = 0;
                        short int a = recon[j - bytewidth];
                        short int b = prevline[j];
                        short int c = prevline[j - bytewidth];
                        short int pa = abs(b - c);
                        short int pb = abs(a - c);
                        short int pc = abs(a + b - c - c);

                        if (pc < pa && pc < pb)
                            paethPredictor = (unsigned char)c;
                        else if (pb < pa)
                            paethPredictor = (unsigned char)b;
                        else
                            paethPredictor = (unsigned char)a;

                        recon[j] = (scanline[j] + paethPredictor);
                    }
                } else {
                    for (j = 0; j != bytewidth; ++j) recon[j] = scanline[j];
                    for (j = bytewidth; j < linebytes; ++j) recon[j] = (scanline[j] + recon[j - bytewidth]);
                }
                break;
        }
        prevline = &ret[outindex];
    }
    return ret;
}

std::vector<unsigned char> QeEncode::decodeDeflate(unsigned char *in, unsigned int size) {
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

        if (BYTE == 0) {  // No compression

            while ((bitPointer & 0x7) != 0) ++bitPointer;
            size_t bytePos = bitPointer / 8;

            unsigned short int length = *(unsigned short int *)&(in[bytePos]);
            bytePos += 4;

            out.insert(out.end(), in[bytePos], in[bytePos] + length);
            bitPointer = bytePos * 8;
        } else if (BYTE == 1 || BYTE == 2)
            decodeHuffmanLZ77(&out, in, &bitPointer,
                              BYTE);  // fixed or dynamic Huffman

        else
            return out;
    }
    return out;
}
