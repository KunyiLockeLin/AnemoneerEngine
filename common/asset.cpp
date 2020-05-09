#include "common.h"
#include <sstream>
#include <stdarg.h>

std::map<std::string, QeAssetXML *> astXMLs;
std::map<std::string, QeAssetJSON *> astJSONs;

QeAssetXML::QeAssetXML() : data(new QeXML()) {}

QeAssetXML::~QeAssetXML() {
    std::vector<QeAssetXML *>::iterator it = data->nexts.begin();
    while (it != data->nexts.end()) {
        if ((*it) != nullptr) delete (*it);
        ++it;
    }
    data->nexts.clear();
    delete data;
    data = nullptr;
}

QeAssetXML *QeAssetXML::getXMLNode(const char *keys) { return getXMLNode(ENCODE->split(keys, ".")); }

QeAssetXML *QeAssetXML::getXMLNode(std::vector<std::string> &keys) {
    QeAssetXML *current = this;
    for (const auto &key : keys) {
        bool b = true;
        for (const auto &node : current->data->nexts) {
            if (key.compare(node->data->key) == 0) {
                current = node;
                b = false;
                break;
            }
        }
        if (b) return nullptr;
    }
    return current;
}

const char *QeAssetXML::getXMLValue(const char *keys) {
    const char *ret = nullptr;
    getXMLValue(ret, keys);
    return ret;
}

QeAssetXML *QeAssetXML::getXMLValue(const char *&value, const char *keys) { return getXMLValue(value, ENCODE->split(keys, ".")); }

QeAssetXML *QeAssetXML::getXMLValue(const char *&value, std::vector<std::string> &keys) {
    std::vector<std::string> keys1 = keys;
    keys1.pop_back();
    QeAssetXML *current = getXMLNode(keys1);
    if (!current) return nullptr;

    auto final_index = keys.size() - 1;
    keys1.resize(1);
    keys1[0] = keys[final_index];

    for (const auto &node : current->data->elements) {
        if (keys1[0].compare(node.key) == 0) {
            value = node.value.c_str();
            return current;
        }
    }

    current = getXMLNode(keys1);
    if (!current) return nullptr;
    if (keys1[0].compare(current->data->key.c_str()) == 0) {
        value = current->data->value.c_str();
        return current;
    }

    value = nullptr;
    return nullptr;
}

bool QeAssetXML::getXMLValueb(const char *keys) {
    bool ret = false;
    getXMLValueb(ret, keys);
    return ret;
}

QeAssetXML *QeAssetXML::getXMLValueb(bool &value, const char *keys) { return getXMLValueb(value, ENCODE->split(keys, ".")); }

QeAssetXML *QeAssetXML::getXMLValueb(bool &value, std::vector<std::string> &keys) {
    const char *value_c = nullptr;
    QeAssetXML *ret = getXMLValue(value_c, keys);
    if (ret) value = atoi(value_c);
    return ret;
}

int QeAssetXML::getXMLValuei(const char *keys) {
    int ret = 0;
    getXMLValuei(ret, keys);
    return ret;
}

QeAssetXML *QeAssetXML::getXMLValuei(int &value, const char *keys) { return getXMLValuei(value, ENCODE->split(keys, ".")); }

QeAssetXML *QeAssetXML::getXMLValuei(int &value, std::vector<std::string> &keys) {
    const char *value_c = nullptr;
    QeAssetXML *ret = getXMLValue(value_c, keys);
    if (ret) value = atoi(value_c);
    return ret;
}

float QeAssetXML::getXMLValuef(const char *keys) {
    float ret = 0.f;
    getXMLValuef(ret, keys);
    return ret;
}

QeAssetXML *QeAssetXML::getXMLValuef(float &value, const char *keys) { return getXMLValuef(value, ENCODE->split(keys, ".")); }

QeAssetXML *QeAssetXML::getXMLValuef(float &value, std::vector<std::string> &keys) {
    const char *value_c = nullptr;
    QeAssetXML *ret = getXMLValue(value_c, keys);
    if (ret) value = float(atof(value_c));
    return ret;
}

QeVector2i QeAssetXML::getXMLValueiXY(const char *keys) {
    QeVector2i ret = {};
    getXMLValueiXY(ret, keys);
    return ret;
}

