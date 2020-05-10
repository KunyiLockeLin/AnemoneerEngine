#pragma once

#define DllExport __declspec(dllexport)
#define DllImport __declspec(dllimport)

#include <cstring>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>

#define SINGLETON_CLASS(class_name)           \
   private:                                   \
    class_name();                             \
                                              \
   public:                                    \
    static inline class_name *getInstance() { \
        static class_name instance;           \
        return &instance;                     \
    }

// template class DllExport std::vector<std::string>;
// template class DllExport std::basic_string<char>;

const char INDEX_NONE = -1;

#define ARRAY_SIZE(c_array) sizeof(c_array) / sizeof(c_array[0])

template <class T, int N>
struct DllExport AeVector {
    const int size = N;

    union {
        T elements[N];
        struct {
            T x, y, z, w;
        } pos;
        struct {
            T r, g, b, a;
        } color;
        struct {
            T u, v;
        } uv;
        struct {
            T width, height, depth;
        } volume;
    };

    AeVector();

    template <class T2, int N2>
    bool operator==(const AeVector<T2, N2> &other) const;

    template <class T2, int N2>
    bool operator!=(const AeVector<T2, N2> &other) const;

    template <class T2, int N2>
    AeVector<T, N> &operator=(const AeVector<T2, N2> &other);

    template <class T2, int N2>
    AeVector<T, N> &operator+=(const AeVector<T2, N2> &other);
    template <class T2, int N2>
    AeVector<T, N> &operator-=(const AeVector<T2, N2> &other);
    template <class T2, int N2>
    AeVector<T, N> &operator*=(const AeVector<T2, N2> &other);
    template <class T2, int N2>
    AeVector<T, N> &operator/=(const AeVector<T2, N2> &other);

    template <class T2, int N2>
    AeVector<T, N> operator+(const AeVector<T2, N2> &other);
    template <class T2, int N2>
    AeVector<T, N> operator-(const AeVector<T2, N2> &other);
    template <class T2, int N2>
    AeVector<T, N> operator*(const AeVector<T2, N2> &other);
    template <class T2, int N2>
    AeVector<T, N> operator/(const AeVector<T2, N2> &other);

    template <class T2>
    AeVector<T, N> &operator+=(const T2 &other);
    template <class T2>
    AeVector<T, N> &operator-=(const T2 &other);
    template <class T2>
    AeVector<T, N> &operator*=(const T2 &other);
    template <class T2>
    AeVector<T, N> &operator/=(const T2 &other);

    template <class T2>
    AeVector<T, N> operator+(const T2 &other);
    template <class T2>
    AeVector<T, N> operator-(const T2 &other);
    template <class T2>
    AeVector<T, N> operator*(const T2 &other);
    template <class T2>
    AeVector<T, N> operator/(const T2 &other);
};

/*
template <class T, int N>
struct DllExport AeMatrix {
    const int size = N;
    T e[N][N];

    AeMatrix();
    AeMatrix(float value);
    AeMatrix<T, N> &operator*=(const AeMatrix<T, N> &other);
    AeMatrix<T, N> operator*(const AeMatrix<T, N> &other);
    AeVector<N> operator*(const AeVector<N> &other);
    AeMatrix<T, N> &operator/=(const float &other);
};
*/

struct DllExport QeVector2i {
    int x, y;

    QeVector2i();
    QeVector2i(int _x, int _y);
};

struct DllExport QeVector2f {
    float x, y;

    QeVector2f();
    QeVector2f(float _x, float _y);

    bool operator==(const QeVector2f &other) const;
    QeVector2f &operator+=(const QeVector2f &other);
    QeVector2f &operator-=(const QeVector2f &other);
    QeVector2f &operator/=(const float &other);
    QeVector2f &operator*=(const float &other);
    QeVector2f operator/(const float &other);
    QeVector2f operator*(const float &other);
};

struct DllExport QeVector3i {
    int x, y, z;

    QeVector3i();
    QeVector3i(int _x, int _y, int _z);

    bool operator==(const QeVector3i &other) const;
    QeVector3i &operator+=(const QeVector3i &other);
    QeVector3i &operator-=(const QeVector3i &other);
    QeVector3i &operator-=(const int &other);
    QeVector3i &operator/=(const int &other);
    QeVector3i operator-(const QeVector3i &other);
    QeVector3i operator/(const int &other);
    QeVector3i operator*(const int &other);
};

