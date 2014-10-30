GBAdv
=====

libgba provides almost everything you need to drive the GBA graphical capabilities, but it forces the programmer to do so in a very low level fashion, thru direct access to the GBA internal registers.
This is the reason I made GBAdv, a collection of wrapper functions (and a bit more) which are useful to abstract from that low level, and finally let more enthusiast homebrewers into GBA programming.

You just have to download the GBAdv C source and put it with your project sources.
Modify it if you wish/need it! Just please let me know if you find any bug.

Here's a list of the functions that are defined. Full documentation is available here -> http://wcms.teleion.it/users/cgq/gba/gbadv.html

Video Mode functions:
- void initDisplay (unsigned int mode, int boolean_object_mapping_style_1D)

Background functions:
- void configureBackground (unsigned int background_number, unsigned int mode, unsigned int maps, unsigned int tiles)
- void showBackgrounds (unsigned int which)
- void toggleBackgrounds (unsigned int show, unsigned int hide)
- void toggleBackgroundsOn (unsigned int show)
- void toggleBackgroundsOff (unsigned int hide)
- void scrollBackground (unsigned int background_number, int scroll_x, int scroll_y)
- void configureBackground2AffineMatrix (int dx, int dmx, int dy, int dmy)
- void configureBackground3AffineMatrix (int dx, int dmx, int dy, int dmy)
- void configureBackground2ReferencePoint (int x, int y)
- void configureBackground3ReferencePoint (int x, int y)

Sprite functions:
- void showSprites (int boolean_active)
- void initSprites (int boolean_back_to_front)
- void initSpritesFrontToBack (void)
- void initSpritesBackToFront (void)
- int addSprite (int x, int y, unsigned int image, unsigned int properties)
- int reserveSprite (void)
- void setSpriteProperties (unsigned int sprite, unsigned int image, unsigned int properties)
- int setSpritePosition (unsigned int sprite, int x, int y)
- int addObjectAffineMatrix (int pa, int pb, int pc, int pd)
- void setObjectAffineMatrix (unsigned int matrix, int pa, int pb, int pc, int pd)
 
Other functions:
- void setSpecialEffect (unsigned int which, unsigned int target_A, unsigned int target_B)
- void setAlphaBlendingSpecialEffectCoefficients (unsigned int target_A, unsigned int target_B)
- void setBrightnessSpecialEffectCoefficient (unsigned int brightness)
- void setMosaicCoefficients (unsigned int BG_Mosaic_x, unsigned int BG_Mosaic_y, unsigned int OBJ_Mosaic_x, unsigned int OBJ_Mosaic_y)
- void activateWindow (unsigned int windows)
- void deactivateWindow (unsigned int windows)
- void setWindow0Coordinates (unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1)
- void setWindow1Coordinates (unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1)
- void setWindowControl (unsigned int enabled_inside_win0, unsigned int enabled_inside_win1, unsigned int enabled_outside, unsigned int enabled_objects)