QeAssetXML *QeAssetXML::getXMLValueiXY(QeVector2i &value, const char *keys) {
    return getXMLValueiXY(value, ENCODE->split(keys, "."));
}
QeAssetXML *QeAssetXML::getXMLValueiXY(QeVector2i &value, std::vector<std::string> &keys) {
    std::vector<std::string> keys1 = keys;
    keys1.pop_back();
    QeAssetXML *node = getXMLNode(keys1);
    if (!node) return nullptr;

    auto final_index = keys.size() - 1;
    keys1.resize(1);

    keys1[0] = keys[final_index] + "X";
    QeAssetXML *ret = node->getXMLValuei(value.x, keys1);

    keys1[0] = keys[final_index] + "Y";
    ret = node->getXMLValuei(value.y, keys1);
    return ret;
}

QeVector3i QeAssetXML::getXMLValueiXYZ(const char *keys) {
    QeVector3i ret = {};
    getXMLValueiXYZ(ret, keys);
    return ret;
}

QeAssetXML *QeAssetXML::getXMLValueiXYZ(QeVector3i &value, const char *keys) {
    return getXMLValueiXYZ(value, ENCODE->split(keys, "."));
}

QeAssetXML *QeAssetXML::getXMLValueiXYZ(QeVector3i &value, std::vector<std::string> &keys) {
    std::vector<std::string> keys1 = keys;
    keys1.pop_back();
    QeAssetXML *node = getXMLNode(keys1);
    if (!node) return nullptr;

    auto final_index = keys.size() - 1;
    keys1.resize(1);

    keys1[0] = keys[final_index] + "X";
    QeAssetXML *ret = node->getXMLValuei(value.x, keys1);

    keys1[0] = keys[final_index] + "Y";
    ret = node->getXMLValuei(value.y, keys1);

    keys1[0] = keys[final_index] + "Z";
    ret = node->getXMLValuei(value.z, keys1);
    return ret;
}

QeVector4i QeAssetXML::getXMLValueiXYZW(const char *keys) {
    QeVector4i ret = {};
    getXMLValueiXYZW(ret, keys);
    return ret;
}

QeAssetXML *QeAssetXML::getXMLValueiXYZW(QeVector4i &value, const char *keys) {
    return getXMLValueiXYZW(value, ENCODE->split(keys, "."));
}

QeAssetXML *QeAssetXML::getXMLValueiXYZW(QeVector4i &value, std::vector<std::string> &keys) {
    std::vector<std::string> keys1 = keys;
    keys1.pop_back();
    QeAssetXML *node = getXMLNode(keys1);
    if (!node) return nullptr;

    auto final_index = keys.size() - 1;
    keys1.resize(1);

    keys1[0] = keys[final_index] + "X";
    QeAssetXML *ret = node->getXMLValuei(value.x, keys1);

    keys1[0] = keys[final_index] + "Y";
    ret = node->getXMLValuei(value.y, keys1);

    keys1[0] = keys[final_index] + "Z";
    ret = node->getXMLValuei(value.z, keys1);

    keys1[0] = keys[final_index] + "W";
    ret = node->getXMLValuei(value.w, keys1);
    return ret;
}

QeVector2f QeAssetXML::getXMLValuefXY(const char *keys) {
    QeVector2f ret = {};
    getXMLValuefXY(ret, keys);
    return ret;
}

QeAssetXML *QeAssetXML::getXMLValuefXY(QeVector2f &value, const char *keys) {
    return getXMLValuefXY(value, ENCODE->split(keys, "."));
}
QeAssetXML *QeAssetXML::getXMLValuefXY(QeVector2f &value, std::vector<std::string> &keys) {
    std::vector<std::string> keys1 = keys;
    keys1.pop_back();
    QeAssetXML *node = getXMLNode(keys1);
    if (!node) return nullptr;

    auto final_index = keys.size() - 1;
    keys1.resize(1);

    keys1[0] = keys[final_index] + "X";
    QeAssetXML *ret = node->getXMLValuef(value.x, keys1);

    keys1[0] = keys[final_index] + "Y";
    ret = node->getXMLValuef(value.y, keys1);
    return ret;
}

QeVector3f QeAssetXML::getXMLValuefXYZ(const char *keys) {
    QeVector3f ret = {};
    getXMLValuefXYZ(ret, keys);
    return ret;
}