struct QeVector4f;
struct DllExport QeVector3f {
    float x, y, z;

    QeVector3f();
    QeVector3f(float _x, float _y, float _z);
    QeVector3f(int _x, int _y, int _z);
    QeVector3f(const QeVector4f &other);

    bool operator==(const QeVector3f &other) const;
    bool operator!=(const QeVector3f &other) const;

    QeVector3f &operator=(const QeVector4f &other);
    QeVector3f &operator+=(const QeVector3f &other);
    QeVector3f &operator-=(const QeVector3f &other);
    QeVector3f &operator*=(const QeVector3f &other);
    QeVector3f &operator/=(const QeVector3f &other);
    QeVector3f &operator-=(const float &other);
    QeVector3f &operator+=(const float &other);
    QeVector3f &operator/=(const float &other);
    QeVector3f &operator*=(const float &other);
    QeVector3f operator+(const QeVector3f &other);
    QeVector3f operator-(const QeVector3f &other);
    QeVector3f operator*(const QeVector3f &other);
    QeVector3f operator+(const float &other);
    QeVector3f operator/(const float &other);
    QeVector3f operator*(const float &other);
};

struct DllExport QeVector4s {
    short int x, y, z, w;
    QeVector4s();
};

struct DllExport QeVector4i {
    int x, y, z, w;
    QeVector4i();
    QeVector4i &operator=(const QeVector4s &other);
};

struct DllExport QeVector4f {
    float x, y, z, w;

    QeVector4f();
    QeVector4f(float _x, float _y, float _z, float _w);
    QeVector4f(int _x, int _y, int _z, int _w);
    QeVector4f(const QeVector3f &other, float _w);

    bool operator==(const QeVector4f &other) const;
    bool operator!=(const QeVector4f &other) const;

    QeVector4f &operator=(const QeVector3f &other);
    QeVector4f &operator=(const QeVector2f &other);
    QeVector4f &operator=(const QeVector4s &other);
    QeVector4f &operator+=(const QeVector3f &other);
    QeVector4f operator+(const QeVector3f &other);
    QeVector4f operator-(const QeVector3f &other);
    QeVector4f operator/(const float &other);
    QeVector4f &operator*=(const float &other);
};

struct DllExport QeMatrix4x4f {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;

    QeMatrix4x4f();
    QeMatrix4x4f(float _num);
    QeMatrix4x4f(float __00, float __01, float __02, float __03, float __10, float __11, float __12, float __13, float __20,
                 float __21, float __22, float __23, float __30, float __31, float __32, float __33);
    QeMatrix4x4f &operator*=(const QeMatrix4x4f &other);
    QeMatrix4x4f operator*(const QeMatrix4x4f &other);
    QeVector4f operator*(const QeVector4f &other);
    QeMatrix4x4f &operator/=(const float &other);
};

struct DllExport QeRay {
    QeVector3f origin;
    QeVector3f direction;
    QeVector3f positionByTime(float t);
};

struct DllExport QeRayHitRecord {
    float t;
    QeVector3f position;
    QeVector3f normal;
};

struct DllExport QeBoundingSphere {
    QeVector3f center;
    float radius;
};

struct DllExport QeBoundingBox {
    QeVector3f min;
    QeVector3f max;
};

class DllExport QeBinaryTree {
    void *data;
    int key;
    QeBinaryTree *left = nullptr;
    QeBinaryTree *right = nullptr;
    void insertNode(QeBinaryTree &node) {}
    void removeNode(QeBinaryTree &node) {}
    QeBinaryTree *getNode(int &key) { return nullptr; }
};

// Right-handed Coordinate System
class DllExport QeMath {
    SINGLETON_CLASS(QeMath)
   public:
    ~QeMath();

    const float PI = 3.1415927f;
    const float RADIANS_TO_DEGREES = 180.0f / PI;
    const float DEGREES_TO_RADIANS = PI / 180;

