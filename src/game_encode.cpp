#include "game_encode.h"

SINGLETON_INSTANCE(QeGameEncode)
/*
QeAssetModel *QeEncode::decodeOBJ(char *buffer) {
    std::vector<QeVector3f> normalV;
    std::vector<QeVector2f> texCoordV;

    QeVector3f tempV3;
    QeVector2f tempV2;
    QeVector3i tempV3p, tempV3t, tempV3n;
    char mtlPath[256];
    QeAssetModel *model = new QeAssetModel();
    char *context;
    char *line = strtok_s(buffer, "\n", &context);

    while (line != nullptr) {
        if (strncmp(line, "mtllib ", 7) == 0)
            sscanf_s(line, "mtllib %s", mtlPath, (unsigned int)sizeof(mtlPath));

        else if (strncmp(line, "v ", 2) == 0) {
            sscanf_s(line, "v %f %f %f", &(tempV3.x), &(tempV3.y), &(tempV3.z));
            QeVertex vet;
            vet.pos = tempV3;
            vet.color = {1.0f, 1.0f, 1.0f};
            model->vertices.push_back(vet);
        } else if (strncmp(line, "vt ", 3) == 0) {
            sscanf_s(line, "vt %f %f", &(tempV2.x), &(tempV2.y));
            texCoordV.push_back(tempV2);
        } else if (strncmp(line, "vn ", 3) == 0) {
            sscanf_s(line, "vn %f %f %f", &(tempV3.x), &(tempV3.y), &(tempV3.z));
            normalV.push_back(tempV3);
        } else if (strncmp(line, "f ", 2) == 0) {
            if (strstr(line, "//"))
                sscanf_s(line, "f %d//%d %d//%d %d//%d", &(tempV3p.x), &(tempV3n.x), &(tempV3p.y), &(tempV3n.y), &(tempV3p.z),
                         &(tempV3n.z));
            else if (texCoordV.empty())
                sscanf_s(line, "f %d %d %d", &(tempV3p.x), &(tempV3p.y), &(tempV3p.z));
            else if (normalV.empty())
                sscanf_s(line, "f %d/%d %d/%d %d/%d", &(tempV3p.x), &(tempV3t.x), &(tempV3p.y), &(tempV3t.y), &(tempV3p.z),
                         &(tempV3t.z));
            else
                sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &(tempV3p.x), &(tempV3t.x), &(tempV3n.x), &(tempV3p.y), &(tempV3t.y),
                         &(tempV3n.y), &(tempV3p.z), &(tempV3t.z), &(tempV3n.z));

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

QeAssetMaterial *QeEncode::decodeMTL(char *buffer) {
    QeAssetMaterial *mtl = new QeAssetMaterial();
    mtl->type = eMaterialPhong;
    QeDataMaterialPhong mtl1;
    mtl1.ambient.w = 1;
    mtl1.diffuse.w = 1;
    mtl1.specular.w = 1;
    mtl1.emissive.w = 1;
    char diffuseMapPath[512];
    char *context;
    char *line = strtok_s(buffer, "\n", &context);

    while (line != nullptr) {
        if (strncmp(line, "map_Kd ", 7) == 0)
            sscanf_s(line, "map_Kd %s", diffuseMapPath, (unsigned int)sizeof(diffuseMapPath));
        else if (strncmp(line, "Ns ", 3) == 0)
            sscanf_s(line, "Ns %f", &mtl1.param.x);
        else if (strncmp(line, "Ka ", 3) == 0)
            sscanf_s(line, "Ka %f %f %f", &(mtl1.ambient.x), &(mtl1.ambient.y), &(mtl1.ambient.z));
        else if (strncmp(line, "Kd ", 3) == 0)
            sscanf_s(line, "Kd %f %f %f", &(mtl1.diffuse.x), &(mtl1.diffuse.y), &(mtl1.diffuse.z));
        else if (strncmp(line, "Ks ", 3) == 0)
            sscanf_s(line, "Ks %f %f %f", &(mtl1.specular.x), &(mtl1.specular.y), &(mtl1.specular.z));
        else if (strncmp(line, "Ke ", 3) == 0)
            sscanf_s(line, "Ke %f %f %f", &(mtl1.emissive.x), &(mtl1.emissive.y), &(mtl1.emissive.z));
        else if (strncmp(line, "Ni ", 3) == 0)
            sscanf_s(line, "Ni %f", &(mtl1.param.y));
        else if (strncmp(line, "d ", 2) == 0)
            sscanf_s(line, "d %f", &(mtl1.param.z));

        line = strtok_s(NULL, "\n", &context);
    }

    mtl->value.phong = mtl1;
    if (strlen(diffuseMapPath) != 0) mtl->image.pDiffuseMap = AST->getImage(diffuseMapPath);

    return mtl;
}
*/
QeGameEncode::QeGameEncode() {}