QeAssetXML *QeAssetXML::getXMLValuefXYZ(QeVector3f &value, const char *keys) {
    return getXMLValuefXYZ(value, ENCODE->split(keys, "."));
}

QeAssetXML *QeAssetXML::getXMLValuefXYZ(QeVector3f &value, std::vector<std::string> &keys) {
    std::vector<std::string> keys1 = keys;
    keys1.pop_back();
    QeAssetXML *node = getXMLNode(keys1);
    if (!node) return nullptr;

    auto final_index = keys.size() - 1;
    keys1.resize(1);

    keys1[0] = keys[final_index] + "X";
    QeAssetXML *ret = node->getXMLValuef(value.x, keys1);

    keys1[0] = keys[final_index] + "Y";
    ret = node->getXMLValuef(value.y, keys1);

    keys1[0] = keys[final_index] + "Z";
    ret = node->getXMLValuef(value.z, keys1);
    return ret;
}

QeVector4f QeAssetXML::getXMLValuefXYZW(const char *keys) {
    QeVector4f ret = {};
    getXMLValuefXYZW(ret, keys);
    return ret;
}

QeAssetXML *QeAssetXML::getXMLValuefXYZW(QeVector4f &value, const char *keys) {
    return getXMLValuefXYZW(value, ENCODE->split(keys, "."));
}

QeAssetXML *QeAssetXML::getXMLValuefXYZW(QeVector4f &value, std::vector<std::string> &keys) {
    std::vector<std::string> keys1 = keys;
    keys1.pop_back();
    QeAssetXML *node = getXMLNode(keys1);
    if (!node) return nullptr;

    auto final_index = keys.size() - 1;
    keys1.resize(1);

    keys1[0] = keys[final_index] + "X";
    QeAssetXML *ret = node->getXMLValuef(value.x, keys1);

    keys1[0] = keys[final_index] + "Y";
    ret = node->getXMLValuef(value.y, keys1);

    keys1[0] = keys[final_index] + "Z";
    ret = node->getXMLValuef(value.z, keys1);

    keys1[0] = keys[final_index] + "W";
    ret = node->getXMLValuef(value.w, keys1);
    return ret;
}

QeVector3f QeAssetXML::getXMLValueRGB(const char *keys) {
    QeVector3f ret = {};
    getXMLValueRGB(ret, keys);
    return ret;
}

QeAssetXML *QeAssetXML::getXMLValueRGB(QeVector3f &value, const char *keys) {
    return getXMLValueRGB(value, ENCODE->split(keys, "."));
}

QeAssetXML *QeAssetXML::getXMLValueRGB(QeVector3f &value, std::vector<std::string> &keys) {
    std::vector<std::string> keys1 = keys;
    keys1.pop_back();
    QeAssetXML *node = getXMLNode(keys1);
    if (!node) return nullptr;

    auto final_index = keys.size() - 1;
    keys1.resize(1);

    keys1[0] = keys[final_index] + "R";
    QeAssetXML *ret = node->getXMLValuef(value.x, keys1);

    keys1[0] = keys[final_index] + "G";
    ret = node->getXMLValuef(value.y, keys1);

    keys1[0] = keys[final_index] + "B";
    ret = node->getXMLValuef(value.z, keys1);
    return ret;
}

QeVector4f QeAssetXML::getXMLValueRGBA(const char *keys) {
    QeVector4f ret = {};
    getXMLValueRGBA(ret, keys);
    return ret;
}

QeAssetXML *QeAssetXML::getXMLValueRGBA(QeVector4f &value, const char *keys) {
    return getXMLValueRGBA(value, ENCODE->split(keys, "."));
}

QeAssetXML *QeAssetXML::getXMLValueRGBA(QeVector4f &value, std::vector<std::string> &keys) {
    std::vector<std::string> keys1 = keys;
    keys1.pop_back();
    QeAssetXML *node = getXMLNode(keys1);
    if (!node) return nullptr;

    auto final_index = keys.size() - 1;
    keys1.resize(1);

    keys1[0] = keys[final_index] + "R";
    QeAssetXML *ret = node->getXMLValuef(value.x, keys1);

    keys1[0] = keys[final_index] + "G";
    ret = node->getXMLValuef(value.y, keys1);

    keys1[0] = keys[final_index] + "B";
    ret = node->getXMLValuef(value.z, keys1);

    keys1[0] = keys[final_index] + "A";
    ret = node->getXMLValuef(value.w, keys1);
    return ret;
}