    int iRandom(int start, int range);
    void iRandoms(int start, int range, int size, int *ret);
    float fRandom(float start, float range);
    void fRandoms(float start, float range, int size, float *ret);
    QeMatrix4x4f lookAt(QeVector3f &_pos, QeVector3f &_center, QeVector3f &_up);
    QeMatrix4x4f perspective(float _fov, float _aspect, float _near, float _far);
    QeMatrix4x4f translate(QeVector3f &_pos);
    QeVector3f move(QeVector3f &_position, QeVector3f &_addMove, QeVector3f &_face, QeVector3f &_up);
    QeMatrix4x4f rotate_quaternion(QeVector3f &_eulerAngles);
    QeMatrix4x4f rotate_quaternion(QeVector4f &quaternion);
    QeMatrix4x4f rotate_quaternion(float _angle, QeVector3f &_axis);
    QeVector4f eulerAngles_to_quaternion(QeVector3f &_eulerAngles);
    QeVector4f axis_to_quaternion(float _angle, QeVector3f &_axis);
    QeMatrix4x4f rotate_eularAngles(QeVector3f &_eulerAngles);  // (roll, pitch, yaw) or (bank, attitude, heading)
    QeMatrix4x4f rotate_axis(float _angle, QeVector3f &_axis);
    QeMatrix4x4f rotateX(float _angle);
    QeMatrix4x4f rotateY(float _angle);
    QeMatrix4x4f rotateZ(float _angle);
    QeVector4f matrix_to_quaternion(QeMatrix4x4f matrix);
    QeMatrix4x4f scale(QeVector3f &_size);
    QeMatrix4x4f transform(QeVector3f &_tanslation, QeVector4f &_rotation_quaternion, QeVector3f &_scale);
    QeMatrix4x4f getTransformMatrix(QeVector3f &_translate, QeVector3f &_rotateEuler, QeVector3f &_scale,
                                    QeVector3f &camera_world_position, bool bRotate = true, bool bFixSize = false);
    QeVector3f normalize(QeVector3f &_vec);
    QeVector4f normalize(QeVector4f &_vec);
    QeVector3f eulerAnglesToVector(QeVector3f &_eulerAngles);
    QeVector3f vectorToEulerAngles(QeVector3f &_vector);
    float length(QeVector2f &_vec);
    float length(QeVector3f &_vec);
    float length(QeVector4f &_vec);
    float dot(QeVector2f &_vec1, QeVector2f &_vec2);
    float dot(QeVector3f &_vec1, QeVector3f &_vec2);
    float dot(QeVector4f &_vec1, QeVector4f &_vec2);
    QeVector3f cross(QeVector3f &_vec1, QeVector3f &_vec2);
    float fastSqrt(float _number);
    bool inverse(QeMatrix4x4f &_inMat, QeMatrix4x4f &_outMat);
    QeMatrix4x4f transpose(QeMatrix4x4f &_mat);
    int clamp(int in, int low, int high);
    float clamp(float in, float low, float high);
    QeVector4f interpolateDir(QeVector4f &a, QeVector4f &b, float blend);
    QeVector3f interpolatePos(QeVector3f &start, QeVector3f &end, float progression);
    float getAnglefromVectors(QeVector3f &v1, QeVector3f &v2);
    QeVector3f revolute_axis(QeVector3f &_position, QeVector3f &_addRevolute, QeVector3f &_centerPosition, bool bFixX = false,
                             bool bFixY = false, bool bFixZ = false);
    QeVector3f revolute_eularAngles(QeVector3f &_position, QeVector3f &_addRevolute, QeVector3f &_centerPosition, bool bFixX,
                                    bool bFixY, bool bFixZ);

    // void getAnglefromVector(QeVector3f& inV, float & outPolarAngle, float & outAzimuthalAngle);
    // void rotatefromCenter(QeVector3f& center, QeVector3f& pos, float polarAngle, float azimuthalAngle);
    // void rotatefromCenter(QeVector3f& center, QeVector3f& pos, QeVector2f & axis, float angle, bool bStopTop);
    bool hit_test_raycast_sphere(QeRay &ray, QeBoundingSphere &sphere, float maxDistance = 0.f, QeRayHitRecord *hit = nullptr);
    void quicksort(float *data, int count);
};
#define MATH QeMath::getInstance()

enum DllExport QeAssetType {
    eAssetXML = 0,
    eAssetJSON = 1,
};

struct QeNode {
    std::string key;
    std::string value;
};

struct QeAssetXML;
struct QeXML {
    std::string version;
    std::vector<std::string> comments;
    std::string key;
    std::string value;
    std::vector<QeNode> elements;
    std::vector<QeAssetXML *> nexts;
    QeAssetXML *parent = nullptr;
};

struct DllExport QeAssetXML {
    QeXML *data;

    QeAssetXML();
    ~QeAssetXML();

