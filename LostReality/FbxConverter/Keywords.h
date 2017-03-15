#pragma once

#define CONVERTER_VERSION				"a.0"

#define USE_READABLE_KEY

#ifdef USE_READABLE_KEY

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
#define K_INTERPOLATION					"Interp"
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
#define K_NODE							"Node"

#else

#define K_META							0xfc000000
#define K_TITLE							(K_META+1)
#define K_SUBJECT						(K_TITLE+1)
#define K_AUTHOR						(K_SUBJECT+1)
#define K_KEYWORDS						(K_AUTHOR+1)
#define K_REVISION						(K_KEYWORDS+1)
#define K_COMMENT						(K_REVISION+1)

#define K_THUMBNAIL						(K_COMMENT+1)
#define K_FORMAT						(K_THUMBNAIL+1)
#define K_DIM_UNKNOW					(K_FORMAT+1)
#define K_DIM_64						(K_DIM_UNKNOW+1)
#define K_DIM_128						(K_DIM_64+1)

#define K_MARKER						(K_DIM_128+1)
#define K_SKELETON						(K_MARKER+1)
#define K_MESH							(K_SKELETON+1)
#define K_NURBS							(K_MESH+1)
#define K_PATCH							(K_NURBS+1)
#define K_CAMERA						(K_PATCH+1)
#define K_LIGHT							(K_CAMERA+1)
#define K_LODGROUP						(K_LIGHT+1)
#define K_POSE							(K_LODGROUP+1)

#define K_CONTROLPOINT					(K_POSE+1)
#define K_POLYGON						(K_CONTROLPOINT+1)
#define K_GROUPID						(K_POLYGON+1)

#define K_METADATA_CONNECTION			(K_GROUPID+1)

#define K_INDEX							(K_METADATA_CONNECTION+1)
#define K_COORDINATE					(K_INDEX+1)
#define K_NORMAL						(K_COORDINATE+1)
#define K_TANGENT						(K_NORMAL+1)
#define K_BINORMAL						(K_TANGENT+1)
#define K_NAME							(K_BINORMAL+1)
#define K_TYPE							(K_NAME+1)
#define K_LOOK							(K_TYPE+1)
#define K_SIZE							(K_LOOK+1)
#define K_RED							(K_SIZE+1)
#define K_GREEN							(K_RED+1)
#define K_BLUE							(K_GREEN+1)
#define K_RGB							(K_BLUE+1)
#define K_RGBA							(K_RGB+1)
#define K_IKPIVOT						(K_RGBA+1)
#define K_VERTEXCOLOR					(K_IKPIVOT+1)
#define K_UV							(K_VERTEXCOLOR+1)
#define K_VISIBILITY					(K_UV+1)
#define K_EDGE							(K_VISIBILITY+1)
#define K_EDGE_VISIBILITY				(K_EDGE+1)
#define K_MATRIX						(K_EDGE_VISIBILITY+1)
#define K_TRANSFORM						(K_MATRIX+1)
#define K_ROTATE_X						(K_TRANSFORM+1)
#define K_ROTATE_Y						(K_ROTATE_X+1)
#define K_ROTATE_Z						(K_ROTATE_Y+1)
#define K_TRANSLATE_X					(K_ROTATE_Z+1)
#define K_TRANSLATE_Y					(K_TRANSLATE_X+1)
#define K_TRANSLATE_Z					(K_TRANSLATE_Y+1)
#define K_SCALE_X						(K_TRANSLATE_Z+1)
#define K_SCALE_Y						(K_SCALE_X+1)
#define K_SCALE_Z						(K_SCALE_Y+1)

#define K_ROOT							(K_SCALE_Z+1)
#define K_LIMB							(K_ROOT+1)
#define K_LIMBNODE						(K_LIMB+1)
#define K_EFFECTOR						(K_LIMBNODE+1)

#define K_LENGTH						(K_EFFECTOR+1)
#define K_NODESIZE						(K_LENGTH+1)
#define K_ROOTSIZE						(K_NODESIZE+1)

#define K_MAPPINGMODE					(K_ROOTSIZE+1)
#define K_REFERENCEMODE					(K_MAPPINGMODE+1)

#define K_ISBIND						(K_REFERENCEMODE+1)
#define K_COUNT							(K_ISBIND+1)
#define K_CHARACTER_POSE				(K_COUNT+1)
#define K_ANIMATION						(K_CHARACTER_POSE+1)
#define K_STACK							(K_ANIMATION+1)
#define K_LAYER							(K_STACK+1)
#define K_LAYER2						(K_LAYER+1)
#define K_TIME							(K_LAYER2+1)
#define K_VALUE							(K_TIME+1)
#define K_CURVE							(K_VALUE+1)
#define K_CURVE_KEY						(K_CURVE+1)
#define K_CURVE_LIST					(K_CURVE_KEY+1)
#define K_KEY							(K_CURVE_LIST+1)
#define K_KEY_TIME						(K_KEY+1)
#define K_PROPERTY						(K_KEY_TIME+1)
#define K_INTERPOLATION					(K_PROPERTY+1)
#define K_CONSTANT_MODE					(K_INTERPOLATION+1)
#define K_CUBIC_MODE					(K_CONSTANT_MODE+1)
#define K_TANGENT_WEIGHT				(K_CUBIC_MODE+1)
#define K_TANGENT_VELOCITY				(K_TANGENT_WEIGHT+1)
#define K_SHAPE_CHANNEL					(K_TANGENT_VELOCITY+1)

#define K_INTENSITY						(K_SHAPE_CHANNEL+1)
#define K_OUTER_ANGLE					(K_INTENSITY+1)
#define K_FOG							(K_OUTER_ANGLE+1)
#define K_FOV							(K_FOG+1)
#define K_FOV_X							(K_FOV+1)
#define K_FOV_Y							(K_FOV_X+1)
#define K_OPTICAL_CENTERX				(K_FOV_Y+1)
#define K_OPTICAL_CENTERY				(K_OPTICAL_CENTERX+1)
#define K_ROLL							(K_OPTICAL_CENTERY+1)
#define K_YAW							(K_ROLL+1)
#define K_PITCH							(K_YAW+1)
#define K_LABEL							(K_PITCH+1)
#define K_NODE							(K_LABEL+1)

#endif

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