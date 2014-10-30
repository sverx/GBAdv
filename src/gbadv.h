//---------------------------------------------------------------------------------
#ifndef	_gbadv_h_
#define	_gbadv_h_
//---------------------------------------------------------------------------------

#include <gba.h>

typedef union OAMTable {
	OBJATTR oamBuffer[128];
	OBJAFFINE matrixBuffer[32];
} OAMTable;

// *****************************************************
// ****         Video Modes & Backgrounds          ***** 
// *****************************************************
//
// The video modes
#define DISPLAY_4TEXT                           MODE_0   // this is ZERO
#define DISPLAY_2TEXT1AFFINE                    MODE_1
#define DISPLAY_2AFFINE                         MODE_2
#define DISPLAY_1BITMAP15BPP_1FRAME             MODE_3
#define DISPLAY_1BITMAP8BPP_2FRAMES             MODE_4
#define DISPLAY_1SMALLBITMAP15BPP_2FRAMES       MODE_5

// The object mapping modes
#define OBJECT_MAPPING_1D           1
#define OBJECT_MAPPING_2D           0

void initDisplay(unsigned int mode, int boolean_object_mapping_style_1D);

// the showBackgrounds/toggleBackgrounds flags
#define BACKGROUND_NONE             0
#define BACKGROUND_0                BIT(8)
#define BACKGROUND_1                BIT(9)
#define BACKGROUND_2                BIT(10)
#define BACKGROUND_3                BIT(11)

void showBackgrounds(unsigned int which);
void toggleBackgrounds(unsigned int show, unsigned int hide);
void toggleBackgroundsOn(unsigned int show);
void toggleBackgroundsOff(unsigned int hide);

void showSprites(int boolean_active);

// the text BG sizes
#define BACKGROUND_TEXT_32x32   BG_SIZE_0           // this is ZERO
#define BACKGROUND_TEXT_64x32   BG_SIZE_1
#define BACKGROUND_TEXT_32x64   BG_SIZE_2
#define BACKGROUND_TEXT_64x64   BG_SIZE_3

// the affine BG sizes
#define BACKGROUND_AFFINE_16x16   BG_SIZE_0         // this is ZERO
#define BACKGROUND_AFFINE_32x32   BG_SIZE_1
#define BACKGROUND_AFFINE_64x64   BG_SIZE_2
#define BACKGROUND_AFFINE_128x128 BG_SIZE_3

// the color flags
#define BACKGROUND_16COLORS     BG_16_COLOR         // this is ZERO
#define BACKGROUND_256COLORS    BG_256_COLOR

// the wrap flag (affine BGs only)
#define BACKGROUND_WRAP         BG_WRAP

// the mosaic flag
#define BACKGROUND_MOSAIC       BG_MOSAIC

// priority macro
#define BACKGROUND_PRIO(n)      BG_PRIORITY(n)

void configureBackground(unsigned int background_number, unsigned int mode, unsigned int maps, unsigned int tiles);
                                                                            // maps 0-31, tiles 0-3

// scrolling (for text backgounds only!)
void scrollBackground(unsigned int background_number, int scroll_x, int scroll_y);

// affine matrix and reference point for affine backgounds
void configureBackground2AffineMatrix(int dx, int dmx, int dy, int dmy);
void configureBackground3AffineMatrix(int dx, int dmx, int dy, int dmy);
void configureBackground2ReferencePoint (int x, int y);
void configureBackground3ReferencePoint (int x, int y);

// **************************************************
// ****              SPRITES                    ***** 
// **************************************************
//
// Thanks: http://www.coranac.com/tonc/text/regobj.htm
// Note: Caution! A very large OBJ (of 128 pixels vertically, ie. a 64 pixels OBJ in a
//       Double Size area) located at Y>128 will be treated as at Y>-128, the OBJ is
//       then displayed parts offscreen at the TOP of the display, it is then NOT displayed
//       at the bottom. (from http://nocash.emubase.de/gbatek.htm#lcdobjoamattributes )


void initSprites(int boolean_back_to_front);

inline void initSpritesFrontToBack(void);
inline void initSpritesBackToFront(void);

// the 4 possible types
#define SPRITE_TYPE_NORMAL          0x00
#define SPRITE_TYPE_ROTSCALE        0x01
#define SPRITE_TYPE_ROTSCALE_DOUBLE 0x03
#define SPRITE_TYPE_DISABLED        0x02