    QeAssetXML *getXMLNode(const char *keys);
    QeAssetXML *getXMLNode(std::vector<std::string> &keys);

    const char *getXMLValue(const char *keys);
    QeAssetXML *getXMLValue(const char *&value, const char *keys);
    QeAssetXML *getXMLValue(const char *&value, std::vector<std::string> &keys);

    bool getXMLValueb(const char *keys);
    QeAssetXML *getXMLValueb(bool &value, const char *keys);
    QeAssetXML *getXMLValueb(bool &value, std::vector<std::string> &keys);

    int getXMLValuei(const char *keys);
    QeAssetXML *getXMLValuei(int &value, const char *keys);
    QeAssetXML *getXMLValuei(int &value, std::vector<std::string> &keys);

    float getXMLValuef(const char *keys);
    QeAssetXML *getXMLValuef(float &value, const char *keys);
    QeAssetXML *getXMLValuef(float &value, std::vector<std::string> &keys);

    QeVector2i getXMLValueiXY(const char *keys);
    QeAssetXML *getXMLValueiXY(QeVector2i &value, const char *keys);
    QeAssetXML *getXMLValueiXY(QeVector2i &value, std::vector<std::string> &keys);

    QeVector3i getXMLValueiXYZ(const char *keys);
    QeAssetXML *getXMLValueiXYZ(QeVector3i &value, const char *keys);
    QeAssetXML *getXMLValueiXYZ(QeVector3i &value, std::vector<std::string> &keys);

    QeVector4i getXMLValueiXYZW(const char *keys);
    QeAssetXML *getXMLValueiXYZW(QeVector4i &value, const char *keys);
    QeAssetXML *getXMLValueiXYZW(QeVector4i &value, std::vector<std::string> &keys);

    QeVector2f getXMLValuefXY(const char *keys);
    QeAssetXML *getXMLValuefXY(QeVector2f &value, const char *keys);
    QeAssetXML *getXMLValuefXY(QeVector2f &value, std::vector<std::string> &keys);

    QeVector3f getXMLValuefXYZ(const char *keys);
    QeAssetXML *getXMLValuefXYZ(QeVector3f &value, const char *keys);
    QeAssetXML *getXMLValuefXYZ(QeVector3f &value, std::vector<std::string> &keys);

    QeVector4f getXMLValuefXYZW(const char *keys);
    QeAssetXML *getXMLValuefXYZW(QeVector4f &value, const char *keys);
    QeAssetXML *getXMLValuefXYZW(QeVector4f &value, std::vector<std::string> &keys);

    QeVector3f getXMLValueRGB(const char *keys);
    QeAssetXML *getXMLValueRGB(QeVector3f &value, const char *keys);
    QeAssetXML *getXMLValueRGB(QeVector3f &value, std::vector<std::string> &keys);

    QeVector4f getXMLValueRGBA(const char *keys);
    QeAssetXML *getXMLValueRGBA(QeVector4f &value, const char *keys);
    QeAssetXML *getXMLValueRGBA(QeVector4f &value, std::vector<std::string> &keys);

    QeAssetXML *copyXMLNode();
    void copyXMLValue(QeAssetXML *to);
    void copyXMLNode(QeAssetXML *to);

    void addXMLNode(QeAssetXML *node);
    void removeXMLNode(QeAssetXML *node);

    void setXMLKey(const char *key);
    void setXMLValue(const char *value);
    void setXMLValue(const char *key, const char *value);

    void outputXML(const char *path, int level = 0, std::string *content = nullptr);
};

struct QeAssetJSON;
struct QeJSON {
    std::vector<std::string> eKeysforValues;
    std::vector<std::string> eValues;
    std::vector<std::string> eKeysforNodes;
    std::vector<QeAssetJSON *> eNodes;
    std::vector<std::string> eKeysforArrayValues;
    std::vector<std::vector<std::string>> eArrayValues;
    std::vector<std::string> eKeysforArrayNodes;
    std::vector<std::vector<QeAssetJSON *>> eArrayNodes;
};

struct DllExport QeAssetJSON {
    QeJSON *data;

    QeAssetJSON();
    ~QeAssetJSON();