QeAssetXML *QeAssetXML::copyXMLNode() {
    QeAssetXML *node = new QeAssetXML();
    copyXMLNode(node);
    return node;
}

void QeAssetXML::copyXMLNode(QeAssetXML *to) {
    copyXMLValue(to);

    for (const auto &node : to->data->nexts) {
        if (node) delete node;
    }
    to->data->nexts.clear();

    for (const auto &node : data->nexts) {
        QeAssetXML *new_node = new QeAssetXML();
        to->data->nexts.push_back(new_node);
        node->copyXMLNode(new_node);
    }
}

void QeAssetXML::copyXMLValue(QeAssetXML *to) {
    to->data->comments = data->comments;
    to->data->key = data->key;
    to->data->value = data->value;
    to->data->elements = data->elements;
}

void QeAssetXML::addXMLNode(QeAssetXML *node) { data->nexts.push_back(node); }

void QeAssetXML::setXMLKey(const char *key) { this->data->key = key; }
void QeAssetXML::setXMLValue(const char *value) { this->data->value = value; }

void QeAssetXML::setXMLValue(const char *key, const char *value) {
    for (auto &node : data->elements) {
        if (node.key.compare(key) == 0) {
            node.value = value;
            return;
        }
    }
    QeNode node = {key, value};
    data->elements.push_back(node);
}

void QeAssetXML::removeXMLNode(QeAssetXML *node) {
    for (int i = 0; i < data->nexts.size(); ++i) {
        if (data->nexts[i] == node) {
            data->nexts.erase(data->nexts.begin() + i);
            delete node;
            return;
        }
    }
    for (const auto &node1 : data->nexts) {
        node1->removeXMLNode(node);
    }
}
void QeAssetXML::outputXML(const char *path, int level, std::string *content) {
    std::string s;

    if (!content) {
        content = &s;
    }
    std::string space = "";
    for (int i = 0; i < level; ++i) {
        space += "    ";
    }
    if (data->version.length()) {
        *content += "<?";
        *content += data->version;
        *content += "?>\n";
    }
    for (const auto &s : data->comments) {
        *content += space;
        *content += "<!--";
        *content += s;
        *content += "-->\n";
    }

    *content += space;
    *content += "<";
    *content += data->key;
    for (const auto &node : data->elements) {
        *content += " ";
        *content += node.key;
        *content += "=\"";
        *content += node.value;
        *content += "\"";
    }
    if (!data->nexts.size()) {
        if (data->value.length()) {
            *content += ">";
            *content += data->value;
            *content += "</";
            *content += data->key;
            *content += ">\n";
        } else {
            *content += " />\n";
        }
    } else {
        *content += ">\n";
        for (const auto &node : data->nexts) {
            node->outputXML(nullptr, level + 1, content);
        }

        *content += space;
        *content += "</";
        *content += data->key;
        *content += ">\n";
    }
    if (path) {
        std::ofstream ofile;
        ofile.open(path);

        ofile << s << std::endl;
        ofile.close();
    }
}

QeAssetJSON::QeAssetJSON() : data(new QeJSON) {}

QeAssetJSON::~QeAssetJSON() {
    std::vector<QeAssetJSON *>::iterator it = data->eNodes.begin();
    while (it != data->eNodes.end()) {
        if ((*it) != nullptr) delete (*it);
        ++it;
    }
    data->eNodes.clear();

    std::vector<std::vector<QeAssetJSON *>>::iterator it1 = data->eArrayNodes.begin();
    while (it1 != data->eArrayNodes.end()) {
        it = it1->begin();
        while (it != it1->end()) {
            if ((*it) != nullptr) delete (*it);
            ++it;
        }
        ++it1;
    }
    data->eArrayNodes.clear();
    delete data;
    data = nullptr;
}

const char *QeAssetJSON::getJSONValue(int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    const char *ret = getJSONValue(keys1, length);
    va_end(keys);
    delete[] keys1;
    return ret;
}