// the 12 possible shapes
#define SPRITE_8x8      ATTR1_SIZE_8                 // this is ZERO
#define SPRITE_16x16    ATTR1_SIZE_16
#define SPRITE_32x32    ATTR1_SIZE_32
#define SPRITE_64x64    ATTR1_SIZE_64
#define SPRITE_16x8     (SPRITE_8x8|(ATTR0_WIDE>>8))
#define SPRITE_8x16     (SPRITE_8x8|(ATTR0_TALL>>8))
#define SPRITE_32x8     (SPRITE_16x16|(ATTR0_WIDE>>8))
#define SPRITE_8x32     (SPRITE_16x16|(ATTR0_TALL>>8))
#define SPRITE_32x16    (SPRITE_32x32|(ATTR0_WIDE>>8))
#define SPRITE_16x32    (SPRITE_32x32|(ATTR0_TALL>>8))
#define SPRITE_64x32    (SPRITE_64x64|(ATTR0_WIDE>>8))
#define SPRITE_32x64    (SPRITE_64x64|(ATTR0_TALL>>8))

// the color flags
#define SPRITE_16COLORS     (ATTR0_COLOR_16>>8)      // this is ZERO
#define SPRITE_256COLORS    (ATTR0_COLOR_256>>8)

// the flipping flags
#define SPRITE_FLIPX        (ATTR1_FLIP_X)
#define SPRITE_FLIPY        (ATTR1_FLIP_Y)

// the semi-transparency flag
#define SPRITE_TRANSPARENT  (OBJ_TRANSLUCENT>>8)

// the object window flag (NOTE: can't be used together with semi-transparency flag)
#define SPRITE_WINDOW       (OBJ_WINDOW>>8)

// the mosaic flag
#define SPRITE_MOSAIC       (OBJ_MOSAIC>>8)

// palette macro (no effect on 256 colors sprites)
#define SPRITE_PAL(n)       ((n)<<20)

// priority macro
#define SPRITE_PRIO(n)      ((n)<<18)

// affine matrix
#define SPRITE_AFFINE_MATRIX(n)     ((n)<<9)

// the possible addSprite()/reserveSprite()/setSpritePosition() return error codes
#define NO_SPRITES_LEFT         -1
#define SPRITE_CLIPPED          -2
// the possible addObjectAffineMatrix() return error code
#define NO_AFFINE_MATRIX_LEFT   -8

int  addSprite(int x, int y, unsigned int image, unsigned int properties);
int  reserveSprite(void);
void setSpriteProperties(unsigned int sprite, unsigned int image, unsigned int properties);
int  setSpritePosition(unsigned int sprite, int x, int y);
int  addObjectAffineMatrix(int pa, int pb, int pc, int pd);
void setObjectAffineMatrix(unsigned int matrix, int pa, int pb, int pc, int pd);

void finalizeSprites(void);
void copySpritesToOAM(void);

// **************************************************
// ****    Special Effects & Windowing          ***** 
// **************************************************
//

#define SPECIALEFFECT_TYPE_DISABLE              0x00
#define SPECIALEFFECT_TYPE_ALPHABLEND           0x01
#define SPECIALEFFECT_TYPE_BRIGHTNESSINCREASE   0x02
#define SPECIALEFFECT_TYPE_BRIGHTNESSDECREASE   0x03

#define SPECIALEFFECT_TARGET_BACKGROUND_0       BIT(0)
#define SPECIALEFFECT_TARGET_BACKGROUND_1       BIT(1)
#define SPECIALEFFECT_TARGET_BACKGROUND_2       BIT(2)
#define SPECIALEFFECT_TARGET_BACKGROUND_3       BIT(3)
#define SPECIALEFFECT_TARGET_SPRITES            BIT(4)
#define SPECIALEFFECT_TARGET_BACKDROP           BIT(5)
#define SPECIALEFFECT_TARGET_EVERYTHING         0x3F

void setSpecialEffect(unsigned int which, unsigned int target_A, unsigned int target_B);
void setAlphaBlendingSpecialEffectCoefficients(unsigned int target_A, unsigned int target_B);
void setBrightnessSpecialEffectCoefficient(unsigned int brightness);

void setMosaicCoefficients(unsigned int BG_Mosaic_x,unsigned int BG_Mosaic_y,unsigned int OBJ_Mosaic_x,unsigned int OBJ_Mosaic_y);

#define WINDOW0         WIN0_ENABLE
#define WINDOW1         WIN1_ENABLE
#define SPRITEWINDOW    OBJ_WIN_ENABLE

#define WINDOW_NOTHING              0
#define WINDOW_BACKGROUND_0         BIT(0)
#define WINDOW_BACKGROUND_1         BIT(1)
#define WINDOW_BACKGROUND_2         BIT(2)
#define WINDOW_BACKGROUND_3         BIT(3)
#define WINDOW_SPRITES              BIT(4)
#define WINDOW_SPECIALEFFECTS       BIT(5)  // if special effects should take place in the window
#define WINDOW_EVERYTHING           0x3F

void activateWindow(unsigned int windows);
void deactivateWindow(unsigned int windows);
void setWindow0Coordinates(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1);
void setWindow1Coordinates(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1);
void setWindowControl(unsigned int enabled_inside_win0, unsigned int enabled_inside_win1, unsigned int enabled_outside, unsigned int enabled_objects);

//---------------------------------------------------------------------------------
#endif //_gbadv_h
//---------------------------------------------------------------------------------
