#include "qeheader.h"

float QeMath::dot(QeVector3f _vec1, QeVector3f _vec2) { return _vec1.x*_vec2.x + _vec1.y*_vec2.y + _vec1.z*_vec2.z; }
float QeMath::dot(QeVector4f _vec1, QeVector4f _vec2) { return _vec1.x*_vec2.x + _vec1.y*_vec2.y + _vec1.z*_vec2.z + _vec1.w*_vec2.w; }

QeVector3f QeMath::cross(QeVector3f _vec1, QeVector3f _vec2) {

	QeVector3f _rtn;
	_rtn.x = _vec1.y * _vec2.z - _vec1.z * _vec2.y;
	_rtn.y = _vec1.z * _vec2.x - _vec1.x * _vec2.z;
	_rtn.z = _vec1.x * _vec2.y - _vec1.y * _vec2.x;
	return _rtn;
}

QeVector3f QeMath::normalize(QeVector3f _vec) { return _vec / length(_vec); }
QeVector4f QeMath::normalize(QeVector4f _vec) { return _vec / length(_vec); }

float QeMath::distance(QeVector3f _from, QeVector3f _to) { return length(_to - _from); }

float QeMath::length(QeVector3f _vec) { return fastSqrt(dot(_vec, _vec)); }
float QeMath::length(QeVector4f _vec) { return fastSqrt(dot(_vec, _vec)); }

float QeMath::fastSqrt(float _number) {

//#ifdef FAST_SQRT_QUAKE3
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = _number * 0.5F;
	y = _number;
	i = *(long *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y));
	return 1.0f/y;
//#else
//	return sqrtf(_number);
//#endif
}


QeMatrix4x4f QeMath::lookAt(QeVector3f _pos, QeVector3f _target, QeVector3f _up) {

	QeMatrix4x4f _rtn;
	QeVector3f _face = normalize(_target - _pos);
	QeVector3f _surface = normalize(cross(_face, _up));
	QeVector3f _up1 = cross(_surface, _face);

	_rtn._00 = _surface.x;
	_rtn._10 = _surface.y;
	_rtn._20 = _surface.z;

	_rtn._01 = _up1.x;
	_rtn._11 = _up1.y;
	_rtn._21 = _up1.z;

	_rtn._02 = -_face.x;
	_rtn._12 = -_face.y;
	_rtn._22 = -_face.z;

	_rtn._30 = -dot(_surface, _pos);
	_rtn._31 = -dot(_up1, _pos);
	_rtn._32 = dot(_face, _pos);
	return _rtn;
}

QeMatrix4x4f QeMath::perspective(float _fov, float _aspect, float _near, float _far) {

	QeMatrix4x4f _rtn;
	float _fovR = _fov*DEGREES_TO_RADIANS;
	float tanHalfFov = tan(_fovR / 2);
	_rtn._00 = 1.0f / (tanHalfFov*_aspect);
	_rtn._11 = -1.0f / tanHalfFov;
	_rtn._22 = _far / (_near - _far);
	_rtn._32 = _far*_near / (_near - _far);
	//_rtn._22 = ( _far+_near ) / (_near - _far);
	//_rtn._32 = _far*_near*2 / (_near - _far);
	_rtn._23 = -1.0f;
	return _rtn;
}

QeMatrix4x4f QeMath::translate(QeVector3f _pos) {

	QeMatrix4x4f _rtn;
	_rtn._30 = _pos.x;
	_rtn._31 = _pos.y;
	_rtn._32 = _pos.z;
	return _rtn;
}

QeMatrix4x4f QeMath::rotate(float _angle, QeVector3f _axis) {

	QeMatrix4x4f _rtn;
	float _radian = _angle*DEGREES_TO_RADIANS;
	float _cosA = cos(_radian);
	float _sinA = sin(_radian);
	_axis = normalize(_axis);
	QeVector3f _temp(_axis*(1.0f - _cosA));

	_rtn._00 = _cosA + _temp.x*_axis.x;
	_rtn._01 = _temp.x*_axis.y + _sinA*_axis.z;
	_rtn._02 = _temp.x*_axis.z - _sinA*_axis.y;

	_rtn._10 = _temp.y*_axis.x - _sinA*_axis.z;
	_rtn._11 = _cosA + _temp.y*_axis.y;
	_rtn._12 = _temp.y*_axis.z + _sinA*_axis.x;

	_rtn._20 = _temp.z*_axis.x + _sinA*_axis.y;
	_rtn._21 = _temp.z*_axis.y - _sinA*_axis.x;
	_rtn._22 = _cosA + _temp.z*_axis.z;

	return _rtn;
}