const char *QeAssetJSON::getJSONValue(const char *keys[], int length) {
    QeAssetJSON *source = this;

    for (int index = 0; index < length; ++index) {
        if (index == (length - 1)) {
            int size = int(source->data->eKeysforValues.size());
            for (int index1 = 0; index1 < size; ++index1) {
                if (strcmp(keys[index], source->data->eKeysforValues[index1].c_str()) == 0)
                    return source->data->eValues[index1].c_str();
            }
            break;
        }

        int size = int(source->data->eKeysforNodes.size());
        int index1 = 0;
        for (; index1 < size; ++index1) {
            if (strcmp(keys[index], source->data->eKeysforNodes[index1].c_str()) == 0) {
                source = source->data->eNodes[index1];
                break;
            }
        }
        if (index1 != size) continue;

        size = int(source->data->eKeysforArrayNodes.size());
        index1 = 0;
        for (; index1 < size; ++index1) {
            if (strcmp(keys[index], source->data->eKeysforArrayNodes[index1].c_str()) == 0) {
                int size2 = int(source->data->eArrayNodes[index1].size());
                int index2 = 0;
                for (; index2 < size2; ++index2) {
                    const char *ret = source->data->eArrayNodes[index1][index2]->getJSONValue(&keys[index + 1], length - index - 1);
                    if (ret != nullptr) return ret;
                }
                break;
            }
        }
        break;
    }
    return nullptr;
}

QeAssetJSON *QeAssetJSON::getJSONNode(int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    QeAssetJSON* source = getJSONNode(keys1, length);
    va_end(keys);
    delete[] keys1;
    return source;
}

QeAssetJSON *QeAssetJSON::getJSONNode(const char *keys[], int length) {
    int size = int(data->eKeysforNodes.size());
    for (int index = 0; index < size; ++index) {
        if (strcmp(keys[0], data->eKeysforNodes[index].c_str()) == 0) {
            if (length == 1) return data->eNodes[index];

            return data->eNodes[index]->getJSONNode(&keys[1], length - 1);
        }
    }

    size = int(data->eKeysforArrayNodes.size());
    for (int index = 0; index < size; ++index) {
        if (strcmp(keys[0], data->eKeysforArrayNodes[index].c_str()) == 0) {
            if (length == 1) return data->eArrayNodes[index][0];

            int size1 = int(data->eArrayNodes[index].size());

            for (int index1 = 0; index1 < size1; ++index1) {
                QeAssetJSON *ret = data->eArrayNodes[index][index1]->getJSONNode(&keys[1], length - 1);
                if (ret != nullptr) return ret;
            }
            break;
        }
    }
    return nullptr;
}

std::vector<std::string> *QeAssetJSON::getJSONArrayValues(int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    std::vector<std::string> *ret = getJSONArrayValues(keys1, length);
    va_end(keys);
    delete[] keys1;
    return ret;
}

std::vector<std::string> *QeAssetJSON::getJSONArrayValues(const char *keys[], int length) {
    QeAssetJSON *source = this;

    for (int index = 0; index < length; ++index) {
        if (index == (length - 1)) {
            int size = int(source->data->eKeysforArrayValues.size());
            for (int index1 = 0; index1 < size; ++index1) {
                if (strcmp(keys[index], source->data->eKeysforArrayValues[index1].c_str()) == 0)
                    return &source->data->eArrayValues[index1];
            }
            break;
        }

        int size = int(source->data->eKeysforNodes.size());
        int index1 = 0;
        for (; index1 < size; ++index1) {
            if (strcmp(keys[index], source->data->eKeysforNodes[index1].c_str()) == 0) {
                source = source->data->eNodes[index1];
                break;
            }
        }
        if (index1 != size) continue;

        size = int(source->data->eKeysforArrayNodes.size());
        index1 = 0;
        for (; index1 < size; ++index1) {
            if (strcmp(keys[index], source->data->eKeysforArrayNodes[index1].c_str()) == 0) {
                int size2 = int(source->data->eArrayNodes[index1].size());
                int index2 = 0;
                for (; index2 < size2; ++index2) {
                    std::vector<std::string> *ret =
                        source->data->eArrayNodes[index1][index2]->getJSONArrayValues(&keys[index + 1], length - index - 1);
                    if (ret != nullptr) return ret;
                }
                break;
            }
        }
        break;
    }
    return nullptr;
}

