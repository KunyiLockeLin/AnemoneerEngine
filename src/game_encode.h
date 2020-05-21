#pragma once
#include "header.h"

class QeGameEncode {
    SINGLETON_CLASS(QeGameEncode);

   public:
    ~QeGameEncode() {}

    // QeAssetModel* decodeOBJ(char* buffer);
    QeAssetModel *decodeGLTF(QeAssetJSON *json, bool bCubeMap = false);
    //QeAssetParticleRule *decodeParticle(AeXMLNode *node);
    // QeAssetModel* decodeGLB(char* buffer);
    // QeAssetMaterial* decodeMTL(char* buffer);
};
#define G_ENCODE QeGameEncode::getInstance()
