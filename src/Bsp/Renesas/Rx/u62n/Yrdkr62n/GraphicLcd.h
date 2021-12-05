#ifndef Bsp_Renesas_GraphicLcd_h
#define Bsp_Renesas_GraphicLcd_h
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file 


    This file provides a basic interface for OKAYA 96x64 monochromo Graphic LCD.
    The RX62N CPU interfaces with display's STD7579 display controller via a
    a SPI bus (SPI0).

    NOTE: This graphic driver is NOT THREAD SAFE.  It it left to application
          to manage/handle/implement thread safety.


    DO NOT include this file directly! Instead include the generic BSP
    interface - src/Bsp/Api.h.

*----------------------------------------------------------------------------*/ 

#include "colony_config.h"
#include <stdint.h>


// Start C++
#ifdef __cplusplus
extern "C" {
#endif



/// Option for the Bsp_GLcd_setFont() command. 
typedef enum 
    {
    eBSP_GCLD_FONT_8X8 = 0,    
    eBSP_GCLD_FONT_8X16,   
    eBSP_GCLD_FONT_5X7,    
    eBSP_GCLD_FONT_BITMAPS,
    eBSP_GCLD_FONT_SYMBOLS
    } BspGLcd_FontSelection_T;



///////////////////////////////////////////////////////////////////////////////
// Basic Functions
///////////////////////////////////////////////////////////////////////////////

/** This method initializes the graphics driver.  This method must be called
    BEFOE any other methods in this interface. The initial font is determined by 
    the USE_BSP_DEFAULT_FONT compiler switch (the default is the 5x7 font).  The 
    method returns 1 if successful; else 0 is returned.

    Font                    Max Rows    Max Cols
    ----------------------  --------    --------
    eBSP_GCLD_FONT_5X7          8          19
    eBSP_GCLD_FONT_8X8          8          12
    eBSP_GCLD_FONT_8X16         4          12


 */
uint8_t Bsp_GLcd_intialize( void );


/** This method clears the display.
 */
void Bsp_GLcd_clearScreen( void );


/** This method sets the current font.  All 'draw/write' operation after this
    call will use the new font. Previously drawn text is not effected. If the 
    selected is not available then 0 is returned; else 1.  When the
    method returns 0 - the current font is left unchanged.
 */  
uint8_t Bsp_GLcd_setFont( BspGLcd_FontSelection_T newfont );


/** This method will clear an the entire specified line.  Line numbers
    are zero based indexes.
 */
void Bsp_GLcd_clearLine( uint16_t lineNum );


/** This method is the same as Bsp_GLcd_writeRowCol(), except the caller
    specifies how many characters/bytes in the 'message' to write to the display 
    (instead of writing the entire string/message contents).
 */
void Bsp_GLcd_writeRowColLen( uint16_t lineNum, uint16_t colNum, const char* message, uint16_t maxLen );




///////////////////////////////////////////////////////////////////////////////
// FONT Selection (i.e. what will be available in ROM)
//
// NOTE: the 5x7 font is ALWAYS available
///////////////////////////////////////////////////////////////////////////////

/** This compile switch enables the use of the 'symbol' character table.  
    Enable this character table via the 'colony_config.h' header file
 */
#ifdef USE_BSP_GLCD_ENABLE_FONT_SYMBOL
#define USE_GLYPH_FONT_SYMBOL
#endif

/** This compile switch enables the use of the 'bitmap' character table.  
    Enable this character table via the 'colony_config.h' header file
 */
#ifdef USE_BSP_GLCD_ENABLE_FONT_BITMAPS
#define USE_GLYPH_FONT_BITMAP
#endif
 
/** This compile switch enables the use of the '8x8' character table.  
    Enable this character table via the 'colony_config.h' header file
 */
#ifdef USE_BSP_GLCD_ENABLE_FONT_8X8
#define USE_GLYPH_FONT_8_BY_8
#endif

/** This compile switch enables the use of the '8x16' character table.  
    Enable this character table via the 'colony_config.h' header file
 */
#ifdef USE_BSP_GLCD_ENABLE_FONT_8X16
#define USE_GLYPH_FONT_8_BY_16
#endif




/** This sets the default font.  Note: The default MUST BE 'enabled'
    Use the one of the font tables below as possible option for this
    compiler switch.
 */
#ifndef USE_BSP_CHANFGE_DEFAULT_FONT
#define USE_DEFAULT_FONT        Bsp_GLcd_FontTable_5x7
#else
#define USE_DEFAULT_FONT        USE_BSP_CHANFGE_DEFAULT_FONT
#endif        


///////////////////////////////////////////////////////////////////////////////
// FONT Tables.  The application has the option of overriding any of the 
// following font tables.  To override the BSP supplied font table use one or
// more of the following compiler switches.  Note: The application must also 
// make sure that the font is 'enabled' (using the above compile switches).
//
//  #define USE_BSP_GCLD_FONT_TABLE_SYMBOLS_APP_SUPPLIED
//  #define USE_BSP_GCLD_FONT_TABLE_BITMAPS_APP_SUPPLIED
//  #define USE_BSP_GCLD_FONT_TABLE_8X16_APP_SUPPLIED
//  #define USE_BSP_GCLD_FONT_TABLE_8X8_APP_SUPPLIED
//  #define USE_BSP_GCLD_FONT_TABLE_5X7_APP_SUPPLIED
//
///////////////////////////////////////////////////////////////////////////////

/// Font table
extern const uint8_t * const Bsp_GLcd_FontTable_Symbols[256];

/// Font table
extern const uint8_t * const Bsp_GLcd_FontTable_Bitmap[256];

/// Font table
extern const uint8_t * const Bsp_GLcd_FontTable_8x16[256];

/// Font table
extern const uint8_t * const Bsp_GLcd_FontTable_8x8[256];

/// Font table
extern const uint8_t * const Bsp_GLcd_FontTable_5x7[256];


// END C++
#ifdef __cplusplus
};
#endif
#endif  // end header latch
