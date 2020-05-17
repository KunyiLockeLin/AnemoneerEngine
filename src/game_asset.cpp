#include "header.h"

QeAssetModel::~QeAssetModel() { pMaterial = nullptr; }

QeGameAsset::QeGameAsset() {}

QeGameAsset::~QeGameAsset() {
    std::map<std::string, QeAssetModel *>::iterator it2 = astModels.begin();
    while (it2 != astModels.end()) {
        if ((it2->second) != nullptr) delete (it2->second);
        ++it2;
    }
    astModels.clear();

    std::map<std::string, QeAssetMaterial *>::iterator it3 = astMaterials.begin();
    while (it3 != astMaterials.end()) {
        if ((it3->second) != nullptr) delete (it3->second);
        ++it3;
    }
    astMaterials.clear();

    std::map<std::string, VkShaderModule>::iterator it4 = astShaders.begin();
    while (it4 != astShaders.end()) {
        vkDestroyShaderModule(VK->device, it4->second, nullptr);
        ++it4;
    }
    astShaders.clear();

    std::map<std::string, QeVKImage *>::iterator it5 = astTextures.begin();
    while (it5 != astTextures.end()) {
        if ((it5->second) != nullptr) delete (it5->second);
        ++it5;
    }
    astTextures.clear();
}