QeAssetModel *QeGameEncode::decodeGLTF(QeAssetJSON *json, bool bCubeMap) {
    QeAssetModel *model = new QeAssetModel();

    const char *binData = json->getJSONValue(2, "buffers", "uri");
    char *ret = strrchr((char *)binData, '.');
    std::vector<char> buf;
    if (strcmp(ret + 1, "bin") == 0) {
        std::string _filePath = G_AST.combinePath(binData, eAssetBin);
        buf = CM_MGR.loadFile(_filePath.c_str());
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
    std::vector<QeAssetJSON *> *jaccessors = json->getJSONArrayNodes(1, "accessors");
    std::vector<QeAssetJSON *> *jbufferViews = json->getJSONArrayNodes(1, "bufferViews");
    std::vector<std::string> *sv;
    size_t size, size1, size2;
    unsigned char index;
    unsigned int offset, count, length, componentType;
    size_t i, j, k;

    unsigned char bufferViews[256];
    memset(bufferViews, 0xFF, 256 * sizeof(char));
    const char *c = json->getJSONValue(3, "meshes", "primitives", "indices");
    if (c != nullptr) bufferViews[0] = atoi(c);

    c = json->getJSONValue(4, "meshes", "primitives", "attributes", "POSITION");
    if (c != nullptr) bufferViews[1] = atoi(c);

    c = json->getJSONValue(4, "meshes", "primitives", "attributes", "NORMAL");
    if (c != nullptr) bufferViews[2] = atoi(c);

    c = json->getJSONValue(4, "meshes", "primitives", "attributes", "TEXCOORD_0");
    if (c != nullptr) bufferViews[3] = atoi(c);

    c = json->getJSONValue(4, "meshes", "primitives", "attributes", "TANGENT");
    if (c != nullptr) bufferViews[4] = atoi(c);

    c = json->getJSONValue(4, "meshes", "primitives", "attributes", "JOINTS_0");
    if (c != nullptr) bufferViews[5] = atoi(c);

    c = json->getJSONValue(4, "meshes", "primitives", "attributes", "WEIGHTS_0");
    if (c != nullptr) bufferViews[6] = atoi(c);

    sv = json->getJSONArrayValues(2, "nodes", "scale");
    if (sv == nullptr || sv->empty())
        model->scale = {1.0f, 1.0f, 1.0f};
    else
        model->scale = {float(atof((*sv)[0].c_str())), float(atof((*sv)[1].c_str())), float(atof((*sv)[2].c_str()))};

    c = json->getJSONValue(2, "skins", "inverseBindMatrices");
    if (c != nullptr) {
        // skeletal animation
        bufferViews[7] = atoi(c);
        std::vector<std::string> *jboneID = json->getJSONArrayValues(2, "skins", "joints");
        size = jboneID->size();
        model->jointsAnimation.resize(size);

        std::vector<QeAssetJSON *> *jboneName = json->getJSONArrayNodes(1, "nodes");

        for (i = 0; i < size; ++i) {
            model->jointsAnimation[i].id = atoi((*jboneID)[i].c_str());
            model->jointsAnimation[i].name = (*jboneName)[model->jointsAnimation[i].id]->getJSONValue(1, "name");

            if (strncmp(model->jointsAnimation[i].name, BONE_ROOT_NAME, 13) == 0) model->rootJoint = &model->jointsAnimation[i];

            /*sv = AST->getJSONArrayValues((*jboneName)[model->jointsAnimation[i].id],
            1, "translation"); if (sv != nullptr) {
                    model->jointsAnimation[i].translation.x =
            float(atof((*sv)[0].c_str())); model->jointsAnimation[i].translation.y =
            float(atof((*sv)[1].c_str())); model->jointsAnimation[i].translation.z =
            float(atof((*sv)[2].c_str()));
            }
            sv = AST->getJSONArrayValues((*jboneName)[model->jointsAnimation[i].id],
            1, "rotation"); if (sv != nullptr) { model->jointsAnimation[i].rotation.x
            = float(atof((*sv)[0].c_str())); model->jointsAnimation[i].rotation.y =
            float(atof((*sv)[1].c_str())); model->jointsAnimation[i].rotation.z =
            float(atof((*sv)[2].c_str())); model->jointsAnimation[i].rotation.w =
            float(atof((*sv)[3].c_str()));
            }
            model->jointsAnimation[i].transform =
            MATH->transform(model->jointsAnimation[i].translation,
            model->jointsAnimation[i].rotation, QeVector3f(1,1,1)); sv =
            AST->getJSONArrayValues((*jboneName)[model->jointsAnimation[i].id], 1,
            "scale"); if (sv != nullptr) { model->jointsAnimation[i].scale.x =
            float(atof((*sv)[0].c_str())); model->jointsAnimation[i].scale.y =
            float(atof((*sv)[1].c_str())); model->jointsAnimation[i].scale.z =
            float(atof((*sv)[2].c_str()));
            }*/
        }
        // QeMatrix4x4f mat;
        // setChildrenJointTranform(model->rootJoint, mat);

        for (i = 0; i < size; ++i) {
            sv = (*jboneName)[model->jointsAnimation[i].id]->getJSONArrayValues(1, "children");
            if (sv != nullptr) {
                size1 = sv->size();
                model->jointsAnimation[i].children.resize(size1);
                for (j = 0; j < size1; ++j) {
                    index = atoi((*sv)[j].c_str());
                    size2 = model->jointsAnimation.size();
                    for (k = 0; k < size2; ++k) {
                        if (index == model->jointsAnimation[k].id)
                            model->jointsAnimation[i].children[j] = &model->jointsAnimation[k];
                    }
                }
            }
        }

        std::vector<QeAssetJSON *> *jchannels = json->getJSONArrayNodes(2, "animations", "channels");
        std::vector<QeAssetJSON *> *jsmaplers = json->getJSONArrayNodes(2, "animations", "samplers");
        size1 = jchannels->size();
        const char *path = nullptr;

        for (i = 0; i < size1; ++i) {
            j = atoi((*jchannels)[i]->getJSONValue(2, "target", "node"));

            for (k = 0; k < size; ++k) {
                if (model->jointsAnimation[k].id == j) {
                    path = (*jchannels)[i]->getJSONValue(2, "target", "path");
                    index = atoi((*jsmaplers)[i]->getJSONValue(1, "input"));
                    count = atoi((*jaccessors)[index]->getJSONValue(1, "count"));
                    offset = atoi((*jbufferViews)[index]->getJSONValue(1, "byteOffset"));
                    length = atoi((*jbufferViews)[index]->getJSONValue(1, "byteLength"));

                    if (strncmp(path, "translation", 11) == 0) {
                        model->jointsAnimation[k].translationInput.resize(count);
                        memcpy(model->jointsAnimation[k].translationInput.data(), binData + offset, length);
                    } else if (strncmp(path, "rotation", 8) == 0) {
                        model->jointsAnimation[k].rotationInput.resize(count);
                        memcpy(model->jointsAnimation[k].rotationInput.data(), binData + offset, length);
                    }
                    // else if (strncmp(path, "scale", 5) == 0) {
                    //	model->jointsAnimation[k].scaleInput.resize(count);
                    //	memcpy(model->jointsAnimation[k].scaleInput.data(), binData +
                    // offset, length);
                    //}
                    index = atoi((*jsmaplers)[i]->getJSONValue(1, "output"));
                    count = atoi((*jaccessors)[index]->getJSONValue(1, "count"));
                    offset = atoi((*jbufferViews)[index]->getJSONValue(1, "byteOffset"));
                    length = atoi((*jbufferViews)[index]->getJSONValue(1, "byteLength"));

                    if (strncmp(path, "translation", 11) == 0) {
                        model->jointsAnimation[k].translationOutput.resize(count);
                        memcpy(model->jointsAnimation[k].translationOutput.data(), binData + offset, length);
                    } else if (strncmp(path, "rotation", 8) == 0) {
                        model->jointsAnimation[k].rotationOutput.resize(count);
                        memcpy(model->jointsAnimation[k].rotationOutput.data(), binData + offset, length);
                    }
                    // else if (strncmp(path, "scale", 5) == 0) {
                    // model->jointsAnimation[k].scaleOutput.resize(count);
                    // memcpy(model->jointsAnimation[k].scaleOutput.data(), binData +
                    // offset, length);
                    //}
                    break;
                }
            }
        }

        // If an action doesn't move for EMPTY_FRAMES frames, it means next action.
        const unsigned char EMPTY_FRAMES = 5;
        unsigned char notMoveFrameCounts = 0;

        size = model->jointsAnimation[0].translationInput.size();
        // size1 = model->jointsAnimation[0].rotationInput.size();
        // if (size1 > size) size = size1;
        // size1 = model->jointsAnimation[0].scaleInput.size();
        // if (size1 > size) size = size1;

        size1 = model->jointsAnimation.size();
        model->animationStartFrames.push_back(0);

        for (i = 1; i < size; ++i) {
            if (notMoveFrameCounts == EMPTY_FRAMES) {
                notMoveFrameCounts = 0;
                model->animationEndFrames.push_back(unsigned int(i - EMPTY_FRAMES) - 1);
                model->animationStartFrames.push_back(unsigned int(i));
            }
            for (j = 0; j < size1; ++j) {
                if (/*(i< model->jointsAnimation[j].translationOutput.size()) &&*/ (
                    model->jointsAnimation[j].translationOutput[i] != model->jointsAnimation[j].translationOutput[i - 1])) {
                    notMoveFrameCounts = 0;
                    break;
                }
                if (/*(i< model->jointsAnimation[j].rotationOutput.size()) &&*/ (model->jointsAnimation[j].rotationOutput[i] !=
                                                                                 model->jointsAnimation[j].rotationOutput[i - 1])) {
                    notMoveFrameCounts = 0;
                    break;
                }
                // if ((i< model->jointsAnimation[j].scaleOutput.size()) &&
                // (model->jointsAnimation[j].scaleOutput[i] !=
                // model->jointsAnimation[j].scaleOutput[i-1]) ) {
                // notMoveFrameCounts =
                // 0; 	break;
                //}
            }
            if (j == size1) ++notMoveFrameCounts;
        }
        model->animationEndFrames.push_back(unsigned int(i) - 1);
        model->animationNum = unsigned char(model->animationEndFrames.size());
    }
    size = jbufferViews->size();

    for (i = 0; i < size; ++i) {
        index = atoi((*jaccessors)[i]->getJSONValue(1, "bufferView"));
        count = atoi((*jaccessors)[i]->getJSONValue(1, "count"));
        componentType = atoi((*jaccessors)[i]->getJSONValue(1, "componentType"));
        offset = atoi((*jbufferViews)[i]->getJSONValue(1, "byteOffset"));
        length = atoi((*jbufferViews)[i]->getJSONValue(1, "byteLength"));

        if (index == bufferViews[0]) {  // indices
            if (componentType == 5121) {
                unsigned char *dataPos = (unsigned char *)(binData + offset);
                model->indexSize = count;
                for (j = 0; j < count; ++j) model->indices.push_back(*(dataPos + j));
            } else if (componentType == 5123) {
                unsigned short int *dataPos = (unsigned short int *)(binData + offset);
                model->indexSize = count;
                for (j = 0; j < count; ++j) model->indices.push_back(*(dataPos + j));
            } else if (componentType == 5125) {
                unsigned int *dataPos = (unsigned int *)(binData + offset);
                model->indexSize = count;
                for (j = 0; j < count; ++j) model->indices.push_back(*(dataPos + j));
            }
        } else if (index == bufferViews[1]) {  // position
            AeArray<float, 3> *dataPos = (AeArray<float, 3> *)(binData + offset);
            if (model->vertices.size() < count) model->vertices.resize(count);
            for (j = 0; j < count; ++j) {
                model->vertices[j].pos = *(dataPos + j);
                model->vertices[j].color = {1.0f, 1.0f, 1.0f, 1.0f};
            }
        } else if (index == bufferViews[2]) {  // normal
            AeArray<float, 3> *dataPos = (AeArray<float, 3> *)(binData + offset);
            if (model->vertices.size() < count) model->vertices.resize(count);
            for (j = 0; j < count; ++j) model->vertices[j].normal = *(dataPos + j);
        } else if (index == bufferViews[3]) {  // uv
            AeArray<float, 2> *dataPos = (AeArray<float, 2> *)(binData + offset);
            if (model->vertices.size() < count) model->vertices.resize(count);
            for (j = 0; j < count; ++j) model->vertices[j].uv = *(dataPos + j);
        } else if (index == bufferViews[4]) {  // tangent
            AeArray<float, 4> *dataPos = (AeArray<float, 4> *)(binData + offset);
            if (model->vertices.size() < count) model->vertices.resize(count);
            for (j = 0; j < count; ++j) model->vertices[j].tangent = *(dataPos + j);
        } else if (index == bufferViews[5]) {  // joint
            AeArray<short int, 4> *dataPos = (AeArray<short int, 4> *)(binData + offset);
            if (model->vertices.size() < count) model->vertices.resize(count);
            for (j = 0; j < count; ++j) model->vertices[j].joint = *(dataPos + j);

            // for (j = 0; j < count; ++j) {
            //	model->vertices[j].joint[0] = (dataPos + j)->x;
            //	model->vertices[j].joint[1] = (dataPos + j)->y;
            //	model->vertices[j].joint[2] = (dataPos + j)->z;
            //	model->vertices[j].joint[3] = (dataPos + j)->w;
            //}
            // model->joints.resize(count);
            // for (j = 0; j < count; ++j) model->joints[j] = *(dataPos + j);
            // memcpy(model->joints.data(), dataPos, length);
        } else if (index == bufferViews[6]) {  // weight
            AeArray<float, 4> *dataPos = (AeArray<float, 4> *)(binData + offset);
            if (model->vertices.size() < count) model->vertices.resize(count);
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
                *(((float *)(&model->vertices[j].weight)) + lastWeight) += totalWeight;
            }
        } else if (index == bufferViews[7]) {  // inverseBindMatrices  bone matrices
            QeMatrix4x4f *dataPos = (QeMatrix4x4f *)(binData + offset);
            for (j = 0; j < count; ++j) model->jointsAnimation[j].inverseBindMatrix = *(dataPos + j);
        }
    }

    // material
    QeAssetMaterial *pMaterial = new QeAssetMaterial();
    // pMaterial->type = eMaterialPBR;
    model->pMaterial = pMaterial;
    c = json->getJSONValue(4, "materials", "pbrMetallicRoughness", "baseColorTexture", "index");
    if (c) {
        int textureIndex = atoi(c);
        std::vector<QeAssetJSON *> *imageJSON = json->getJSONArrayNodes(1, "images");
        const char *texturePath = (*imageJSON)[textureIndex]->getJSONValue(1, "uri");

        if (bCubeMap)
            pMaterial->image.pCubeMap = G_AST.getImage(texturePath, bCubeMap, true);
        else
            pMaterial->image.pBaseColorMap = G_AST.getImage(texturePath, bCubeMap, true);
    }

    c = json->getJSONValue(3, "materials", "normalTexture", "index");
    if (c) {
        int textureIndex = atoi(json->getJSONValue(3, "materials", "normalTexture", "index"));
        std::vector<QeAssetJSON *> *imageJSON = json->getJSONArrayNodes(1, "images");
        const char *texturePath = (*imageJSON)[textureIndex]->getJSONValue(1, "uri");

        pMaterial->image.pNormalMap = G_AST.getImage(texturePath, bCubeMap);
    }
    std::vector<std::string> *baseColorJ = json->getJSONArrayValues(3, "materials", "pbrMetallicRoughness", "baseColorFactor");
    // QeDataMaterialPBR mtl;
    QeDataMaterial mtl;

    if (baseColorJ != nullptr) {
        mtl.baseColor.x = float(atof((*baseColorJ)[0].c_str()));
        mtl.baseColor.y = float(atof((*baseColorJ)[1].c_str()));
        mtl.baseColor.z = float(atof((*baseColorJ)[2].c_str()));
        mtl.baseColor.w = float(atof((*baseColorJ)[3].c_str()));
    } else {
        mtl.baseColor.x = 1.0f;
        mtl.baseColor.y = 1.0f;
        mtl.baseColor.z = 1.0f;
        mtl.baseColor.w = 1.0f;
    }
    const char *value = json->getJSONValue(3, "materials", "pbrMetallicRoughness", "metallicFactor");
    if (value == nullptr)
        mtl.metallicRoughnessEmissive.x = 1.0f;
    else
        mtl.metallicRoughnessEmissive.x = float(atof(value));

    value = json->getJSONValue(3, "materials", "pbrMetallicRoughness", "roughnessFactor");
    if (value == nullptr)
        mtl.metallicRoughnessEmissive.y = 1.0f;
    else
        mtl.metallicRoughnessEmissive.y = float(atof(value));

    mtl.metallicRoughnessEmissive.z = 1.0f;
    pMaterial->value = mtl;

    return model;
}

