
#ifndef Storm_Thermostat_Ui_PicoDisplay_Layout_h
#define Storm_Thermostat_Ui_PicoDisplay_Layout_h
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file

    THIS IS PRIVATE HEADER AND SHOULD NOT BE INCLUDED/USED BY THE APPLICATION.

    This file contains all of the magic coordinates, size, constants, etc.
    that define the 'layout' of the screen(s)
 */

/*---------------------------------------------------------------------------*/
#ifndef DOXYGEN_WILL_SKIP_THIS

#define BACKGROUND_COLOR_R          0
#define BACKGROUND_COLOR_G          0
#define BACKGROUND_COLOR_B          0

#define ACTIVE_BACKGROUND_COLOR_R   0  
#define ACTIVE_BACKGROUND_COLOR_G   0
#define ACTIVE_BACKGROUND_COLOR_B   255

#define FOREGROUND_TEXT_COLOR_R     255
#define FOREGROUND_TEXT_COLOR_G     255
#define FOREGROUND_TEXT_COLOR_B     255

#define VALUE_NOMINAL_TEXT_COLOR_R  255
#define VALUE_NOMINAL_TEXT_COLOR_G  255
#define VALUE_NOMINAL_TEXT_COLOR_B  0

#define VALUE_ACTIVE_TEXT_COLOR_R   255
#define VALUE_ACTIVE_TEXT_COLOR_G   255
#define VALUE_ACTIVE_TEXT_COLOR_B   0

#define HVAC_OUTPUT_TEXT_COLOR_R    0
#define HVAC_OUTPUT_TEXT_COLOR_G    255
#define HVAC_OUTPUT_TEXT_COLOR_B    255


#define TEXT_HEIGHT                 18

#define COLUMN0_X0                  10
#define COLUMN1_X0                  (COLUMN0_X0+50+4)
#define COLUMN2_X0                  (120+10)
#define COLUMN3_X0                  (COLUMN2_X0+40+10)

#define ROW0_Y0                     10
#define ROW1_Y0                     (ROW0_Y0+ROW_HEIGHT)
#define ROW2_Y0                     (ROW1_Y0+ROW_HEIGHT)
#define ROW3_Y0                     (ROW2_Y0+ROW_HEIGHT+ROW_HEIGHT-4)
#define ROW_HEIGHT                  (TEXT_HEIGHT+8)

#define MODE_LABEL_COLOR_R          FOREGROUND_TEXT_COLOR_R    
#define MODE_LABEL_COLOR_G          FOREGROUND_TEXT_COLOR_G
#define MODE_LABEL_COLOR_B          FOREGROUND_TEXT_COLOR_B
#define MODE_LABEL_X0               COLUMN0_X0
#define MODE_LABEL_Y0               ROW0_Y0
#define MODE_LABEL_WIDTH            50

#define MODE_NOMINAL_COLOR_R        VALUE_NOMINAL_TEXT_COLOR_R    
#define MODE_NOMINAL_COLOR_G        VALUE_NOMINAL_TEXT_COLOR_G
#define MODE_NOMINAL_COLOR_B        VALUE_NOMINAL_TEXT_COLOR_B
#define MODE_ACTIVE_COLOR_R         VALUE_ACTIVE_TEXT_COLOR_R    
#define MODE_ACTIVE_COLOR_G         VALUE_ACTIVE_TEXT_COLOR_G
#define MODE_ACTIVE_COLOR_B         VALUE_ACTIVE_TEXT_COLOR_B
#define MODE_BKGRD_ACTIVE_COLOR_R   ACTIVE_BACKGROUND_COLOR_R  
#define MODE_BKGRD_ACTIVE_COLOR_G   ACTIVE_BACKGROUND_COLOR_G
#define MODE_BKGRD_ACTIVE_COLOR_B   ACTIVE_BACKGROUND_COLOR_B
#define MODE_X0                     COLUMN1_X0
#define MODE_Y0                     MODE_LABEL_Y0
#define MODE_WIDTH                  50
#define MODE_HEIGHT                 TEXT_HEIGHT
#define MODE_OUTLINE_X0             (MODE_X0 - MODE_OUTLINE_DELTA)
#define MODE_OUTLINE_Y0             (MODE_Y0 - MODE_OUTLINE_DELTA)
#define MODE_OUTLINE_WIDTH          (MODE_WIDTH + MODE_OUTLINE_DELTA)
#define MODE_OUTLINE_HEIGHT         (MODE_HEIGHT + MODE_OUTLINE_DELTA-1)
#define MODE_OUTLINE_DELTA          4

