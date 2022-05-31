
#include "Arduino.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/FreeRTOS/Thread.h"
#include "Cpl/Io/InputOutput.h"
#include <Adafruit_SPIFlash.h>
#include <SdFat.h>
#include "ff.h"

extern Cpl::Io::InputOutput& Bsp_Serial( void );


Adafruit_FlashTransport_QSPI flashTransport;
Adafruit_SPIFlash flash( &flashTransport );

// file system object from SdFat
FatFileSystem fatfs;

// up to 11 characters
#define DISK_LABEL    "EXTFLASH"

// Since SdFat doesn't fully support FAT12 such as format a new flash
// We will use Elm Cham's fatfs f_mkfs() to format
#include "ff.h"
#include "diskio.h"

FATFS elmchamFatfs;
uint8_t workbuf[4096]; // Working buffer for f_fdisk function.


// the setup function runs once when you press reset or power the board
// NOTE: FreeRTOS is RUNNING at this point
void setup()
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Make the current/main thread a CPL Thread
    Cpl::System::FreeRTOS::Thread::makeNativeMainThreadACplThread();

    // Initialize the board (for use with CPL)
    Bsp_Api_initialize();
    Bsp_beginArduinoSerialObject( 115200, SERIAL_8N1 );

    ::Serial.println( "Adafruit SPI Flash FatFs Format Example" );

    // Wait for user to send OK to continue.
    do {
        ::Serial.setTimeout( 5000 );  // Increase timeout to print message less frequently.
        ::Serial.println( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" );
        ::Serial.println( "This sketch will ERASE ALL DATA on the flash chip and format it with a new filesystem!" );
        ::Serial.println( "Type OK (all caps) and press enter to continue." );
        ::Serial.println( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" );
    } while ( !::Serial.find( (char*) "OK" ) );

    // Initialize flash library and check its chip ID.
    if ( !flash.begin() ) {
        ::Serial.println( "Error, failed to initialize flash chip!" );
        while ( 1 ) yield();
    }
    ::Serial.print( "Flash chip JEDEC ID: 0x" ); ::Serial.println( flash.getJEDECID(), HEX );


    // Call fatfs begin and passed flash object to initialize file system
    ::Serial.println( "Creating and formatting FAT filesystem (this takes ~60 seconds)..." );

    // Make filesystem.
    FRESULT r = f_mkfs( "", FM_FAT | FM_SFD, 0, workbuf, sizeof( workbuf ) );
    if ( r != FR_OK ) {
        ::Serial.print( "Error, f_mkfs failed with error code: " ); ::Serial.println( r, DEC );
        while ( 1 ) yield();
    }

    // mount to set disk label
    r = f_mount( &elmchamFatfs, "0:", 1 );
    if ( r != FR_OK ) {
        ::Serial.print( "Error, f_mount failed with error code: " ); ::Serial.println( r, DEC );
        while ( 1 ) yield();
    }

    // Setting label
    ::Serial.println( "Setting disk label to: " DISK_LABEL );
    r = f_setlabel( DISK_LABEL );
    if ( r != FR_OK ) {
        ::Serial.print( "Error, f_setlabel failed with error code: " ); ::Serial.println( r, DEC );
        while ( 1 ) yield();
    }

    // unmount
    f_unmount( "0:" );

    // sync to make sure all data is written to flash
    flash.syncBlocks();

    ::Serial.println( "Formatted flash!" );

    // Check new filesystem
    if ( !fatfs.begin( &flash ) ) {
        ::Serial.println( "Error, failed to mount newly formatted filesystem!" );
        while ( 1 ) delay( 1 );
    }

    // Done!
    ::Serial.println( "Flash chip successfully formatted with new empty filesystem!" );
    }

//--------------------------------------------------------------------+
// fatfs diskio
//--------------------------------------------------------------------+
extern "C"
{

    DSTATUS disk_status( BYTE pdrv )
    {
        (void)pdrv;
        return 0;
    }

    DSTATUS disk_initialize( BYTE pdrv )
    {
        (void)pdrv;
        return 0;
    }

    DRESULT disk_read(
        BYTE pdrv,		/* Physical drive nmuber to identify the drive */
        BYTE* buff,		/* Data buffer to store read data */
        DWORD sector,	/* Start sector in LBA */
        UINT count		/* Number of sectors to read */
    )
    {
        (void)pdrv;
        return flash.readBlocks( sector, buff, count ) ? RES_OK : RES_ERROR;
    }

    DRESULT disk_write(
        BYTE pdrv,			/* Physical drive nmuber to identify the drive */
        const BYTE* buff,	/* Data to be written */
        DWORD sector,		/* Start sector in LBA */
        UINT count			/* Number of sectors to write */
    )
    {
        (void)pdrv;
        return flash.writeBlocks( sector, buff, count ) ? RES_OK : RES_ERROR;
    }

    DRESULT disk_ioctl(
        BYTE pdrv,		/* Physical drive nmuber (0..) */
        BYTE cmd,		/* Control code */
        void* buff		/* Buffer to send/receive control data */
    )
    {
        (void)pdrv;

        switch ( cmd )
        {
        case CTRL_SYNC:
            flash.syncBlocks();
            return RES_OK;

        case GET_SECTOR_COUNT:
            *( (DWORD*)buff ) = flash.size() / 512;
            return RES_OK;

        case GET_SECTOR_SIZE:
            *( (WORD*)buff ) = 512;
            return RES_OK;

        case GET_BLOCK_SIZE:
            *( (DWORD*)buff ) = 8;    // erase block size in units of sector size
            return RES_OK;

        default:
            return RES_PARERR;
        }
    }

}

//------------------------------------------------------------------------------

// the loop function runs over and over again forever
void loop()
{
    // Nothing to do in the loop.
    delay( 100 );
}
