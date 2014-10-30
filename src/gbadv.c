#include "gbadv.h"

// ***********************************************************************
// ****         Video Mode & Backgrounds                              ****
// ***********************************************************************

void initDisplay(unsigned int mode, int boolean_object_mapping_style_1D) {
  // set video mode and object mapping style, resets all other bits
  REG_DISPCNT = (mode&0x7)|((boolean_object_mapping_style_1D)?OBJ_1D_MAP:0);
}

void showBackgrounds(unsigned int which) {
  // turns on selected background, turns off all the other backgrounds
  REG_DISPCNT = (REG_DISPCNT & ~0xF00) | which;
}

void toggleBackgrounds(unsigned int show, unsigned int hide) {
  // turns on selected background, turns off selected backgrounds, untouching other backgrounds
  REG_DISPCNT = (REG_DISPCNT & ~hide) | show;
}

void toggleBackgroundsOn(unsigned int show) {
  // turns on selected background, untouching other backgrounds
  REG_DISPCNT = REG_DISPCNT | show;
}

void toggleBackgroundsOff(unsigned int hide) {
  // turns off selected background, untouching other backgrounds
  REG_DISPCNT = (REG_DISPCNT & ~hide);
}

void showSprites(int boolean_active) {
  // turns on/off sprites
  REG_DISPCNT = (REG_DISPCNT & ~0x1000) | ((boolean_active)?OBJ_ON:0);
}

void configureBackground(unsigned int background_number, unsigned int mode, unsigned int maps, unsigned int tiles) {
  // set BG(n) mode (size/colors/wrap/priority/maps/tiles) resets all other bits
  BGCTRL[background_number&0x3]=mode|BG_MAP_BASE(maps&0x1F)|BG_TILE_BASE(tiles&0x3);
}

void scrollBackground(unsigned int background_number, int scroll_x, int scroll_y) {
  // scrolls (text) backgrounds as requested
  BG_OFFSET[background_number&0x3].x=scroll_x;
  BG_OFFSET[background_number&0x3].y=scroll_y;
}

void configureBackground2AffineMatrix(int dx, int dmx, int dy, int dmy) {
  // configure affine matrix values for BG2
  REG_BG2PA=dx;
  REG_BG2PB=dmx;
  REG_BG2PC=dy;
  REG_BG2PD=dmy;
}

void configureBackground3AffineMatrix(int dx, int dmx, int dy, int dmy) {
  // configure affine matrix values for BG3
  REG_BG3PA=dx;
  REG_BG3PB=dmx;
  REG_BG3PC=dy;
  REG_BG3PD=dmy;
}

void configureBackground2ReferencePoint (int x, int y) {
  REG_BG2X=x;
  REG_BG2Y=y;
}

void configureBackground3ReferencePoint (int x, int y) {
  REG_BG3X=x;
  REG_BG3Y=y;
}

// ***********************************************************************
// ****           Sprites                                             ****
// ***********************************************************************

OAMTable sprites;

struct {
    short int currentSprite;
    signed char direction;
    unsigned char currentMatrix;
} SpriteMgrStruct;

void initSprites(int boolean_back_to_front) {
  if (boolean_back_to_front) {
    SpriteMgrStruct.currentSprite=127;
    SpriteMgrStruct.direction=-1;
  } else {
    SpriteMgrStruct.currentSprite=0;
    SpriteMgrStruct.direction=+1;
  }
  SpriteMgrStruct.currentMatrix=0;
}

inline void initSpritesFrontToBack(void) { initSprites(0); }
inline void initSpritesBackToFront(void) { initSprites(1); }

// *************** properties explanation bit per bit **********************
//
//    23 22 21 20 19 18 17 16 | 15 14 13 12 11 10 09 08 | 07 06 05 04 03 02 01 00 | bits
//    ----Pal---- -Pri- RS RS | --Sz- Vf Hf .. .. .. RS | --Sh- Co Mo OW Tr --Ty- | meaning when 'normal'
//                                    --aff matrix--  0 |                         | meaning when 'rotscale'
//    --- attr2 high byte --- | --- attr1 high byte --- | --- attr0 high byte --- | correspondig attributes
//
//    Ty  = type (0=normal)
//    Tr  = transparency flag
//    OW  = object window (can't be used with transparency ON)
//    Mo  = mosaic flag
//    Co  = 256 colors flag
//    Sh  = shape
//    Sz  = size
//    Vf  = vertical flip
//    Hf  = horizontal flip
//    Pal = palette
//    Pri = priority
//    ..  = unused
//    RS  = reserved (must be 0)
//
// *************************************************************************

