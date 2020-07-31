// DONOT modify this file
// This is auto-generated by TableGen
#ifndef CCE_AICORE_INTRINSICS_TYPES
#define CCE_AICORE_INTRINSICS_TYPES
typedef enum {

// CONV_RELU is only valid for the data moved between L0C/L0C16/L0C32/L0CSC16/L0CSC32/L0CDPF16/L0CDPF32 and UB
  CRMODE_NONE = 0,
  CRMODE_F32toF16_NONE = 1,
  CRMODE_F32toF16_RELU = 2,
  CRMODE_S32toF16_NONE = 3,
  CRMODE_F16toF32_NONE = 4,
  CRMODE_NONE_RELU     = 5,
  CRMODE_F16_MUL       = 6,
  CRMODE_S32toF16_DEQSCALE_SPR = 7,
  CRMODE_DEQSCALE_VDEQ8 = 8,
  CRMODE_DEQSCALE_DEQ8 = 9,
  CRMODE_DEQSCALE_VDEQ16 = 10,
  CRMODE_DEQSCALE_DEQ16 = 11,
  CRMODE_DEQSCALE_VDEQS16 = 12,
  CRMODE_DEQSCALE_DEQS16 = 13,

} ConvRelu_t;

typedef enum {

  inc = 0,
  dec = 1,

} addr_cal_mode_t;

typedef enum {

  SINGLE_CACHE_LINE = 0, // DCCI single cacheline entry in the data cache
  ENTIRE_DATA_CACHE,      // DCCI the entire data cache

} cache_line_t;

typedef enum {

  CSIZE0 = 0,
  CSIZE1 = 1,

} csize_t;

typedef enum {

  e0 = 0,
  e2 = 2,
  e4 = 4,
  e6 = 6,

} even0_7_t;

typedef enum {

  EVENT_ID0 = 0,
  EVENT_ID1,
  EVENT_ID2,
  EVENT_ID3,
#if __CCE_AICORE__ == 200 // Only available for V200
  EVENT_ID4,
  EVENT_ID5,
  EVENT_ID6,
  EVENT_ID7,
#endif

} event_t;

typedef enum {

// PAD MODE is only valid for OUT->L1 data path
  PAD_NONE  = 0,
  PAD_MODE1 = 1,
  PAD_MODE2 = 2,
  PAD_MODE3 = 3,
  PAD_MODE4 = 4,
  PAD_MODE5 = 5,

} pad_t;

typedef enum {

  PIPE_S = 0,  // Scalar Pipe
  PIPE_V,      // Vector Pipe, including{VectorOP write UB,  L0C->UB write}
  PIPE_M,      // Matrix Pipe, including{}
  PIPE_MTE1,   // L1->L0{A,B}
  PIPE_MTE2,   // OUT ->{L1, L0{A,B}, UB}
  PIPE_MTE3,   // UB ->{OUT,L1}
  PIPE_ALL,
#if __CCE_AICORE__ == 200 // Only available for V200
  PIPE_MTE4,   // MOV_UB_TO_OUT
  PIPE_MTE5,   // MOV_OUT_TO_UB
#ifndef __DAV_M200_VEC__
  PIPE_V2,     // Lower priority vector pipe,
               // includes data move between L0C and UB
#endif
#endif

} pipe_t;

typedef enum {

  VA0 = 0,
  VA1,
  VA2,
  VA3,
  VA4,
  VA5,
  VA6,
  VA7,

} ub_addr8_t;

typedef enum {

  L128 = 0,
  H128,

} vpart_t;

typedef enum {

  b8  = 0,  // These values are also used as encoding in backend
  b16 = 1,
  b32 = 2,
  s8  = 3,
  s32 = 4,
  f16 = 5,
  fmix = 6,
  f32 = 7,

} vtype_t;

#endif

