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

#define ARRAY_SIZE(c_array) sizeof c_array / sizeof c_array[0]

template <class T, int N>
struct DllExport AeVector {
    union {
        T elements[N];
        struct {
            T x, y, z, w;
        };
        struct {
            T r, g, b, a;
        };
        struct {
            T u, v;
        };
        struct {
            T width, height, depth;
        };
    };

    AeVector();
    //template <class T2>
    //AeVector(T2, T2, T2);
    AeVector(T* other);
    template <class T2, int N2>
    AeVector(const AeVector<T2, N2> &other);
    template <class T2, int N2, class T3>
    AeVector(const AeVector<T2, N2> &other, T3 value);

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
    AeVector<T, N> &operator=(const T2 &other);
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
/*
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
*/

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
    AeVector<float, 4> operator*(const AeVector<float, 4> &other);
    QeMatrix4x4f &operator/=(const float &other);
};

struct DllExport QeRay {
    AeVector<float, 3> origin;
    AeVector<float, 3> direction;
    AeVector<float, 3> positionByTime(float t);
};

struct DllExport QeRayHitRecord {
    float t;
    AeVector<float, 3> position;
    AeVector<float, 3> normal;
};

struct DllExport QeBoundingSphere {
    AeVector<float, 3> center;
    float radius;
};

struct DllExport QeBoundingBox {
    AeVector<float, 3> min;
    AeVector<float, 3> max;
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
class DllExport AeMath {
    SINGLETON_CLASS(AeMath)
   public:
    ~AeMath();

    const float PI = 3.1415927f;
    const float RADIANS_TO_DEGREES = 180.0f / PI;
    const float DEGREES_TO_RADIANS = PI / 180;

    template <class T>
    T random(T start, T range);
    template <class T, int N>
    AeVector<T, N> randoms(T start, T range);
    template <int N>
    float dot(AeVector<float, N> &vec1, AeVector<float, N> &vec2);
    template <int N>
    float length(AeVector<float, N> &vec);
    template <int N>
    AeVector<float, N> normalize(AeVector<float, N> &_vec);

    QeMatrix4x4f lookAt(AeVector<float, 3> &_pos, AeVector<float, 3> &_center, AeVector<float, 3> &_up);
    QeMatrix4x4f perspective(float _fov, float _aspect, float _near, float _far);
    QeMatrix4x4f translate(AeVector<float, 3> &_pos);
    AeVector<float, 3> move(AeVector<float, 3> &_position, AeVector<float, 3> &_addMove, AeVector<float, 3> &_face,
                            AeVector<float, 3> &_up);
    QeMatrix4x4f rotate_quaternion(AeVector<float, 3> &_eulerAngles);
    QeMatrix4x4f rotate_quaternion(AeVector<float, 4> &quaternion);
    QeMatrix4x4f rotate_quaternion(float _angle, AeVector<float, 3> &_axis);
    AeVector<float, 4> eulerAngles_to_quaternion(AeVector<float, 3> &_eulerAngles);
    AeVector<float, 4> axis_to_quaternion(float _angle, AeVector<float, 3> &_axis);
    QeMatrix4x4f rotate_eularAngles(AeVector<float, 3> &_eulerAngles);  // (roll, pitch, yaw) or (bank, attitude, heading)
    QeMatrix4x4f rotate_axis(float _angle, AeVector<float, 3> &_axis);
    QeMatrix4x4f rotateX(float _angle);
    QeMatrix4x4f rotateY(float _angle);
    QeMatrix4x4f rotateZ(float _angle);
    AeVector<float, 4> matrix_to_quaternion(QeMatrix4x4f matrix);
    QeMatrix4x4f scale(AeVector<float, 3> &_size);
    QeMatrix4x4f transform(AeVector<float, 3> &_tanslation, AeVector<float, 4> &_rotation_quaternion, AeVector<float, 3> &_scale);
    QeMatrix4x4f getTransformMatrix(AeVector<float, 3> &_translate, AeVector<float, 3> &_rotateEuler, AeVector<float, 3> &_scale,
                                    AeVector<float, 3> &camera_world_position, bool bRotate = true, bool bFixSize = false);

