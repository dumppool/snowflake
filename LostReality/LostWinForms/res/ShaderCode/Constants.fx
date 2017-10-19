#ifndef CONSTANTS_H
#define CONSTANTS_H

// macros
#define HAS_FLAGS(flags, srcFlags) ((flags & srcFlags) == flags)

// Constants,�ο�ShaderManager.h
// MAX_BONES
// LIT_MODE
// CUSTOM_BUFFER
// VERTEX_FLAGS
// VERTEX_TEXCOORD0
// VERTEX_NORMAL
// VERTEX_TANGENT
// VERTEX_COLOR
// VERTEX_SKIN
// VERTEX_TEXCOORD1
// VERTEX_COORDINATE2D

// Switches
#define VE_HAS_POS2D HAS_FLAGS(VERTEX_COORDINATE2D, VERTEX_FLAGS)
#define VE_HAS_TEXCOORD0 HAS_FLAGS(VERTEX_TEXCOORD0, VERTEX_FLAGS)
#define VE_HAS_TEXCOORD1 HAS_FLAGS(VERTEX_TEXCOORD1, VERTEX_FLAGS)
#define VE_HAS_NORMAL HAS_FLAGS(VERTEX_NORMAL, VERTEX_FLAGS)
#define VE_HAS_TANGENT HAS_FLAGS(VERTEX_TANGENT, VERTEX_FLAGS)
#define VE_HAS_COLOR HAS_FLAGS(VERTEX_COLOR, VERTEX_FLAGS)
#define VE_HAS_SKIN HAS_FLAGS(VERTEX_SKIN, VERTEX_FLAGS)

#endif //CONSTANTS_H