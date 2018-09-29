const int MAX_JOINT_NUM = 20;
const float M_PI = 3.141592653589793;

const int layoutSet_Model = 0;
const int Model_uniform = 0;
const int Model_image = 10;
const int Model_texel = 20;
const int Model_storage = 30;

const int layoutSet_Common = 1;
const int Common_uniform = 0;
const int Common_storage = 10;

const int layoutSet_Postprocessing = 2;
const int Postprocessing_buffer = 0;
const int Postprocessing_image = 10;


layout( push_constant ) uniform TimeState {
  float DeltaTime;
} pushConstant;


struct QeDataCamera {
	vec4 pos;
	mat4 view;
	mat4 projection;
};

layout( set = layoutSet_Common, binding = Common_uniform+ 0) uniform QeDataEnvironment {
	vec4 ambientColor;
	QeDataCamera camera;
	vec4 param; // 0: light num, 1: gamma, 2: exposure
} environmentData;


struct QeDataLight{
	vec4 pos;
	vec4 dir;
	vec4 color;
	vec4 param; // 1: type, 2: intensity, 3: coneAngle
};

layout( set = layoutSet_Common, binding = Common_storage + 0 ) buffer QeLight {
	QeDataLight lights[];
};


struct QeDataMaterial {
	vec4 baseColor;
	vec4 metallicRoughnessEmissive;
};

layout( set = layoutSet_Model, binding = Model_uniform + 0) uniform QeDataModel {
	mat4 model;
	mat4 joints[MAX_JOINT_NUM];
	vec4 param; // 0: bColorMap, 1: bCubeMap, 2: bNormalMap, 3: outlineWidth
	QeDataMaterial mtl;
} modelData;