#define FAN_LABEL_COLOR_R           FOREGROUND_TEXT_COLOR_R    
#define FAN_LABEL_COLOR_G           FOREGROUND_TEXT_COLOR_G
#define FAN_LABEL_COLOR_B           FOREGROUND_TEXT_COLOR_B
#define FAN_LABEL_X0                COLUMN2_X0
#define FAN_LABEL_Y0                MODE_LABEL_Y0
#define FAN_LABEL_WIDTH             40

#define FAN_NOMINAL_COLOR_R         VALUE_NOMINAL_TEXT_COLOR_R    
#define FAN_NOMINAL_COLOR_G         VALUE_NOMINAL_TEXT_COLOR_G
#define FAN_NOMINAL_COLOR_B         VALUE_NOMINAL_TEXT_COLOR_B
#define FAN_ACTIVE_COLOR_R          VALUE_ACTIVE_TEXT_COLOR_R   
#define FAN_ACTIVE_COLOR_G          VALUE_ACTIVE_TEXT_COLOR_G
#define FAN_ACTIVE_COLOR_B          VALUE_ACTIVE_TEXT_COLOR_B
#define FAN_BKGRD_ACTIVE_COLOR_R    ACTIVE_BACKGROUND_COLOR_R    
#define FAN_BKGRD_ACTIVE_COLOR_G    ACTIVE_BACKGROUND_COLOR_G
#define FAN_BKGRD_ACTIVE_COLOR_B    ACTIVE_BACKGROUND_COLOR_B
#define FAN_X0                      COLUMN3_X0
#define FAN_Y0                      FAN_LABEL_Y0
#define FAN_WIDTH                   43
#define FAN_HEIGHT                  TEXT_HEIGHT
#define FAN_OUTLINE_X0              (FAN_X0 - FAN_OUTLINE_DELTA)
#define FAN_OUTLINE_Y0              (FAN_Y0 - FAN_OUTLINE_DELTA)
#define FAN_OUTLINE_WIDTH           (FAN_WIDTH + FAN_OUTLINE_DELTA)
#define FAN_OUTLINE_HEIGHT          (FAN_HEIGHT + FAN_OUTLINE_DELTA-1)
#define FAN_OUTLINE_DELTA           4


#define IDT_LABEL_COLOR_R           FOREGROUND_TEXT_COLOR_R    
#define IDT_LABEL_COLOR_G           FOREGROUND_TEXT_COLOR_G
#define IDT_LABEL_COLOR_B           FOREGROUND_TEXT_COLOR_B
#define IDT_LABEL_X0                COLUMN0_X0
#define IDT_LABEL_Y0                ROW1_Y0
#define IDT_LABEL_WIDTH             50

#define IDT_NOMINAL_COLOR_R         VALUE_NOMINAL_TEXT_COLOR_R    
#define IDT_NOMINAL_COLOR_G         VALUE_NOMINAL_TEXT_COLOR_G
#define IDT_NOMINAL_COLOR_B         VALUE_NOMINAL_TEXT_COLOR_B
#define IDT_X0                      COLUMN1_X0
#define IDT_Y0                      IDT_LABEL_Y0
#define IDT_WIDTH                   50
#define IDT_HEIGHT                  TEXT_HEIGHT
#define IDT_OUTLINE_X0              (IDT_X0 - IDT_OUTLINE_DELTA)
#define IDT_OUTLINE_Y0              (IDT_Y0 - IDT_OUTLINE_DELTA)
#define IDT_OUTLINE_WIDTH           (IDT_WIDTH + IDT_OUTLINE_DELTA)
#define IDT_OUTLINE_HEIGHT          (IDT_HEIGHT + IDT_OUTLINE_DELTA-1)
#define IDT_OUTLINE_DELTA           2

#define ODT_LABEL_COLOR_R           FOREGROUND_TEXT_COLOR_R    
#define ODT_LABEL_COLOR_G           FOREGROUND_TEXT_COLOR_G
#define ODT_LABEL_COLOR_B           FOREGROUND_TEXT_COLOR_B
#define ODT_LABEL_X0                COLUMN0_X0
#define ODT_LABEL_Y0                ROW2_Y0
#define ODT_LABEL_WIDTH             50

#define ODT_NOMINAL_COLOR_R         VALUE_NOMINAL_TEXT_COLOR_R   
#define ODT_NOMINAL_COLOR_G         VALUE_NOMINAL_TEXT_COLOR_G
#define ODT_NOMINAL_COLOR_B         VALUE_NOMINAL_TEXT_COLOR_B
#define ODT_X0                      COLUMN1_X0
#define ODT_Y0                      ODT_LABEL_Y0
#define ODT_WIDTH                   43
#define ODT_HEIGHT                  TEXT_HEIGHT
#define ODT_OUTLINE_X0              (ODT_X0 - ODT_OUTLINE_DELTA)
#define ODT_OUTLINE_Y0              (ODT_Y0 - ODT_OUTLINE_DELTA)
#define ODT_OUTLINE_WIDTH           (ODT_WIDTH + ODT_OUTLINE_DELTA)
#define ODT_OUTLINE_HEIGHT          (ODT_HEIGHT + ODT_OUTLINE_DELTA-1)
#define ODT_OUTLINE_DELTA           4