std::vector<QeAssetJSON *> *QeAssetJSON::getJSONArrayNodes(int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    std::vector<QeAssetJSON *> *ret = getJSONArrayNodes(keys1, length);
    va_end(keys);
    delete[] keys1;
    return ret;
}

std::vector<QeAssetJSON *> *QeAssetJSON::getJSONArrayNodes(const char *keys[], int length) {
    int size = int(data->eKeysforArrayNodes.size());
    for (int index = 0; index < size; ++index) {
        if (strcmp(keys[0], data->eKeysforArrayNodes[index].c_str()) == 0) {
            if (length == 1) return &data->eArrayNodes[index];

            int size1 = int(data->eArrayNodes[index].size());

            for (int index1 = 0; index1 < size1; ++index1) {
                std::vector<QeAssetJSON *> *ret = data->eArrayNodes[index][index1]->getJSONArrayNodes(&keys[1], length - 1);
                if (ret != nullptr) return ret;
            }
            break;
        }
    }

    if (length > 1) {
        size = int(data->eKeysforNodes.size());
        for (int index = 0; index < size; ++index) {
            if (strcmp(keys[0], data->eKeysforNodes[index].c_str()) == 0) {
                return data->eNodes[index]->getJSONArrayNodes(&keys[1], length - 1);
            }
        }
    }
    return nullptr;
}

bool QeAssetJSON::getJSONbValue(bool *output, int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    const char *ret = getJSONValue(keys1, length + 1);
    va_end(keys);
    delete[] keys1;

    if (ret) *output = atoi(ret);

    return ret;
}

bool QeAssetJSON::getJSONiValue(int *output, int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    const char *ret = getJSONValue(keys1, length + 1);
    va_end(keys);
    delete[] keys1;

    if (ret) *output = atoi(ret);

    return ret;
}

bool QeAssetJSON::getJSONfValue(float *output, int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    const char *ret = getJSONValue(keys1, length + 1);
    va_end(keys);
    delete[] keys1;

    if (ret) *output = float(atof(ret));

    return ret;
}

QeAsset::QeAsset() {}
QeAsset::~QeAsset() {
    std::map<std::string, QeAssetXML *>::iterator it = astXMLs.begin();
    while (it != astXMLs.end()) {
        if ((it->second) != nullptr) delete (it->second);
        ++it;
    }
    astXMLs.clear();

    std::map<std::string, QeAssetJSON *>::iterator it1 = astJSONs.begin();
    while (it1 != astJSONs.end()) {
        if ((it1->second) != nullptr) delete (it1->second);
        ++it1;
    }
    astJSONs.clear();
}

void QeAsset::removeXML(std::string path) {
    std::map<std::string, QeAssetXML *>::iterator it = astXMLs.find(path);

    if (it != astXMLs.end()) {
        delete it->second;
        astXMLs.erase(it);
    }
}

std::vector<char> QeAsset::loadFile(const char *_filePath) {
    std::vector<char> ret;
    std::ifstream file(_filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) return ret;

    file.seekg(0, file.end);
    int length = int(file.tellg());
    file.seekg(0);

    ret.resize(length);
    file.read(ret.data(), length);
    file.close();
    return ret;
}

QeAssetJSON *QeAsset::getJSON(const char *_filePath) {
    std::map<std::string, QeAssetJSON *>::iterator it = astJSONs.find(_filePath);

    if (it != astJSONs.end()) return it->second;

    std::vector<char> buffer = loadFile(_filePath);

    int index = 0;
    QeAssetJSON *head = ENCODE->decodeJSON(buffer.data(), index);
    astJSONs[_filePath] = head;
    return head;
}

QeAssetXML *QeAsset::getXML(const char *_filePath) {
    std::map<std::string, QeAssetXML *>::iterator it = astXMLs.find(_filePath);

    if (it != astXMLs.end()) return it->second;

    // std::ifstream file(_filePath, std::ios::ate | std::ios::binary);
    // if (!file.is_open()) return nullptr;

    std::vector<char> buffer = loadFile(_filePath);

    int index = 0;
    QeAssetXML *head = ENCODE->decodeXML(buffer.data(), index);
    astXMLs[_filePath] = head;

    return head;
}