    const char *getJSONValue(int length, ...);
    const char *getJSONValue(const char *keys[], int length);
    QeAssetJSON *getJSONNode(int length, ...);
    QeAssetJSON *getJSONNode(const char *keys[], int length);
    std::vector<std::string> *getJSONArrayValues(int length, ...);
    std::vector<std::string> *getJSONArrayValues(const char *keys[], int length);
    std::vector<QeAssetJSON *> *getJSONArrayNodes(int length, ...);
    std::vector<QeAssetJSON *> *getJSONArrayNodes(const char *keys[], int length);
    bool getJSONbValue(bool *output, int length, ...);
    bool getJSONiValue(int *output, int length, ...);
    bool getJSONfValue(float *output, int length, ...);
};

class DllExport QeAsset {
    SINGLETON_CLASS(QeAsset)
   public:
    ~QeAsset();

    QeAssetJSON *getJSON(const char *_filePath);
    QeAssetXML *getXML(const char *_filePath);
    void removeXML(std::string path);

    std::vector<char> loadFile(const char *_filePath);
};
#define AST QeAsset::getInstance()

class DllExport QeEncode {
    SINGLETON_CLASS(QeEncode)
   public:
    ~QeEncode() {}

    QeAssetXML *decodeXML(const char *buffer, int &index, QeAssetXML *parent = nullptr);
    QeAssetJSON *decodeJSON(const char *buffer, int &index);
    // QeAssetModel* decodeOBJ(char* buffer);
    // QeAssetModel* decodeGLB(char* buffer);
    // QeAssetMaterial* decodeMTL(char* buffer);
    std::vector<unsigned char> decodeJPEG(unsigned char *buffer, size_t size, int *width, int *height, int *bytes);
    std::vector<unsigned char> decodeBMP(unsigned char *buffer, int *width, int *height, int *bytes);
    std::vector<unsigned char> decodePNG(unsigned char *buffer, int *width, int *height, int *bytes);
    std::vector<unsigned char> decodeDeflate(unsigned char *in, unsigned int size);

    int readBits(const unsigned char *stream, size_t *bitPointer, size_t readCount, bool bLeft = false, bool bNegative = false);
    std::string trim(std::string s);
    std::vector<std::string> split(std::string s, std::string delim);
};
#define ENCODE QeEncode::getInstance()

class DllExport QeTimer {
   public:
    QeTimer();
    ~QeTimer();
    int timerMilliSecond = 0;
    std::chrono::steady_clock::time_point *lastTime;

    std::chrono::steady_clock::time_point getNowTime();
    void initTime();
    void setTimer(int timerMilliSecond);
    int getPassTime();
    bool checkTimer(int &passMilliSecond);
};

class DllExport AeFile {
   public:
    AeFile();
    ~AeFile();

    std::ofstream *ofile;
    std::string *output_path;

    bool open(const char *output_path);
    bool isOpen();
    bool addNewLine(const char *s);
    void close();
};

class DllExport AeLogListener {
   public:
    virtual void updateLog(const char *msg) {}
};

class DllExport AeLog {
    SINGLETON_CLASS(AeLog)
   public:
    ~AeLog();

    AeFile *file;
    std::vector<AeLogListener *> *listeners;

    void addListener(AeLogListener &listener);
    void removeListener(AeLogListener &listener);
    void switchOutput(bool turn_on, const char *output_path = nullptr);
    std::string stack(int from, int to);
    void print(std::string &msg, bool bShowStack = false, int stackLevel = 4);

    bool isOutput();
};
#define LOGOBJ AeLog::getInstance()
#define LOG(msg) LOGOBJ->print(std::string("") + msg)
#define STACK(msg) LOGOBJ->print(std::string("") + msg, true)

namespace AeLib {
std::string DllExport toString(const int &i);
std::string DllExport operator+(std::string const &a, const int &b);
std::string DllExport operator+(std::string const &a, const size_t &b);
std::string DllExport operator+(std::string const &a, const float &b);
std::string DllExport operator+(std::string const &a, const double &b);
std::string DllExport operator+(std::string const &a, const char *b);
std::string DllExport operator+=(std::string const &a, const int &b);
std::string DllExport operator+=(std::string const &a, const size_t &b);
std::string DllExport operator+=(std::string const &a, const float &b);
std::string DllExport operator+=(std::string const &a, const double &b);
std::string DllExport operator+=(std::string const &a, const char *b);

template <class T>
int DllExport findElementFromVector(std::vector<T> &vec, T element);

template <class T>
bool DllExport eraseElementFromVector(std::vector<T> &vec, T element);
};  // namespace AeLib
using namespace AeLib;

#include "template_define.h"