#define COOL_LABEL_COLOR_R          FOREGROUND_TEXT_COLOR_R    
#define COOL_LABEL_COLOR_G          FOREGROUND_TEXT_COLOR_G
#define COOL_LABEL_COLOR_B          FOREGROUND_TEXT_COLOR_B
#define COOL_LABEL_X0               COLUMN2_X0
#define COOL_LABEL_Y0               ROW1_Y0
#define COOL_LABEL_WIDTH            50

#define COOL_NOMINAL_COLOR_R        VALUE_NOMINAL_TEXT_COLOR_R    
#define COOL_NOMINAL_COLOR_G        VALUE_NOMINAL_TEXT_COLOR_G
#define COOL_NOMINAL_COLOR_B        VALUE_NOMINAL_TEXT_COLOR_B
#define COOL_ACTIVE_COLOR_R         VALUE_ACTIVE_TEXT_COLOR_R    
#define COOL_ACTIVE_COLOR_G         VALUE_ACTIVE_TEXT_COLOR_G
#define COOL_ACTIVE_COLOR_B         VALUE_ACTIVE_TEXT_COLOR_B
#define COOL_BKGRD_ACTIVE_COLOR_R   ACTIVE_BACKGROUND_COLOR_R  
#define COOL_BKGRD_ACTIVE_COLOR_G   ACTIVE_BACKGROUND_COLOR_G
#define COOL_BKGRD_ACTIVE_COLOR_B   ACTIVE_BACKGROUND_COLOR_B
#define COOL_X0                     COLUMN3_X0
#define COOL_Y0                     COOL_LABEL_Y0
#define COOL_WIDTH                  25
#define COOL_HEIGHT                 TEXT_HEIGHT
#define COOL_OUTLINE_X0             (COOL_X0 - COOL_OUTLINE_DELTA)
#define COOL_OUTLINE_Y0             (COOL_Y0 - COOL_OUTLINE_DELTA)
#define COOL_OUTLINE_WIDTH          (COOL_WIDTH + COOL_OUTLINE_DELTA)
#define COOL_OUTLINE_HEIGHT         (COOL_HEIGHT + COOL_OUTLINE_DELTA-1)
#define COOL_OUTLINE_DELTA          4

#define HEAT_LABEL_COLOR_R          FOREGROUND_TEXT_COLOR_R    
#define HEAT_LABEL_COLOR_G          FOREGROUND_TEXT_COLOR_G
#define HEAT_LABEL_COLOR_B          FOREGROUND_TEXT_COLOR_B
#define HEAT_LABEL_X0               COLUMN2_X0
#define HEAT_LABEL_Y0               ROW2_Y0
#define HEAT_LABEL_WIDTH            50

#define HEAT_NOMINAL_COLOR_R        VALUE_NOMINAL_TEXT_COLOR_R    
#define HEAT_NOMINAL_COLOR_G        VALUE_NOMINAL_TEXT_COLOR_G
#define HEAT_NOMINAL_COLOR_B        VALUE_NOMINAL_TEXT_COLOR_B
#define HEAT_ACTIVE_COLOR_R         VALUE_ACTIVE_TEXT_COLOR_R    
#define HEAT_ACTIVE_COLOR_G         VALUE_ACTIVE_TEXT_COLOR_G
#define HEAT_ACTIVE_COLOR_B         VALUE_ACTIVE_TEXT_COLOR_B
#define HEAT_BKGRD_ACTIVE_COLOR_R   ACTIVE_BACKGROUND_COLOR_R  
#define HEAT_BKGRD_ACTIVE_COLOR_G   ACTIVE_BACKGROUND_COLOR_G
#define HEAT_BKGRD_ACTIVE_COLOR_B   ACTIVE_BACKGROUND_COLOR_B
#define HEAT_X0                     COLUMN3_X0
#define HEAT_Y0                     HEAT_LABEL_Y0
#define HEAT_WIDTH                  25
#define HEAT_HEIGHT                 TEXT_HEIGHT
#define HEAT_OUTLINE_X0             (HEAT_X0 - HEAT_OUTLINE_DELTA)
#define HEAT_OUTLINE_Y0             (HEAT_Y0 - HEAT_OUTLINE_DELTA)
#define HEAT_OUTLINE_WIDTH          (HEAT_WIDTH + HEAT_OUTLINE_DELTA)
#define HEAT_OUTLINE_HEIGHT         (HEAT_HEIGHT + HEAT_OUTLINE_DELTA-1)
#define HEAT_OUTLINE_DELTA          4