// QeAssetModel* QeEncode::decodeGLB(char* buffer) { return nullptr; }
/*QeAssetParticleRule *QeGameEncode::decodeParticle(AeXMLNode *node) {
    QeAssetParticleRule *particle = new QeAssetParticleRule();

    // const char* c = AST->getXMLValue(node, 1, "image");
    // if (c != nullptr) particle->image = AST->getImage(c);
    particle->image = node->getXMLValue("image");
    particle->bAlpha = node->getXMLValueb("alpha");
    particle->bReborn = node->getXMLValueb("reborn");
    particle->count_once = node->getXMLValuei("count.once");
    particle->count_period = node->getXMLValuei("count.period");
    particle->count_total = node->getXMLValuei("count.total");
    particle->count_range = node->getXMLValuei("count.range");
    particle->life_scend = node->getXMLValuei("life.second");
    particle->life_range = node->getXMLValuei("life.range");
    particle->init_pos_square.x = node->getXMLValuef("init_pos.length");
    particle->init_pos_square_range.x = node->getXMLValuef("init_pos.length_range");
    particle->init_pos_square.y = node->getXMLValuef("init_pos.width");
    particle->init_pos_square_range.y = node->getXMLValuef("init_pos.width_range");
    particle->init_pos_square.z = node->getXMLValuef("init_pos.height");
    particle->init_pos_square_range.z = node->getXMLValuef("init_pos.height_range");
    particle->init_pos_cycle.x = node->getXMLValuef("init_pos.radius");
    particle->init_pos_cycle_range.x = node->getXMLValuef("init_pos.radius_range");
    particle->init_pos_cycle.y = node->getXMLValuef("init_pos.degree");
    particle->init_pos_cycle_range.y = node->getXMLValuef("init_pos.degree_range");
    particle->init_speed = node->getXMLValuefXYZ("init_speed.");
    particle->init_speed_range = node->getXMLValuefXYZ("init_speed.range");
    particle->force = node->getXMLValuefXYZ("force.");
    particle->force_range = node->getXMLValuefXYZ("force.range");
    particle->color = node->getXMLValueRGB("color");
    particle->color_range = node->getXMLValueRGB("color.range");
    particle->size = node->getXMLValuefXY("size.");
    particle->size_range = node->getXMLValuefXY("size.range");

    return particle;
}*/
