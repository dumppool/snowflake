#pragma once

#define CONVERTER_VERSION				"a.0"

#define K_META							"MetaData"
#define K_TITLE							"Title"
#define K_SUBJECT						"Subject"
#define K_AUTHOR						"Author"
#define K_KEYWORDS						"Keywords"
#define K_REVISION						"Revision"
#define K_COMMENT						"Comment"

#define K_THUMBNAIL						"Thumbnail"
#define K_FORMAT						"Format"
#define K_DIM_UNKNOW					"DimUnknown"
#define K_DIM_64						"Dim64"
#define K_DIM_128						"Dim128"

#define K_MARKER						"Marker"
#define K_SKELETON						"Skeleton"
#define K_MESH							"Mesh"
#define K_NURBS							"Nurbs"
#define K_PATCH							"Patch"
#define K_CAMERA						"Camera"
#define K_LIGHT							"Light"
#define K_LODGROUP						"LODGroup"
#define K_POSE							"Pose"

#define K_CONTROLPOINT					"ControlPoint"
#define K_POLYGON						"Polygon"
#define K_GROUPID						"GroupID"

#define K_METADATA_CONNECTION			"MetaDataConnection"

#define K_INDEX							"Index"
#define K_COORDINATE					"Coordinate"
#define K_NORMAL						"Normal"
#define K_TANGENT						"Tangent"
#define K_BINORMAL						"Binormal"
#define K_NAME							"Name"
#define K_TYPE							"Type"
#define K_LOOK							"Look"
#define K_SIZE							"Size"
#define K_RED							"Red"
#define K_GREEN							"Green"
#define K_BLUE							"Blue"
#define K_RGB							"RGB"
#define K_RGBA							"RGBA"
#define K_IKPIVOT						"IKPivot"
#define K_VERTEXCOLOR					"VertexColor"
#define K_UV							"UV"
#define K_VISIBILITY					"Visibility"
#define K_EDGE							"Edge"
#define K_EDGE_VISIBILITY				"EdgeVisibility"
#define K_MATRIX						"Matrix"
#define K_TRANSFORM						"Transform"
#define K_ROTATE_X						"RX"
#define K_ROTATE_Y						"RY"
#define K_ROTATE_Z						"RZ"
#define K_TRANSLATE_X					"TX"
#define K_TRANSLATE_Y					"TY"
#define K_TRANSLATE_Z					"TZ"
#define K_SCALE_X						"SX"
#define K_SCALE_Y						"SY"
#define K_SCALE_Z						"SZ"

#define K_ROOT							"Root"
#define K_LIMB							"Limb"
#define K_LIMBNODE						"LimbNode"
#define K_EFFECTOR						"Effector"

#define K_LENGTH						"Length"
#define K_NODESIZE						"NodeSize"
#define K_ROOTSIZE						"RootSize"

#define K_MAPPINGMODE					"MappingMode"
#define K_REFERENCEMODE					"ReferenceMode"

#define K_ISBIND						"IsBind"
#define K_COUNT							"Count"
#define K_CHARACTER_POSE				"CharacterPose"
#define K_ANIMATION						"Animation"
#define K_STACK							"Stack"
#define K_LAYER							"Layer"
#define K_LAYER2						"Layer2"
#define K_TIME							"Time"
#define K_VALUE							"Value"
#define K_CURVE							"Curve"
#define K_CURVE_KEY						"CurveKey"
#define K_CURVE_LIST					"KeyList"
#define K_KEY							"Key"
#define K_KEY_TIME						"KeyTime"
#define K_PROPERTY						"Property"
#define K_INTERPOLATION					"Interpolation"
#define K_CONSTANT_MODE					"ConstantMode"
#define K_CUBIC_MODE					"CubicMode"
#define K_TANGENT_WEIGHT				"TangentWeight"
#define K_TANGENT_VELOCITY				"TangentVelocity"
#define K_SHAPE_CHANNEL					"ShapeChannel"

#define K_INTENSITY						"Intensity"
#define K_OUTER_ANGLE					"OuterAngle"
#define K_FOG							"Fog"
#define K_FOV							"Fov"
#define K_FOV_X							"FovX"
#define K_FOV_Y							"FovY"
#define K_OPTICAL_CENTERX				"OpticalCenterX"
#define K_OPTICAL_CENTERY				"OpticalCenterY"
#define K_ROLL							"Roll"
#define K_YAW							"Yaw"
#define K_PITCH							"Pitch"
#define K_LABEL							"Label"

#define FLAG_UNKNOWN					0x0badfffa

enum class EThumbnailFormat
{
	RGB,
	RGBA
};

enum class EMarkerType
{
	Standard,
	Optical,
	IK_Effector,
	FK_Effector,
};

enum class EMarkerLook
{
	Cube,
	Sphere,
	HardCross,
	LightCross,
};

enum class EMappingMode
{
	None,
	ByControlPoint,
	ByPolygonVertex,
	ByPolygon,
	ByEdge,
	AllSame,
};

enum class EReferenceMode
{
	Direct,
	Index,
	IndexToDirect
};

enum class EInterpolation
{
	Constant,
	Linear,
	Cubic,
};

enum class EConstantMode
{
	Standard,
	Next,
};

enum class ECubicMode
{
	Auto,
	AutoBreak,
	Tcb,
	User,
	Break,
	UserBreak,
};

enum ETangentWVMode
{
	None,
	Right,
	NextLeft,
};