int spriteHeight(unsigned int properties) {

  int h=(8<<((properties>>14) & 0x3));  // 8 SHL bits 14-15 = 8,16,32,64

  switch(properties & 0xC0) {  // check bits 6-7 (shape)
    case (ATTR0_TALL>>8):h<<=((h<32)?1:0); break;   // 16,32,32,64
    case (ATTR0_WIDE>>8):h>>=((h>8)?1:0); break;    // 8,8,16,32
  }

  return(h<<(((properties & SPRITE_TYPE_ROTSCALE_DOUBLE)==SPRITE_TYPE_ROTSCALE_DOUBLE)?1:0));
}

int spriteWidth(unsigned int properties) {

  int w=(8<<((properties>>14) & 0x3));  // 8 SHL bits 14-15 = 8,16,32,64

  switch(properties & 0xC0) {  // check bits 6-7 (shape)
    case (ATTR0_WIDE>>8):w<<=((w<32)?1:0); break;   // 16,32,32,64
    case (ATTR0_TALL>>8):w>>=((w>8)?1:0); break;    // 8,8,16,32
  }

  return(w<<(((properties & SPRITE_TYPE_ROTSCALE_DOUBLE)==SPRITE_TYPE_ROTSCALE_DOUBLE)?1:0));
}

int countSpritesLeft(void) {
  if (SpriteMgrStruct.direction<0) {          // back to front (reverse)
    return(SpriteMgrStruct.currentSprite+1);
  } else {                                    // front to back
    return(128-SpriteMgrStruct.currentSprite);
  }
}

int addSprite(int x, int y, unsigned int image, unsigned int properties) {

  // check if we've got any free sprite left
  if (countSpritesLeft()==0)
    return(NO_SPRITES_LEFT);

  // quite serious clipping
  if ((x>=SCREEN_WIDTH) || (y>=SCREEN_HEIGHT) ||
      (x<=-spriteWidth(properties)) || (y<=-spriteHeight(properties)))
    return(SPRITE_CLIPPED);

  // set sprite attributes
  sprites.oamBuffer[SpriteMgrStruct.currentSprite].attr0=(y&0xFF)|((properties<<8)&0xFF00);
  sprites.oamBuffer[SpriteMgrStruct.currentSprite].attr1=(x&0x1FF)|(properties&0xFF00);
  sprites.oamBuffer[SpriteMgrStruct.currentSprite].attr2=(image&0x3FF)|((properties>>8)&0xFF00);

  // move to next free sprite
  SpriteMgrStruct.currentSprite += SpriteMgrStruct.direction;

  // return the number of the current sprite
  return(SpriteMgrStruct.currentSprite-SpriteMgrStruct.direction);
}

int reserveSprite(void) {

  // check if we've got any free sprite left
  if (countSpritesLeft()==0)
    return(NO_SPRITES_LEFT);

  // disable next sprite
  sprites.oamBuffer[SpriteMgrStruct.currentSprite].attr0=(SCREEN_HEIGHT&0xFF)|ATTR0_DISABLED|ATTR0_SQUARE;
  sprites.oamBuffer[SpriteMgrStruct.currentSprite].attr1=(SCREEN_WIDTH&0x1FF)|ATTR1_SIZE_8;

  // move to next free sprite
  SpriteMgrStruct.currentSprite += SpriteMgrStruct.direction;
  
  // return the number of the reserved sprite
  return(SpriteMgrStruct.currentSprite-SpriteMgrStruct.direction);
}

void setSpriteProperties(unsigned int sprite, unsigned int image, unsigned int properties) {
  // set sprite attributes retaining original X and Y
  sprites.oamBuffer[sprite].attr0=(sprites.oamBuffer[sprite].attr0&0xFF)|((properties<<8)&0xFF00);
  sprites.oamBuffer[sprite].attr1=(sprites.oamBuffer[sprite].attr1&0x1FF)|(properties&0xFF00);
  sprites.oamBuffer[sprite].attr2=(image&0x3FF)|((properties>>8)&0xFF00);
}

int setSpritePosition(unsigned int sprite, int x, int y) {
  // set sprite X and Y retaining original properties
  // quite serious clipping
  unsigned int properties=((sprites.oamBuffer[sprite].attr0)>>8)|(sprites.oamBuffer[sprite].attr1&0xFF00);
  if ((x>=SCREEN_WIDTH) || (y>=SCREEN_HEIGHT) ||
      (x<=-spriteWidth(properties)) || (y<=-spriteHeight(properties))) {
    // sprite clipped: disable it!
    sprites.oamBuffer[sprite].attr0=(SCREEN_HEIGHT&0xFF)|ATTR0_DISABLED|ATTR0_SQUARE;
    sprites.oamBuffer[sprite].attr1=(SCREEN_WIDTH&0x1FF)|ATTR1_SIZE_8;
    return(SPRITE_CLIPPED);
  }

  sprites.oamBuffer[sprite].attr0=(y&0xFF)|(sprites.oamBuffer[sprite].attr0&0xFF00);
  sprites.oamBuffer[sprite].attr1=(x&0x1FF)|(sprites.oamBuffer[sprite].attr1&0xFE00);
  
  return(sprite);
}