#define OUTPUT_GROUP_SPACER_WIDTH   18
#define OUTPUT_SPACE_WIDTH          6

#define G_OUTPUT_X0                 COLUMN0_X0
#define G_OUTPUT_Y0                 ROW3_Y0
#define G_OUTPUT_WIDTH              55
#define G_OUTPUT_HEIGHT             TEXT_HEIGHT
#define G_OUTPUT_NOMINAL_COLOR_R    HVAC_OUTPUT_TEXT_COLOR_R    
#define G_OUTPUT_NOMINAL_COLOR_G    HVAC_OUTPUT_TEXT_COLOR_G
#define G_OUTPUT_NOMINAL_COLOR_B    HVAC_OUTPUT_TEXT_COLOR_B

#define Y1_OUTPUT_X0                (G_OUTPUT_X0+G_OUTPUT_WIDTH+OUTPUT_GROUP_SPACER_WIDTH)
#define Y1_OUTPUT_Y0                ROW3_Y0
#define Y1_OUTPUT_WIDTH             20
#define Y1_OUTPUT_HEIGHT            TEXT_HEIGHT
#define Y1_OUTPUT_NOMINAL_COLOR_R   HVAC_OUTPUT_TEXT_COLOR_R    
#define Y1_OUTPUT_NOMINAL_COLOR_G   HVAC_OUTPUT_TEXT_COLOR_G
#define Y1_OUTPUT_NOMINAL_COLOR_B   HVAC_OUTPUT_TEXT_COLOR_B

#define Y2_OUTPUT_X0                (Y1_OUTPUT_X0+Y1_OUTPUT_WIDTH+OUTPUT_SPACE_WIDTH)
#define Y2_OUTPUT_Y0                ROW3_Y0
#define Y2_OUTPUT_WIDTH             (Y1_OUTPUT_WIDTH+2)
#define Y2_OUTPUT_HEIGHT            TEXT_HEIGHT
#define Y2_OUTPUT_NOMINAL_COLOR_R   HVAC_OUTPUT_TEXT_COLOR_R    
#define Y2_OUTPUT_NOMINAL_COLOR_G   HVAC_OUTPUT_TEXT_COLOR_G
#define Y2_OUTPUT_NOMINAL_COLOR_B   HVAC_OUTPUT_TEXT_COLOR_B

#define W1_OUTPUT_X0                (Y2_OUTPUT_X0+Y2_OUTPUT_WIDTH+OUTPUT_GROUP_SPACER_WIDTH)
#define W1_OUTPUT_Y0                ROW3_Y0        
#define W1_OUTPUT_WIDTH             20
#define W1_OUTPUT_HEIGHT            TEXT_HEIGHT
#define W1_OUTPUT_NOMINAL_COLOR_R   HVAC_OUTPUT_TEXT_COLOR_R    
#define W1_OUTPUT_NOMINAL_COLOR_G   HVAC_OUTPUT_TEXT_COLOR_G
#define W1_OUTPUT_NOMINAL_COLOR_B   HVAC_OUTPUT_TEXT_COLOR_B

#define W2_OUTPUT_X0                (W1_OUTPUT_X0+W1_OUTPUT_WIDTH+OUTPUT_SPACE_WIDTH)
#define W2_OUTPUT_Y0                ROW3_Y0
#define W2_OUTPUT_WIDTH             (W1_OUTPUT_WIDTH+2)
#define W2_OUTPUT_HEIGHT            TEXT_HEIGHT
#define W2_OUTPUT_NOMINAL_COLOR_R   HVAC_OUTPUT_TEXT_COLOR_R    
#define W2_OUTPUT_NOMINAL_COLOR_G   HVAC_OUTPUT_TEXT_COLOR_G
#define W2_OUTPUT_NOMINAL_COLOR_B   HVAC_OUTPUT_TEXT_COLOR_B

#define W3_OUTPUT_X0                (W2_OUTPUT_X0+W2_OUTPUT_WIDTH+OUTPUT_SPACE_WIDTH)
#define W3_OUTPUT_Y0                ROW3_Y0
#define W3_OUTPUT_WIDTH             (W1_OUTPUT_WIDTH+2)
#define W3_OUTPUT_HEIGHT            TEXT_HEIGHT
#define W3_OUTPUT_NOMINAL_COLOR_R   HVAC_OUTPUT_TEXT_COLOR_R    
#define W3_OUTPUT_NOMINAL_COLOR_G   HVAC_OUTPUT_TEXT_COLOR_G
#define W3_OUTPUT_NOMINAL_COLOR_B   HVAC_OUTPUT_TEXT_COLOR_B


#endif // end DOXYGEN_WILL_SKIP_THIS
/*---------------------------------------------------------------------------*/
#endif // end header latch

