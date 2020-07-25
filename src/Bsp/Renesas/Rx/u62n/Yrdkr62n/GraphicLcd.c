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

#include "GraphicLcd.h"
#include "Bsp/Renesas/Rx/_glyph/Glyph.h"
#include <string.h>



///
static T_glyphHandle  glyphHdl_;
static uint8_t        pxPerRow_;
static uint8_t        pxPerCol_;




///////////////////////////////////////////////////////////////////////////////
uint8_t Bsp_GLcd_intialize( void )
    {
    if ( GlyphOpen(&glyphHdl_, 0) == GLYPH_ERROR_NONE ) 
        {
        GlyphNormalScreen( glyphHdl_ );
        GlyphClearScreen( glyphHdl_ );

        // Set pixel count for the default font of: 5 x 7
        pxPerRow_ = 9;
        pxPerCol_ = 5;
        return 1;
        }

    // If I get here, I failed to open/start my driver
    return 0;

    }


void Bsp_GLcd_clearScreen( void )
    {
    GlyphClearScreen( glyphHdl_ );
    }


uint8_t Bsp_GLcd_setFont( BspGLcd_FontSelection_T newFont )
    {
    switch( newFont ) 
        {
        default:
            return 0;


        case eBSP_GCLD_FONT_8X8:
            GlyphSetFont( glyphHdl_, GLYPH_FONT_8_BY_8 );
            pxPerRow_ = 8;
            pxPerCol_ = 8;
            break;

        case eBSP_GCLD_FONT_8X16:
            GlyphSetFont( glyphHdl_, GLYPH_FONT_8_BY_16 );
            pxPerRow_ = 16;
            pxPerCol_ = 8;
            break;

        case eBSP_GCLD_FONT_5X7:
            GlyphSetFont( glyphHdl_, GLYPH_FONT_5_BY_7 );
            pxPerRow_ = 8;
            pxPerCol_ = 5;
            break;

        case eBSP_GCLD_FONT_BITMAPS:
            GlyphSetFont( glyphHdl_, GLYPH_FONT_BITMAP );
            pxPerRow_ = 8;
            pxPerCol_ = 5;
            break;

        case eBSP_GCLD_FONT_SYMBOLS:
            GlyphSetFont( glyphHdl_, GLYPH_FONT_SYMBOL );
            pxPerRow_ = 8;
            pxPerCol_ = 5;
            break;
        }

    // If I get here, everything worked
    return 1;
    }


void Bsp_GLcd_clearLine( uint16_t lineNum )
    {
    uint16_t y = lineNum * pxPerRow_;

    GlyphEraseBlock( glyphHdl_, 0, y, 95, y + pxPerRow_ );
    }

void Bsp_GLcd_writeRowColLen( uint16_t lineNum, uint16_t colNum, const char* message, uint16_t maxLen )
    {
    uint16_t col;
    uint16_t row;
    uint16_t len;

    row = lineNum * pxPerRow_;
    col = colNum  * pxPerCol_;
    len = strlen(message);
    len = len <= maxLen? len: maxLen;

    GlyphSetXY( glyphHdl_, col, row );
    GlyphString( glyphHdl_, (uint8_t*)message, len );
    }