QeMatrix4x4f QeMath::rotateX(float _angle) {

	QeMatrix4x4f _rtn;
	float _radian = _angle*DEGREES_TO_RADIANS;
	float _cosA = cos(_radian);
	float _sinA = sin(_radian);
	_rtn._11 = _cosA;
	_rtn._12 = _sinA;
	_rtn._21 = -_sinA;
	_rtn._22 = _cosA;
	return _rtn;

}

QeMatrix4x4f QeMath::rotateY(float _angle) {

	QeMatrix4x4f _rtn;
	float _radian = _angle*DEGREES_TO_RADIANS;
	float _cosA = cos(_radian);
	float _sinA = sin(_radian);
	_rtn._00 = _cosA;
	_rtn._02 = -_sinA;
	_rtn._20 = _sinA;
	_rtn._22 = _cosA;
	return _rtn;
}

QeMatrix4x4f QeMath::rotateZ(float _angle) {

	QeMatrix4x4f _rtn;
	float _radian = _angle*DEGREES_TO_RADIANS;
	float _cosA = cos(_radian);
	float _sinA = sin(_radian);
	_rtn._00 = _cosA;
	_rtn._01 = _sinA;
	_rtn._10 = -_sinA;
	_rtn._11 = _cosA;
	return _rtn;
}

QeMatrix4x4f QeMath::scale(QeVector3f _size) {

	QeMatrix4x4f _rtn;
	_rtn._00 = _size.x;
	_rtn._11 = _size.y;
	_rtn._22 = _size.z;
	return _rtn;
}

QeVector2i::QeVector2i() :x(0), y(0) {}
QeVector2i::QeVector2i(int _x, int _y) :x(_x), y(_y) {}

QeVector2f::QeVector2f() :x(0.0f), y(0.0f) {}
QeVector2f::QeVector2f(float _x, float _y) :x(_x), y(_y) {}

bool QeVector2f::operator==(const QeVector2f& other) const {
	return x == other.x && y == other.y;
}

QeVector2f& QeVector2f::operator+=(const QeVector2f& other) {
	x += other.x;
	y += other.y;
	return *this;
}
QeVector2f& QeVector2f::operator-=(const QeVector2f& other) {
	x -= other.x;
	y -= other.y;
	return *this;
}
QeVector2f& QeVector2f::operator/=(const float& other) {
	x /= other;
	y /= other;
	return *this;
}
QeVector2f QeVector2f::operator/(const float& other) {
	QeVector2f _new;
	_new.x = x / other;
	_new.y = y / other;
	return _new;
}
QeVector2f QeVector2f::operator*(const float& other) {
	QeVector2f _new;
	_new.x = x * other;
	_new.y = y * other;
	return _new;
}

QeVector3i::QeVector3i() :x(0), y(0), z(0) {}
QeVector3i::QeVector3i(int _x, int _y, int _z) :x(_x), y(_y), z(_z) {}

bool QeVector3i::operator==(const QeVector3i& other) const {
	return x == other.x && y == other.y && z == other.z;
}

QeVector3i& QeVector3i::operator+=(const QeVector3i& other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}
QeVector3i& QeVector3i::operator-=(const QeVector3i& other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}
QeVector3i& QeVector3i::operator-=(const int& other) {
	x -= other;
	y -= other;
	z -= other;
	return *this;
}
QeVector3i& QeVector3i::operator/=(const int& other) {
	x /= other;
	y /= other;
	z /= other;
	return *this;
}
QeVector3i QeVector3i::operator-(const QeVector3i& other) {
	QeVector3i _new;
	_new.x = x - other.x;
	_new.y = y - other.y;
	_new.z = z - other.z;
	return _new;
}
QeVector3i QeVector3i::operator/(const int& other) {
	QeVector3i _new;
	_new.x = x / other;
	_new.y = y / other;
	_new.z = z / other;
	return _new;
}
QeVector3i QeVector3i::operator*(const int& other) {
	QeVector3i _new;
	_new.x = x * other;
	_new.y = y * other;
	_new.z = z * other;
	return _new;
}