VkVertexInputBindingDescription QeVertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(QeVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 7> QeVertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 7> attributeDescriptions = {};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(QeVertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(QeVertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(QeVertex, uv);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(QeVertex, normal);

    attributeDescriptions[4].binding = 0;
    attributeDescriptions[4].location = 4;
    attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[4].offset = offsetof(QeVertex, tangent);

    attributeDescriptions[5].binding = 0;
    attributeDescriptions[5].location = 5;
    attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[5].offset = offsetof(QeVertex, joint);

    attributeDescriptions[6].binding = 0;
    attributeDescriptions[6].location = 6;
    attributeDescriptions[6].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[6].offset = offsetof(QeVertex, weight);

    return attributeDescriptions;
}

bool QeVertex::operator==(const QeVertex &other) const {
    return pos == other.pos && normal == other.normal && uv == other.uv && color == other.color;
}

AeXMLNode *QeGameAsset::getXMLEditNode(AE_GAMEOBJECT_TYPE _type, int eid) {
    std::string s = "";
    int type2 = 0;

    switch (_type) {
        case eGAMEOBJECT_Scene:
            s = "scenes";
            break;
        case eGAMEOBJECT_Object:
            s = "objects";
            break;
        default:
            s = "components";
            type2 = _type;
            break;
    }

    AeXMLNode *node = CONFIG->getXMLNode(s.c_str());

    if (node != nullptr && node->data->nexts.size() > 0) {
        if (type2 != 0) {
            bool b = false;
            for (int index = 0; index < node->data->nexts.size(); ++index) {
                int _type = node->data->nexts[index]->getXMLValue<int>("type");
                if (_type == type2) {
                    b = true;
                    node = node->data->nexts[index];
                    break;
                }
            }
            if (!b) return nullptr;
        }

        for (int index = 0; index < node->data->nexts.size(); ++index) {
            int _eid = node->data->nexts[index]->getXMLValue<int>("eid");
            if (_eid == eid) return node->data->nexts[index];
        }
    }
    return nullptr;
}

QeAssetModel *QeGameAsset::getModel(const char *_filename, bool bCubeMap, float *param) {
    std::string _filePath = combinePath(_filename, eAssetModel);
    std::map<std::string, QeAssetModel *>::iterator it = astModels.find(_filePath);

    if (it != astModels.end()) return it->second;

    char type = 0;

    if (strcmp("cube", _filename) == 0)
        type = eModelData_cube;
    else if (strcmp("axis", _filename) == 0)
        type = eModelData_axis;
    else if (strcmp("grid", _filename) == 0)
        type = eModelData_grid;
    else if (strcmp("line", _filename) == 0)
        type = eModelData_line;
    else if (strcmp("plane", _filename) == 0)
        type = eModelData_plane;

    else {
        char *ret = strrchr((char *)_filename, '.');

        // if (strcmp(ret + 1, "obj") == 0)		type = 0;
        // else
        if (strcmp(ret + 1, "gltf") == 0) type = eModelData_gltf;
        // else if (strcmp(ret + 1, "glb") == 0)	type = 2;
    }

    QeAssetModel *model = nullptr;
    QeAssetJSON *json = nullptr;
    std::vector<char> buffer;
    QeVertex vertex;
    float index = 0.f;

    switch (type) {
        // case 0:
        // buffer = loadFile(_filePath.c_str());
        // model = ENCODE->decodeOBJ(buffer.data());
        //	break;
        case eModelData_gltf:
            json = CM_MGR->getJSON(_filePath.c_str());
            model = G_ENCODE->decodeGLTF(json, bCubeMap);
            break;
            // case 2:
            //	model = ENCODE->decodeGLB(0);
            //	break;

        case eModelData_cube:
            /*model = new QeAssetModel();
            model->scale = { 1,1,1 };
            model->indices = {
            1,3,0,7,5,4,4,1,0,5,2,1,2,7,3,0,7,4,1,2,3,7,6,5,4,5,1,5,6,2,2,6,7,0,3,7 };

            model->indexSize = int(model->indices.size());
            vertex.uv = { 0,0,0,0 };
            vertex.color = { 1,1,1,1 };

            vertex.pos = { 1, -1, -1,1 };
            vertex.normal = { 0, 0, -1,1 };
            model->vertices.push_back(vertex);

            vertex.pos = { 1, -1, 1,1 };
            vertex.normal = { 1, 0, 0,1 };
            model->vertices.push_back(vertex);

            vertex.pos = { -1, -1, 1,1 };
            vertex.normal = { -1, 0, 0,1 };
            model->vertices.push_back(vertex);

            vertex.pos = { -1, -1, -1,1 };
            vertex.normal = { 0, 0, -1,1 };
            model->vertices.push_back(vertex);

            vertex.pos = { 1, 1, -1,1 };
            vertex.normal = { 1, 0, 0,1 };
            model->vertices.push_back(vertex);

            vertex.pos = { 1, 1, 1,1 };
            vertex.normal = { 0, 0, 1,1 };
            model->vertices.push_back(vertex);

            vertex.pos = { -1, 1, 1,1 };
            vertex.normal = { -1, 0, 0,1 };
            model->vertices.push_back(vertex);

            vertex.pos = { -1, 1, -1,1 };
            vertex.normal = { 0, 0, -1,1 };
            model->vertices.push_back(vertex);
            break;
            */
            model = new QeAssetModel();
            model->scale = {1, 1, 1};
            model->indices = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
                              18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};

            model->indexSize = int(model->indices.size());
            vertex.color = {1, 1, 1, 1};

            //
            vertex.normal = {0, 0, -1, 1};
            vertex.pos = {1, -1, -1, 1};
            vertex.uv = {0, 0, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, 1, -1, 1};
            vertex.uv = {1, 1, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {1, 1, -1, 1};
            vertex.uv = {1, 0, 0, 0};
            model->vertices.push_back(vertex);

            vertex.pos = {1, -1, -1, 1};
            vertex.uv = {0, 0, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, -1, -1, 1};
            vertex.uv = {0, 1, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, 1, -1, 1};
            vertex.uv = {1, 1, 0, 0};
            model->vertices.push_back(vertex);
            //
            vertex.normal = {0, 0, 1, 1};
            vertex.pos = {-1, 1, 1, 1};
            vertex.uv = {0, 0, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {1, -1, 1, 1};
            vertex.uv = {1, 1, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {1, 1, 1, 1};
            vertex.uv = {1, 0, 0, 0};
            model->vertices.push_back(vertex);

            vertex.pos = {-1, 1, 1, 1};
            vertex.uv = {0, 0, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, -1, 1, 1};
            vertex.uv = {0, 1, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {1, -1, 1, 1};
            vertex.uv = {1, 1, 0, 0};
            model->vertices.push_back(vertex);
            //
            vertex.normal = {1, 0, 0, 1};
            vertex.pos = {1, 1, 1, 1};
            vertex.uv = {0, 0, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {1, -1, -1, 1};
            vertex.uv = {1, 1, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {1, 1, -1, 1};
            vertex.uv = {1, 0, 0, 0};
            model->vertices.push_back(vertex);

            vertex.pos = {1, 1, 1, 1};
            vertex.uv = {0, 0, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {1, -1, 1, 1};
            vertex.uv = {0, 1, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {1, -1, -1, 1};
            vertex.uv = {1, 1, 0, 0};
            model->vertices.push_back(vertex);
            //
            vertex.normal = {0, -1, 0, 1};
            vertex.pos = {1, -1, 1, 1};
            vertex.uv = {0, 0, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, -1, -1, 1};
            vertex.uv = {1, 1, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {1, -1, -1, 1};
            vertex.uv = {1, 0, 0, 0};
            model->vertices.push_back(vertex);

            vertex.pos = {1, -1, 1, 1};
            vertex.uv = {0, 0, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, -1, 1, 1};
            vertex.uv = {0, 1, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, -1, -1, 1};
            vertex.uv = {1, 1, 0, 0};
            model->vertices.push_back(vertex);
            //
            vertex.normal = {-1, 0, 0, 1};
            vertex.pos = {-1, -1, -1, 1};
            vertex.uv = {0, 0, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, 1, 1, 1};
            vertex.uv = {1, 1, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, 1, -1, 1};
            vertex.uv = {1, 0, 0, 0};
            model->vertices.push_back(vertex);

            vertex.pos = {-1, -1, -1, 1};
            vertex.uv = {0, 0, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, -1, 1, 1};
            vertex.uv = {0, 1, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, 1, 1, 1};
            vertex.uv = {1, 1, 0, 0};
            model->vertices.push_back(vertex);
            //
            vertex.normal = {0, 1, 0, 1};
            vertex.pos = {1, 1, -1, 1};
            vertex.uv = {0, 0, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, 1, 1, 1};
            vertex.uv = {1, 1, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {1, 1, 1, 1};
            vertex.uv = {1, 0, 0, 0};
            model->vertices.push_back(vertex);

            vertex.pos = {1, 1, -1, 1};
            vertex.uv = {0, 0, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, 1, -1, 1};
            vertex.uv = {0, 1, 0, 0};
            model->vertices.push_back(vertex);
            vertex.pos = {-1, 1, 1, 1};
            vertex.uv = {1, 1, 0, 0};
            model->vertices.push_back(vertex);
            break;

        case eModelData_axis:

            model = new QeAssetModel();
            model->scale = {1, 1, 1};
            vertex.pos = {0, 0, 0, 1};

            vertex.normal = {1, 0, 0, 1};
            vertex.color = {1, 0, 0, 1};
            model->vertices.push_back(vertex);

            vertex.normal = {0, 1, 0, 1};
            vertex.color = {0, 1, 0, 1};
            model->vertices.push_back(vertex);

            vertex.normal = {0, 0, 1, 1};
            vertex.color = {0, 0, 1, 1};
            model->vertices.push_back(vertex);
            break;

        case eModelData_grid:

            model = new QeAssetModel();
            model->scale = {1, 1, 1};
            vertex.color = {param[0], param[1], param[2], 1.f};

            for (index = -param[3]; index <= param[3]; ++index) {
                vertex.pos = {index, -param[4], 0, 1};
                vertex.normal = {0, param[4] * 2, 0, 1};
                model->vertices.push_back(vertex);
            }
            for (index = -param[4]; index <= param[4]; ++index) {
                vertex.pos = {-param[3], index, 0, 1};
                vertex.normal = {param[3] * 2, 0, 0, 1};
                model->vertices.push_back(vertex);
            }
            break;

        case eModelData_line:

            model = new QeAssetModel();
            model->scale = {1, 1, 1};
            vertex.normal = {1, 0, 0, 1};
            vertex.uv = {0, 0, 1, 1};
            vertex.color = {param[0], param[1], param[2], 1.f};
            vertex.pos = {0, 0, 0, 1};
            model->vertices.push_back(vertex);
            break;

        case eModelData_plane:

            model = new QeAssetModel();
            model->scale = {1, 1, 1};
            model->indices = {0, 2, 1, 1, 2, 3};

            model->indexSize = int(model->indices.size());
            vertex.uv = {0, 0, 0, 0};
            vertex.color = {1, 1, 1, 1};

            vertex.pos = {-0.5f, -0.5f, 0.0f, 1.0f};
            vertex.normal = {0, 0, 1, 1};
            model->vertices.push_back(vertex);

            vertex.uv = {1, 0, 0, 0};
            vertex.pos = {-0.5f, 0.5f, 0.0f, 1.0f};
            vertex.normal = {0, 0, 1, 1};
            model->vertices.push_back(vertex);

            vertex.uv = {0, 1, 0, 0};
            vertex.pos = {0.5f, -0.5f, 0.0f, 1.0f};
            vertex.normal = {0, 0, 1, 1};
            model->vertices.push_back(vertex);

            vertex.uv = {1, 1, 0, 0};
            vertex.pos = {0.5f, 0.5f, 0.0f, 1.0f};
            vertex.normal = {0, 0, 1, 1};
            model->vertices.push_back(vertex);
            break;
    }

    // VK->createBufferData((void*)model->vertices.data(),
    // sizeof(model->vertices[0]) * model->vertices.size(), model->vertex.buffer,
    // model->vertex.memory, &model->vertex.mapped);
    VK->createBuffer(model->vertex, sizeof(model->vertices[0]) * model->vertices.size(), (void *)model->vertices.data());

    if (model->indexSize > 0) {
        // VK->createBufferData((void*)model->indices.data(),
        // sizeof(model->indices[0]) * model->indices.size(), model->index.buffer,
        // model->index.memory, &model->vertex.mapped);
        VK->createBuffer(model->index, sizeof(model->indices[0]) * model->indices.size(), (void *)model->indices.data());
    }
    // if (model->pMaterial && model->pMaterial->type == eMaterialPBR ) {
    // VK->createUniformBuffer(sizeof(model->pMaterial->value),
    // model->pMaterial->uboBuffer.buffer, model->pMaterial->uboBuffer.memory);
    // VK->setMemory(model->pMaterial->uboBuffer.memory,
    // (void*)&model->pMaterial->value, sizeof(model->pMaterial->value),
    // &model->pMaterial->uboBuffer.mapped);
    // VK->createBuffer(model->pMaterial->uboBuffer,
    // sizeof(model->pMaterial->value), (void*)&model->pMaterial->value);

    astMaterials[_filePath] = model->pMaterial;
    //}

    astModels[_filePath] = model;

    return model;
}

/*QeAssetMaterial* QeAsset::getMaterial(const char* _filename) {

        std::string _filePath = combinePath(_filename, eAssetMaterial);
        std::map<std::string, QeAssetMaterial*>::iterator it =
astMaterials.find(_filePath.c_str()); if (it != astMaterials.end())	return
it->second;

        std::vector<char> buffer = loadFile(_filePath.c_str());
        QeAssetMaterial* mtl = ENCODE->decodeMTL(buffer.data());

        //VK->createBuffer(mtl->uboBuffer, sizeof(mtl->value),
(void*)&mtl->value);
        //VK->createUniformBuffer(sizeof(mtl->value), mtl->uboBuffer.buffer,
mtl->uboBuffer.memory);
        //VK->setMemory(mtl->uboBuffer.memory, (void*)&mtl->value,
sizeof(mtl->value), &mtl->uboBuffer.mapped );

        astMaterials[_filePath] = mtl;

        return mtl;
}*/

QeAssetMaterial *QeGameAsset::getMaterialImage(const char *_filename, bool bCubeMap) {
    std::string _filePath = combinePath(_filename, eAssetTexture);
    std::map<std::string, QeAssetMaterial *>::iterator it = astMaterials.find(_filePath.c_str());
    if (it != astMaterials.end()) return it->second;

    QeAssetMaterial *mtl = new QeAssetMaterial();
    mtl->value.baseColor = {1, 1, 1, 1};
    // mtl->value.phong.ambient = { 1,1,1,1 };
    // mtl->value.phong.diffuse = { 1,1,1,1 };
    // mtl->value.phong.specular = { 1,1,1,1 };
    // mtl->value.phong.emissive = { 1,1,1,1 };
    // mtl->value.phong.param = { 1,1,1,1 };
    mtl->value.metallicRoughnessEmissive = {1.f, 1.f, 1.f, 1.f};

    if (_filePath.length() == 0) {
    } else if (bCubeMap)
        mtl->image.pCubeMap = getImage(_filename, bCubeMap, true);
    else
        mtl->image.pBaseColorMap = getImage(_filename, bCubeMap, true);

    // VK->createBuffer(mtl->uboBuffer, sizeof(mtl->value), (void*)&mtl->value);
    // VK->createUniformBuffer(sizeof(mtl->value), mtl->uboBuffer.buffer,
    // mtl->uboBuffer.memory); VK->setMemory(mtl->uboBuffer.memory,
    // (void*)&mtl->value, sizeof(mtl->value), &mtl->uboBuffer.mapped);

    astMaterials[_filePath] = mtl;

    return mtl;
}

QeVKImage *QeGameAsset::getImage(const char *_filename, bool bCubeMap, bool bGamma) {
    std::string _filePath = combinePath(_filename, eAssetTexture);
    std::map<std::string, QeVKImage *>::iterator it = astTextures.find(_filePath.c_str());

    if (it != astTextures.end()) return it->second;

    char type = 0;  // 0:BMP, 1:PNG, 2:JPEG

    char *ret = strrchr((char *)_filePath.c_str(), '.');

    VkFormat format;

    if (strcmp(ret + 1, "bmp") == 0) {
        type = 0;
        if (bGamma)
            format = VK_FORMAT_B8G8R8A8_SRGB;
        else
            format = VK_FORMAT_B8G8R8A8_UNORM;
    } else if (strcmp(ret + 1, "png") == 0) {
        type = 1;
        if (bGamma)
            format = VK_FORMAT_R8G8B8A8_SRGB;
        else
            format = VK_FORMAT_R8G8B8A8_UNORM;
    } else if (strcmp(ret + 1, "jpg") == 0 || strcmp(ret + 1, "jpeg") == 0) {
        type = 2;
        if (bGamma)
            format = VK_FORMAT_R8G8B8A8_SRGB;
        else
            format = VK_FORMAT_R8G8B8A8_UNORM;
    } else
        return nullptr;

    QeVKImage *image;

    if (bCubeMap)
        image = new QeVKImage(eImage_cube);
    else
        image = new QeVKImage(eImage_2D);
    // image->sampler = VK->createTextureSampler();

    std::vector<std::string> imageList;
    VkExtent2D imageSize;
    VkDeviceSize imageDataSize;
    std::vector<unsigned char> imageDatas;

    if (bCubeMap) {
        /*
        POSITIVE_X	Right
        NEGATIVE_X	Left
        POSITIVE_Y	Top
        NEGATIVE_Y	Bottom
        POSITIVE_Z	Back
        NEGATIVE_Z	Front
        */
        imageList = {"\\posz", "\\negz", "\\negx", "\\posx", "\\posy", "\\negy"};
    } else
        imageList = {""};

    int size = int(imageList.size());
    int cIndex = int(ret - _filePath.c_str());
    std::vector<unsigned char> data;
    int width, height, bytes;

    for (int i = 0; i < size; ++i) {
        std::string path(_filePath);
        path.insert(cIndex, imageList[i]);
        std::vector<char> buffer = CM_MGR->loadFile(path.c_str());

        switch (type) {
            case 0:
                data = ENCODE->decodeBMP((unsigned char *)buffer.data(), &width, &height, &bytes);
                break;
            case 1:
                data = ENCODE->decodePNG((unsigned char *)buffer.data(), &width, &height, &bytes);
                break;
            case 2:
                data = ENCODE->decodeJPEG((unsigned char *)buffer.data(), buffer.size(), &width, &height, &bytes);
                break;
        }
        if (bytes != 4) imageFillto32bits(&data, bytes);

        // uint32_t mipLevels = 1;//
        // static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

        // VK->createImageData((void*)data.data(), format, data.size(), width,
        // height, image->image, image->memory, i, bCubeMap);

        imageDatas.insert(imageDatas.end(), data.begin(), data.end());
        // if(i == 0)	VK->createImage(*image, data.size(), imageSize, format,
        // (void*)data.data()); else		VK->setMemoryImage(*image, data.size(),
        // imageSize, format, (void*)data.data(), i);
    }
    // image->view = VK->createImageView(image->image, format,
    // VK_IMAGE_ASPECT_COLOR_BIT, bCubeMap, mipLevels);
    imageSize = {uint32_t(width), uint32_t(height)};
    imageDataSize = data.size();

    VK->createImage(*image, imageDataSize, size, imageSize, format, (void *)imageDatas.data());

    astTextures[_filePath] = image;

    return image;
}

void QeGameAsset::imageFillto32bits(std::vector<unsigned char> *data, int bytes) {
    unsigned char c = 0xff;

    size_t size = data->size();
    size_t index = bytes;
    short int addBytes = 4 - bytes;

    while (index <= size) {
        for (int i = 0; i < addBytes; ++i) data->insert(data->begin() + index + i, c);

        index += 4;
        size = data->size();
    }
}

VkShaderModule QeGameAsset::getShader(const char *_filename) {
    std::string _filePath = combinePath(_filename, eAssetShader);
    std::map<std::string, VkShaderModule>::iterator it = astShaders.find(_filePath);
    if (it != astShaders.end()) return it->second;

    std::vector<char> buffer = CM_MGR->loadFile(_filePath.c_str());

    VkShaderModule shader = VK->createShaderModel((void *)buffer.data(), int(buffer.size()));
    astShaders[_filePath] = shader;
    return shader;
}

/*
QeAssetParticleRule *QeGameAsset::getParticle(int _eid) {
    std::map<int, QeAssetParticleRule *>::iterator it = astParticles.find(_eid);
    if (it != astParticles.end()) return it->second;

    AeXMLNode *node = getXMLEditNode(eComponent_partical, _eid);
    QeAssetParticleRule *particle = G_ENCODE->decodeParticle(node);
    astParticles[_eid] = particle;
    return particle;
}
*/

std::string QeGameAsset::combinePath(const char *_filename, QeGameAssetType dataType) {
    if (strlen(_filename) > 3 && _filename[1] == ':' && _filename[2] == '\\') return _filename;

    std::string rtn;
    switch (dataType) {
        case eAssetModel:
            rtn = CONFIG->getXMLValue<std::string>("setting.path.model");
            break;
        case eAssetMaterial:
            rtn = CONFIG->getXMLValue<std::string>("setting.path.material");
            break;
        case eAssetBin:
            rtn = CONFIG->getXMLValue<std::string>("setting.path.bin");
            break;
        case eAssetShader:
            rtn = CONFIG->getXMLValue<std::string>("setting.path.sharder");
            break;
        case eAssetTexture:
            rtn = CONFIG->getXMLValue<std::string>("setting.path.texture");
            break;
    }
    return rtn.append(_filename);
}

void QeGameAsset::setGraphicsShader(QeAssetGraphicsShader &shader, AeXMLNode *shaderData, const char *defaultShaderType) {
    const char *c = nullptr;

    if (shaderData) {
        c = shaderData->getXMLValue<const char *>("vert");
        if (c != nullptr && strlen(c)) shader.vert = getShader(c);
        c = shaderData->getXMLValue<const char *>("tesc");
        if (c != nullptr && strlen(c)) shader.tesc = getShader(c);
        c = shaderData->getXMLValue<const char *>("tese");
        if (c != nullptr && strlen(c)) shader.tese = getShader(c);
        c = shaderData->getXMLValue<const char *>("geom");
        if (c != nullptr && strlen(c)) shader.geom = getShader(c);
        c = shaderData->getXMLValue<const char *>("frag");
        if (c != nullptr && strlen(c)) shader.frag = getShader(c);
    }

    if (defaultShaderType && strlen(defaultShaderType)) {
        std::string keys = "shaders.graphics.";
        keys += defaultShaderType;
        AeXMLNode *node = CONFIG->getXMLNode(keys.c_str());
        if (shader.vert == nullptr) {
            c = node->getXMLValue<const char *>("vert");
            if (c != nullptr && strlen(c)) shader.vert = getShader(c);
        }
        if (shader.tesc == nullptr) {
            c = node->getXMLValue<const char *>("tesc");
            if (c != nullptr && strlen(c)) shader.tesc = getShader(c);
        }
        if (shader.tese == nullptr) {
            c = node->getXMLValue<const char *>("tese");
            if (c != nullptr && strlen(c)) shader.tese = getShader(c);
        }
        if (shader.geom == nullptr) {
            c = node->getXMLValue<const char *>("geom");
            if (c != nullptr && strlen(c)) shader.geom = getShader(c);
        }
        if (shader.frag == nullptr) {
            c = node->getXMLValue<const char *>("frag");
            if (c != nullptr && strlen(c)) shader.frag = getShader(c);
        }
    }
}
