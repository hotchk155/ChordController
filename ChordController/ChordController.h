//#define CHORD_TYPE unsigned int
// 7654321076543210
// oooo--iiccccrrrr
//
// oooo Octave
// ii   Inversion
// cccc Chord type
// rrrr Root note

typedef unsigned int CHORD_TYPE;
#define MK_CHORD_TYPE(root, chord) (unsigned long)((root)&0x0F)| ((unsigned long)((chord)&0x0F)<<4)

enum 
{
  ROOT_MASK    = 0x000F,
  ROOT_NONE    = 0x0000,
  ROOT_C       = 0x0001,
  ROOT_CSHARP  = 0x0002,
  ROOT_D       = 0x0003,
  ROOT_DSHARP  = 0x0004,
  ROOT_E       = 0x0005,
  ROOT_F       = 0x0006,
  ROOT_FSHARP  = 0x0007,
  ROOT_G       = 0x0008,
  ROOT_GSHARP  = 0x0009,
  ROOT_A       = 0x000A,
  ROOT_ASHARP  = 0x000B,
  ROOT_B       = 0x000C
};

enum 
{
  CHORD_MASK   = 0x00F0,
  CHORD_NONE   = 0x0000,
  CHORD_MAJ    = 0x0010,
  CHORD_MAJ7   = 0x0020,
  CHORD_MIN7   = 0x0030,
  CHORD_6      = 0x0040,
  CHORD_MIN6   = 0x0050,
  CHORD_9      = 0x0060,
  CHORD_DIM    = 0x0070,
  CHORD_SUS4   = 0x0080,
  CHORD_MIN    = 0x0090,
  CHORD_7      = 0x00A0
};

enum
{
  INV_MASK     = 0x0300,
  INV_NONE     = 0x0000,
  INV_FIRST    = 0x0100,
  INV_SECOND   = 0x0200
};

enum 
{
   KEYTYPE_NONE,
   KEYTYPE_MAJOR,
   KEYTYPE_MINOR
};
