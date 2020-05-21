#pragma once

#include <stdio.h>
#include "common/common.h"
#include <direct.h>
//#include "src/generated_config_struct_enum.h"

const std::string indent = "    ";

void AddCommentEnum(AeXMLNode& node, AeFile& file) {
    for (auto& comment : node.data->comments) {
        if (!comment.compare(0, 5, "enum ")) {
            file.addNewLine("");
            std::vector<std::string> ss = ENCODE.split<std::string>(comment, " ");
            std::string s = "enum " + ss[2] + " {";
            file.addNewLine(s.c_str());
            for (size_t i = 3; i < ss.size(); i += 2) {
                s = indent + ss[i] + " = " + ss[i + 1] + ",";
                file.addNewLine(s.c_str());
            }
            file.addNewLine("};");
        }
    }
    for (auto next : node.data->nexts) {
        AddCommentEnum(*next, file);
    }
}

std::string AddElementCode(std::string& key, std::vector<std::string>& types, std::vector<std::string>& load_codes) {
    if (types.size() != 2) {
        load_codes.push_back(key + " = property_->getXMLValue<" + types[0] + ">(\"" + key + "\");");
        return (types[0] + " " + key + ";");
    }
    std::string type1 = "AeVector<" + types[0] + ", " + types[1] + ">";
    load_codes.push_back(key + " = property_->getXMLValues<" + types[0] + ", " + types[1] + ">(\"" + key + "\");");
    return (type1 + " " + key + ";");
}

void AddGameObjectComponentStrcut(AeXMLNode& node, AeFile& file) {
    // struct
    file.addNewLine("");
    std::string s = "struct AeGameObjectComponent" + node.data->key + "Data {";
    file.addNewLine(s.c_str());
    auto* define_node = node.getXMLNode("define");
    std::vector<std::string> load_codes;

    for (auto& element : define_node->data->elements) {
        s = indent;
        if (!element.key.compare("type") || !element.key.compare("oid") || !element.key.compare("eid")) {
        } else {
            std::vector<std::string> types = ENCODE.split<std::string>(element.value, " ");
            if (types[0].compare("enum") == 0) {
                for (auto& comment : define_node->data->comments) {
                    std::vector<std::string> ss = ENCODE.split<std::string>(comment, " ");
                    if (!element.key.compare(ss[1])) {
                        s += (ss[2] + " " + element.key + ";");
                        load_codes.push_back(element.key + " = static_cast<" + ss[2] + ">(property_->getXMLValue<int>(\"" +
                                             element.key + "\"));");
                        break;
                    }
                }
            } else {
                s += AddElementCode(element.key, types, load_codes);
            }
            file.addNewLine(s.c_str());
        }
    }

    // read xml
    file.addNewLine("");
    file.addNewLine((indent + "AeXMLNode* property_;").c_str());
    file.addNewLine((indent + "void read(AeXMLNode& node) {").c_str());
    file.addNewLine((indent + indent + "property_ = &node;").c_str());
    for (auto& load_code : load_codes) {
        file.addNewLine((indent + indent + load_code).c_str());
    }
    file.addNewLine((indent + "}").c_str());

    file.addNewLine((indent + "void reset() { read(*property_); }").c_str());
    file.addNewLine("};");
}

int main(int argc, char* argv[]) {
    ASSERT(0, "sss");
    // generated_config_struct_enum.h
    LOG("Create generated\\generated_config_struct_enum.h");
    AeFile file;
    file.open("..\\..\\generated\\generated_config_struct_enum.h");
    file.addNewLine("#pragma once");
    file.addNewLine("//This file is generated by code_generator.cpp.DO NOT edit this file.");
    file.addNewLine("#include \"common/common.h\"");
    // file.addNewLine("#include <cstring>");

    auto* config = CM_MGR.getXML("..\\..\\output\\data\\config.xml");

    // enum gameobject
    file.addNewLine("");
    file.addNewLine("enum AE_GAMEOBJECT_TYPE {");
    std::string key = indent + "eGAMEOBJECT_";

    auto* node = config->getXMLNode("scenes.define");
    std::string enum_name = key + "Scene = " + node->getXMLValue<int>("type") + ",";
    file.addNewLine(enum_name.c_str());

    node = config->getXMLNode("objects.define");
    enum_name = key + "Object = " + node->getXMLValue<int>("type") + ",";
    file.addNewLine(enum_name.c_str());

    node = config->getXMLNode("components");
    key += "Component_";
    for (auto comp : node->data->nexts) {
        enum_name = key + comp->data->key + " = " + comp->getXMLValue<int>("define.type") + ",";
        file.addNewLine(enum_name.c_str());
    }
    file.addNewLine("};");
    // comment enum
    AddCommentEnum(*config, file);

    // struct
    file.addNewLine("");
    file.addNewLine("struct AeGameObjectData {");
    file.addNewLine((indent + "std::string name;").c_str());
    file.addNewLine((indent + "AE_GAMEOBJECT_TYPE type;").c_str());
    file.addNewLine((indent + "int oid;").c_str());
    file.addNewLine((indent + "int eid;").c_str());
    file.addNewLine("");
    file.addNewLine((indent + "AeXMLNode* property_;").c_str());
    file.addNewLine((indent + "void read(AeXMLNode& node) {").c_str());
    file.addNewLine((indent + indent + "property_ = &node;").c_str());
    file.addNewLine((indent + indent + "name = property_->data->key;").c_str());
    file.addNewLine((indent + indent + "type = static_cast<AE_GAMEOBJECT_TYPE>(property_->getXMLValue<int>(\"type\"));").c_str());
    file.addNewLine((indent + indent + "oid = property_->getXMLValue<int>(\"oid\");").c_str());
    file.addNewLine((indent + indent + "eid = property_->getXMLValue<int>(\"eid\");").c_str());
    file.addNewLine((indent + "}").c_str());
    file.addNewLine((indent + "void reset() { read(*property_); }").c_str());
    file.addNewLine("};");

    node = config->getXMLNode("components");
    for (auto comp : node->data->nexts) {
        AddGameObjectComponentStrcut(*comp, file);
    }

    file.close();
    LOG("CodeGenerator End.");
    return 0;
}