int countObjectAffineMatrixLeft(void) {
  return(32-SpriteMgrStruct.currentMatrix);
}

int addObjectAffineMatrix(int pa, int pb, int pc, int pd) {

  // are matrixes over?
  if (countObjectAffineMatrixLeft()==0)
    return(NO_AFFINE_MATRIX_LEFT);
  
  sprites.matrixBuffer[SpriteMgrStruct.currentMatrix].pa=pa;
  sprites.matrixBuffer[SpriteMgrStruct.currentMatrix].pb=pb;
  sprites.matrixBuffer[SpriteMgrStruct.currentMatrix].pc=pc;
  sprites.matrixBuffer[SpriteMgrStruct.currentMatrix].pd=pd;
  
  SpriteMgrStruct.currentMatrix++;
  return(SpriteMgrStruct.currentMatrix-1);
}

void setObjectAffineMatrix(unsigned int matrix, int pa, int pb, int pc, int pd) {
  sprites.matrixBuffer[matrix].pa=pa;
  sprites.matrixBuffer[matrix].pb=pb;
  sprites.matrixBuffer[matrix].pc=pc;
  sprites.matrixBuffer[matrix].pd=pd;
}

void finalizeSprites(void) {
  // TO DO: check if setting it to 160,240 puts it as -95,-15 ...
  while (((SpriteMgrStruct.direction<0) && (SpriteMgrStruct.currentSprite>=0)) ||     // back to front (reverse)
         ((SpriteMgrStruct.direction>0) && (SpriteMgrStruct.currentSprite<=127))) {        
    sprites.oamBuffer[SpriteMgrStruct.currentSprite].attr0=(SCREEN_HEIGHT&0xFF)|ATTR0_DISABLED|ATTR0_SQUARE;
    sprites.oamBuffer[SpriteMgrStruct.currentSprite].attr1=(SCREEN_WIDTH&0x1FF)|ATTR1_SIZE_8;
    // move to next free sprite
    SpriteMgrStruct.currentSprite += SpriteMgrStruct.direction;
  }
}

void copySpritesToOAM(void) {
  // to do: use DMA to copy
  CpuFastSet ((void *)sprites.oamBuffer,(void *)OAM,(sizeof(OAMTable)>>2)|COPY32);
}

// ***********************************************************************
// ****           All the rest...                                     ****
// ***********************************************************************

void setSpecialEffect(unsigned int which, unsigned int target_A, unsigned int target_B) {
  REG_BLDCNT=(which<<6)|(target_B<<8)|(target_A);
}

void setAlphaBlendingSpecialEffectCoefficients(unsigned int target_A, unsigned int target_B) {
  REG_BLDALPHA=(target_A&0x1F)|((target_B&0x1F)<<8);
}

void setBrightnessSpecialEffectCoefficient(unsigned int brightness) {
  REG_BLDY=(brightness&0x1F);
}

void activateWindow(unsigned int windows) {
  REG_DISPCNT|=(windows&0xE000);
}

void deactivateWindow(unsigned int windows) {
  REG_DISPCNT&=~(windows&0xE000);
}

void setWindow0Coordinates(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1) {
  REG_WIN0H=((x1+1)&0xFF)|(x0<<8);
  REG_WIN0V=((y1+1)&0xFF)|(y0<<8);
}

void setWindow1Coordinates(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1) {
  REG_WIN1H=((x1+1)&0xFF)|(x0<<8);
  REG_WIN1V=((y1+1)&0xFF)|(y0<<8);
}

void setWindowControl(unsigned int enabled_inside_win0, unsigned int enabled_inside_win1, unsigned int enabled_outside, unsigned int enabled_objects) {
  REG_WININ =(enabled_inside_win0&0x3F)|(enabled_inside_win1&0x3F)<<8;
  REG_WINOUT=(enabled_outside&0x3F)|(enabled_objects&0x3F)<<8;
}

void setMosaicCoefficients(unsigned int BG_Mosaic_x,unsigned int BG_Mosaic_y,unsigned int OBJ_Mosaic_x,unsigned int OBJ_Mosaic_y) {
  REG_MOSAIC=(BG_Mosaic_x&0xF)|((BG_Mosaic_y&0xF)<<4)|((OBJ_Mosaic_x&0xF)<<8)|((OBJ_Mosaic_y&0xF)<<12);
}