    AeVector<float, 3> eulerAnglesToVector(AeVector<float, 3> &_eulerAngles);
    AeVector<float, 3> vectorToEulerAngles(AeVector<float, 3> &_vector);
    AeVector<float, 3> cross(AeVector<float, 3> &_vec1, AeVector<float, 3> &_vec2);
    float fastSqrt(float _number);
    bool inverse(QeMatrix4x4f &_inMat, QeMatrix4x4f &_outMat);
    QeMatrix4x4f transpose(QeMatrix4x4f &_mat);
    int clamp(int in, int low, int high);
    float clamp(float in, float low, float high);
    AeVector<float, 4> interpolateDir(AeVector<float, 4> &a, AeVector<float, 4> &b, float blend);
    AeVector<float, 3> interpolatePos(AeVector<float, 3> &start, AeVector<float, 3> &end, float progression);
    float getAnglefromVectors(AeVector<float, 3> &v1, AeVector<float, 3> &v2);
    AeVector<float, 3> revolute_axis(AeVector<float, 3> &_position, AeVector<float, 3> &_addRevolute,
                                     AeVector<float, 3> &_centerPosition, bool bFixX = false, bool bFixY = false,
                                     bool bFixZ = false);
    AeVector<float, 3> revolute_eularAngles(AeVector<float, 3> &_position, AeVector<float, 3> &_addRevolute,
                                            AeVector<float, 3> &_centerPosition, bool bFixX, bool bFixY, bool bFixZ);

    // void getAnglefromVector(QeVector3f& inV, float & outPolarAngle, float & outAzimuthalAngle);
    // void rotatefromCenter(QeVector3f& center, QeVector3f& pos, float polarAngle, float azimuthalAngle);
    // void rotatefromCenter(QeVector3f& center, QeVector3f& pos, QeVector2f & axis, float angle, bool bStopTop);
    bool hit_test_raycast_sphere(QeRay &ray, QeBoundingSphere &sphere, float maxDistance = 0.f, QeRayHitRecord *hit = nullptr);
    void quicksort(float *data, int count);
};
#define MATH AeMath::getInstance()

enum DllExport QeAssetType {
    eAssetXML = 0,
    eAssetJSON = 1,
};

struct AeNode {
    std::string key;
    std::string value;
};

struct AeXMLNode;
struct AeXMLData {
    std::string version;
    std::vector<std::string> comments;
    std::string key;
    std::string value;
    std::vector<AeNode> elements;
    std::vector<AeXMLNode *> nexts;
    AeXMLNode *parent = nullptr;
};

struct DllExport AeXMLNode {
    AeXMLData *data;

    AeXMLNode();
    ~AeXMLNode();

    AeXMLNode *getXMLNode(const char *key);
    AeXMLNode *getXMLNode(std::vector<std::string> &keys);

    template <class T>
    T getXMLValue(const char *key);
    template <class T>
    AeXMLNode *getXMLValue(T &value, const char *key);

    template <class T, int N>
    AeVector<T, N> getXMLValues(const char *key);
    template <class T, int N>
    AeXMLNode *getXMLValues(AeVector<T, N> &value, const char *key);

    AeXMLNode *copyXMLNode();
    void copyXMLValue(AeXMLNode *to);
    void copyXMLNode(AeXMLNode *to);

    void addXMLNode(AeXMLNode *node);
    void removeXMLNode(AeXMLNode *node);

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

class DllExport AeCommonManager {
    SINGLETON_CLASS(AeCommonManager)
   public:
    ~AeCommonManager();

    QeAssetJSON *getJSON(const char *_filePath);
    AeXMLNode *getXML(const char *_filePath);
    void removeXML(std::string path);

    std::vector<char> loadFile(const char *_filePath);
};
#define CM_MGR AeCommonManager::getInstance()

class DllExport QeEncode {
    SINGLETON_CLASS(QeEncode)
   public:
    ~QeEncode() {}

    AeXMLNode *decodeXML(const char *buffer, int &index, AeXMLNode *parent = nullptr);
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

    template <class T>
    T ConvertTo(const std::string &str);

    template <class T>
    std::vector<T> split(std::string s, std::string delim);
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
