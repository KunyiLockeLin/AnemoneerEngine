#pragma once
#include "header.h"

class QeGameEncode {
    SINGLETON_CLASS(QeGameEncode);

    // QeAssetModel* decodeOBJ(char* buffer);
    QeAssetModel *decodeGLTF(AeJSONNode *json, bool bCubeMap = false);
    //QeAssetParticleRule *decodeParticle(AeXMLNode *node);
    // QeAssetModel* decodeGLB(char* buffer);
    // QeAssetMaterial* decodeMTL(char* buffer);
};
#define G_ENCODE QeGameEncode::getInstance()