QeVector3f::QeVector3f() :x(0.0f), y(0.0f), z(0.0f) {}
QeVector3f::QeVector3f(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {}

bool QeVector3f::operator==(const QeVector3f& other) const {
	return x == other.x && y == other.y && z == other.z;
}

bool QeVector3f::operator!=(const QeVector3f& other) const {
	return !(*this == other);
}

QeVector3f& QeVector3f::operator=(const QeVector4f& other) {
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}
QeVector3f& QeVector3f::operator+=(const QeVector3f& other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}
QeVector3f& QeVector3f::operator-=(const QeVector3f& other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}
QeVector3f& QeVector3f::operator-=(const float& other) {
	x -= other;
	y -= other;
	z -= other;
	return *this;
}
QeVector3f& QeVector3f::operator/=(const float& other) {
	x /= other;
	y /= other;
	z /= other;
	return *this;
}
QeVector3f& QeVector3f::operator*=(const float& other) {
	x *= other;
	y *= other;
	z *= other;
	return *this;
}
QeVector3f QeVector3f::operator+(const QeVector3f& other) {
	QeVector3f _new;
	_new.x = x + other.x;
	_new.y = y + other.y;
	_new.z = z + other.z;
	return _new;
}
QeVector3f QeVector3f::operator-(const QeVector3f& other) {
	QeVector3f _new;
	_new.x = x - other.x;
	_new.y = y - other.y;
	_new.z = z - other.z;
	return _new;
}
QeVector3f QeVector3f::operator/(const float& other) {
	QeVector3f _new;
	_new.x = x / other;
	_new.y = y / other;
	_new.z = z / other;
	return _new;
}
QeVector3f QeVector3f::operator*(const float& other) {
	QeVector3f _new;
	_new.x = x * other;
	_new.y = y * other;
	_new.z = z * other;
	return _new;
}
QeVector4s::QeVector4s() :x(0), y(0), z(0), w(0) {}
QeVector4i::QeVector4i() : x(0), y(0), z(0), w(0) {}
QeVector4f::QeVector4f() :x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
QeVector4f::QeVector4f(float _x, float _y, float _z, float _w) :x(_x), y(_y), z(_z), w(_w) {}
QeVector4f::QeVector4f(const QeVector3f& other, float _w) :x(other.x), y(other.y), z(other.z), w(_w) {}

bool QeVector4f::operator==(const QeVector4f& other) const {
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool QeVector4f::operator!=(const QeVector4f& other) const {
	return !(*this == other);
}

QeVector4f& QeVector4f::operator=(const QeVector3f& other) {
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}
QeVector4f& QeVector4f::operator=(const QeVector4s& other) {
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}
QeVector4i& QeVector4i::operator=(const QeVector4s& other) {
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}
QeVector4f& QeVector4f::operator+=(const QeVector3f& other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}
QeVector4f QeVector4f::operator+(const QeVector3f& other) {
	QeVector4f _new;
	_new.x = x + other.x;
	_new.y = y + other.y;
	_new.z = z + other.z;
	_new.z = z;
	return _new;
}
QeVector4f QeVector4f::operator-(const QeVector3f& other) {
	QeVector4f _new;
	_new.x = x - other.x;
	_new.y = y - other.y;
	_new.z = z - other.z;
	_new.z = z;
	return _new;
}
QeVector4f QeVector4f::operator/(const float& other) {
	QeVector4f _new;
	_new.x = x / other;
	_new.y = y / other;
	_new.z = z / other;
	_new.w = w / other;
	return _new;
}


QeMatrix4x4f::QeMatrix4x4f() :_00(1.0f), _01(0.0f), _02(0.0f), _03(0.0f),
		_10(0.0f), _11(1.0f), _12(0.0f), _13(0.0f),
		_20(0.0f), _21(0.0f), _22(1.0f), _23(0.0f),
		_30(0.0f), _31(0.0f), _32(0.0f), _33(1.0f) {}
QeMatrix4x4f::QeMatrix4x4f(float _num) :_00(_num), _01(_num), _02(_num), _03(_num),
		_10(_num), _11(_num), _12(_num), _13(_num),
		_20(_num), _21(_num), _22(_num), _23(_num),
		_30(_num), _31(_num), _32(_num), _33(_num) {}
QeMatrix4x4f::QeMatrix4x4f(float __00, float __01, float __02, float __03, 
	float __10, float __11, float __12, float __13, 
	float __20, float __21, float __22, float __23, 
	float __30, float __31, float __32, float __33) :
	_00(__00), _01(__01), _02(__02), _03(__03),
	_10(__10), _11(__11), _12(__12), _13(__13),
	_20(__20), _21(__21), _22(__22), _23(__23),
	_30(__30), _31(__31), _32(__32), _33(__33)
{}

QeMatrix4x4f& QeMatrix4x4f::operator*=(const QeMatrix4x4f& other) {

	*this = *this*other;
	return *this;
}
QeMatrix4x4f QeMatrix4x4f::operator*(const QeMatrix4x4f& other) {
	QeMatrix4x4f _new(0);
	for (int i = 0; i<4; i++)
		for (int j = 0; j<4; j++)
			for (int k = 0; k < 4; k++) 
				((float *)&_new)[i * 4 + j] += (((float *)this)[k * 4 + j] * ((float *)&other)[i * 4 + k]);
	return _new;
}
QeVector4f QeMatrix4x4f::operator*(const QeVector4f& other) {
	QeVector4f _new;
	for (int i = 0; i<4; i++)
		for (int j = 0; j < 4; j++) ((float *)&_new)[i] += (((float *)this)[j * 4 + i] * ((float *)&other)[j]);

	return _new;
}
QeMatrix4x4f& QeMatrix4x4f::operator/=(const float& other) {

	for (int i = 0; i<16; i++)	((float *)this)[i] /= other;
	return *this;
}
bool QeMath::inverse(QeMatrix4x4f _inMat, QeMatrix4x4f& _outMat) {

	QeMatrix4x4f _new(0);

	_new._00 =  _inMat._11 * _inMat._22 * _inMat._33 -
				_inMat._11 * _inMat._23 * _inMat._32 -
				_inMat._21 * _inMat._12 * _inMat._33 +
				_inMat._21 * _inMat._13 * _inMat._32 +
				_inMat._31 * _inMat._12 * _inMat._23 -
				_inMat._31 * _inMat._13 * _inMat._22;

	_new._10 = -_inMat._10 * _inMat._22 * _inMat._33 +
				_inMat._10 * _inMat._23 * _inMat._32 +
				_inMat._20 * _inMat._12 * _inMat._33 -
				_inMat._20 * _inMat._13 * _inMat._32 -
				_inMat._30 * _inMat._12 * _inMat._23 +
				_inMat._30 * _inMat._13 * _inMat._22;

	_new._20 =  _inMat._10 * _inMat._21 * _inMat._33 -
				_inMat._10 * _inMat._23 * _inMat._31 -
				_inMat._20 * _inMat._11 * _inMat._33 +
				_inMat._20 * _inMat._13 * _inMat._31 +
				_inMat._30 * _inMat._11 * _inMat._23 -
				_inMat._30 * _inMat._13 * _inMat._21;

	_new._30 = -_inMat._10 * _inMat._21 * _inMat._32 +
				_inMat._10 * _inMat._22 * _inMat._31 +
				_inMat._20 * _inMat._11 * _inMat._32 -
				_inMat._20 * _inMat._12 * _inMat._31 -
				_inMat._30 * _inMat._11 * _inMat._22 +
				_inMat._30 * _inMat._12 * _inMat._21;

	_new._01 = -_inMat._01 * _inMat._22 * _inMat._33 +
				_inMat._01 * _inMat._23 * _inMat._32 +
				_inMat._21 * _inMat._02 * _inMat._33 -
				_inMat._21 * _inMat._03 * _inMat._32 -
				_inMat._31 * _inMat._02 * _inMat._23 +
				_inMat._31 * _inMat._03 * _inMat._22;

	_new._11 =  _inMat._00 * _inMat._22 * _inMat._33 -
				_inMat._00 * _inMat._23 * _inMat._32 -
				_inMat._20 * _inMat._02 * _inMat._33 +
				_inMat._20 * _inMat._03 * _inMat._32 +
				_inMat._30 * _inMat._02 * _inMat._23 -
				_inMat._30 * _inMat._03 * _inMat._22;

	_new._21 = -_inMat._00 * _inMat._21 * _inMat._33 +
				_inMat._00 * _inMat._23 * _inMat._31 +
				_inMat._20 * _inMat._01 * _inMat._33 -
				_inMat._20 * _inMat._03 * _inMat._31 -
				_inMat._30 * _inMat._01 * _inMat._23 +
				_inMat._30 * _inMat._03 * _inMat._21;

	_new._31 =	_inMat._00 * _inMat._21 * _inMat._32 -
				_inMat._00 * _inMat._22 * _inMat._31 -
				_inMat._20 * _inMat._01 * _inMat._32 +
				_inMat._20 * _inMat._02 * _inMat._31 +
				_inMat._30 * _inMat._01 * _inMat._22 -
				_inMat._30 * _inMat._02 * _inMat._21;

	_new._02 =  _inMat._01 * _inMat._12 * _inMat._33 -
				_inMat._01 * _inMat._13 * _inMat._32 -
				_inMat._11 * _inMat._02 * _inMat._33 +
				_inMat._11 * _inMat._03 * _inMat._32 +
				_inMat._31 * _inMat._02 * _inMat._13 -
				_inMat._31 * _inMat._03 * _inMat._12;

	_new._12 = -_inMat._00 * _inMat._12 * _inMat._33 +
				_inMat._00 * _inMat._13 * _inMat._32 +
				_inMat._10 * _inMat._02 * _inMat._33 -
				_inMat._10 * _inMat._03 * _inMat._32 -
				_inMat._30 * _inMat._02 * _inMat._13 +
				_inMat._30 * _inMat._03 * _inMat._12;

	_new._22 =  _inMat._00 * _inMat._11 * _inMat._33 -
				_inMat._00 * _inMat._13 * _inMat._31 -
				_inMat._10 * _inMat._01 * _inMat._33 +
				_inMat._10 * _inMat._03 * _inMat._31 +
				_inMat._30 * _inMat._01 * _inMat._13 -
				_inMat._30 * _inMat._03 * _inMat._11;

	_new._32 = -_inMat._00 * _inMat._11 * _inMat._32 +
				_inMat._00 * _inMat._12 * _inMat._31 +
				_inMat._10 * _inMat._01 * _inMat._32 -
				_inMat._10 * _inMat._02 * _inMat._31 -
				_inMat._30 * _inMat._01 * _inMat._12 +
				_inMat._30 * _inMat._02 * _inMat._11;

	_new._03 = -_inMat._01 * _inMat._12 * _inMat._23 +
				_inMat._01 * _inMat._13 * _inMat._22 +
				_inMat._11 * _inMat._02 * _inMat._23 -
				_inMat._11 * _inMat._03 * _inMat._22 -
				_inMat._21 * _inMat._02 * _inMat._13 +
				_inMat._21 * _inMat._03 * _inMat._12;

	_new._13 =  _inMat._00 * _inMat._12 * _inMat._23 -
				_inMat._00 * _inMat._13 * _inMat._22 -
				_inMat._10 * _inMat._02 * _inMat._23 +
				_inMat._10 * _inMat._03 * _inMat._22 +
				_inMat._20 * _inMat._02 * _inMat._13 -
				_inMat._20 * _inMat._03 * _inMat._12;

	_new._23 = -_inMat._00 * _inMat._11 * _inMat._23 +
				_inMat._00 * _inMat._13 * _inMat._21 +
				_inMat._10 * _inMat._01 * _inMat._23 -
				_inMat._10 * _inMat._03 * _inMat._21 -
				_inMat._20 * _inMat._01 * _inMat._13 +
				_inMat._20 * _inMat._03 * _inMat._11;

	_new._33 =  _inMat._00 * _inMat._11 * _inMat._22 -
				_inMat._00 * _inMat._12 * _inMat._21 -
				_inMat._10 * _inMat._01 * _inMat._22 +
				_inMat._10 * _inMat._02 * _inMat._21 +
				_inMat._20 * _inMat._01 * _inMat._12 -
				_inMat._20 * _inMat._02 * _inMat._11;

	float _det = _inMat._00 * _new._00 + _inMat._01 * _new._10 + _inMat._02 * _new._20 + _inMat._03 * _new._30;

	if (_det == 0)	return false;

	_new /= _det;
	_outMat = _new;
	return true;
}

QeMatrix4x4f QeMath::transpose(QeMatrix4x4f _mat) {
	
	QeMatrix4x4f _new(0);
	_new._00 = _mat._00;
	_new._01 = _mat._10;
	_new._02 = _mat._20;
	_new._03 = _mat._30;

	_new._10 = _mat._01;
	_new._11 = _mat._11;
	_new._12 = _mat._21;
	_new._13 = _mat._31;

	_new._20 = _mat._02;
	_new._21 = _mat._12;
	_new._22 = _mat._22;
	_new._23 = _mat._32;

	_new._30 = _mat._03;
	_new._31 = _mat._13;
	_new._32 = _mat._23;
	_new._33 = _mat._33;
	return _new;
}

int QeMath::clamp(int in, int low, int high) {	return in<low ? low : in > high ? high : in;	}

QeMatrix4x4f QeMath::vectortoRoateMatrix(QeVector4f vector) {
	QeMatrix4x4f matrix;
	float xy = vector.x * vector.y;
	float xz = vector.x * vector.z;
	float xw = vector.x * vector.w;
	float yz = vector.y * vector.z;
	float yw = vector.y * vector.w;
	float zw = vector.z * vector.w;
	float xSquared = vector.x * vector.x;
	float ySquared = vector.y * vector.y;
	float zSquared = vector.z * vector.z;
	matrix._00 = 1 - 2 * (ySquared + zSquared);
	matrix._01 = 2 * (xy - zw);
	matrix._02 = 2 * (xz + yw);
	matrix._03 = 0;
	matrix._10 = 2 * (xy + zw);
	matrix._11 = 1 - 2 * (xSquared + zSquared);
	matrix._12 = 2 * (yz - xw);
	matrix._13 = 0;
	matrix._20 = 2 * (xz - yw);
	matrix._21 = 2 * (yz + xw);
	matrix._22 = 1 - 2 * (xSquared + ySquared);
	matrix._23 = 0;
	matrix._30 = 0;
	matrix._31 = 0;
	matrix._32 = 0;
	matrix._33 = 1;
	return matrix;
}

QeVector4f QeMath::rotateMatrixtoVector(QeMatrix4x4f matrix) {
	QeVector4f ret;
	float diagonal = matrix._00 + matrix._11 + matrix._22;
	if (diagonal > 0) {
		float w4 = fastSqrt(diagonal + 1) * 2;
		ret.w = w4 / 4;
		ret.x = (matrix._21 - matrix._12) / w4;
		ret.y = (matrix._02 - matrix._20) / w4;
		ret.z = (matrix._10 - matrix._01) / w4;
	}
	else if ((matrix._00 > matrix._11) && (matrix._00 > matrix._22)) {
		float x4 = fastSqrt(1 + matrix._00 - matrix._11 - matrix._22) * 2;
		ret.w = (matrix._21 - matrix._12) / x4;
		ret.x = x4 / 4;
		ret.y = (matrix._01 + matrix._10) / x4;
		ret.z = (matrix._02 + matrix._20) / x4;
	}
	else if (matrix._11 > matrix._22) {
		float y4 = fastSqrt(1 + matrix._11 - matrix._00 - matrix._22) * 2;
		ret.w = (matrix._02 - matrix._20) / y4;
		ret.x = (matrix._01 + matrix._10) / y4;
		ret.y = y4 / 4;
		ret.z = (matrix._12 + matrix._21) / y4;
	}
	else {
		float z4 =  fastSqrt(1 + matrix._22 - matrix._00 - matrix._11) * 2;
		ret.w = (matrix._10 - matrix._01) / z4;
		ret.x = (matrix._02 + matrix._20) / z4;
		ret.y = (matrix._12 + matrix._21) / z4;
		ret.z = z4 / 4;
	}
	return ret;
}

QeVector4f QeMath::interpolateDir(QeVector4f a, QeVector4f b, float blend) {
	QeVector4f ret;

	float dot = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
	float blend1 = 1.f - blend;
	if (dot < 0) {
		ret.w = blend1 * a.w + blend * -b.w;
		ret.x = blend1 * a.x + blend * -b.x;
		ret.y = blend1 * a.y + blend * -b.y;
		ret.z = blend1 * a.z + blend * -b.z;
	}
	else {
		ret.w = blend1 * a.w + blend * b.w;
		ret.x = blend1 * a.x + blend * b.x;
		ret.y = blend1 * a.y + blend * b.y;
		ret.z = blend1 * a.z + blend * b.z;
	}
	normalize(ret);
	return ret;
}

QeVector3f QeMath::interpolatePos(QeVector3f start, QeVector3f end, float progression) {

	QeVector3f ret;
	ret.x = start.x + (end.x - start.x) * progression;
	ret.y = start.y + (end.y - start.y) * progression;
	ret.z = start.z + (end.z - start.z) * progression;
	return ret;
}

QeMatrix4x4f QeMath::transform(QeVector3f tanslation, QeVector4f rotation, QeVector3f scale) {
	
	QeMatrix4x4f ret;
	ret *= MATH->translate(tanslation);
	ret *= MATH->vectortoRoateMatrix(rotation);
	ret *= MATH->scale(scale);
	return ret;
}
