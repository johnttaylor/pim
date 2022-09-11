/**************************************************************************/
/*!
    @file     adafruit_fifo.cpp
    @author   hathach

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2015, Adafruit Industries (adafruit.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#include "adafruit_fifo.h"
#include <string.h>

/******************************************************************************/
/*!
    @brief  Constructor

    @param[in] depth
               Maximum number of items can be hold in buffer
    @param[in] item_size
               Number of bytes of each item
    @param[in] overwrite
               Should the buffer is overwritten to the first item when it is full
*/
/******************************************************************************/
Adafruit_FIFO::Adafruit_FIFO(uint16_t depth, uint8_t item_size, bool overwrite)
  : _depth(depth), _item_size(item_size), _overwritable(overwrite)
{
  _buffer = (uint8_t*) malloc(item_size*depth);
  _count = _wr_idx = _rd_idx = 0;

  _mutex = xSemaphoreCreateMutex();
}


/**
 * Destructor
 * @return
 */
Adafruit_FIFO::~Adafruit_FIFO()
{
  vSemaphoreDelete(_mutex);
  rtos_free(_buffer); // use free_name for debug
}

bool Adafruit_FIFO::_mutex_lock(bool isr)
{
  (void) isr;
  return xSemaphoreTake(_mutex, portMAX_DELAY);
}

bool Adafruit_FIFO::_mutex_unlock(bool isr)
{
  (void) isr;
  return xSemaphoreGive(_mutex);
}


/******************************************************************************/
/*!
    @brief  Clear the FIFO
*/
/******************************************************************************/
void Adafruit_FIFO::clear(void)
{
  _mutex_lock(false);
  _rd_idx = _wr_idx = _count = 0;
  _mutex_unlock(false);
}

/******************************************************************************/
/*!
    @brief  Write an item to the FIFO

    @param[in] item
               Memory address of the item
*/
/******************************************************************************/
uint16_t Adafruit_FIFO::write(void const* item)
{
  if ( full() && !_overwritable ) return 0;

  _mutex_lock(false);

  memcpy( _buffer + (_wr_idx * _item_size),
          item,
          _item_size);

  _wr_idx = (_wr_idx + 1) % _depth;

  if ( full() )
  {
    _rd_idx = _wr_idx; // keep the full state (rd == wr && len = size)
  }
  else
  {
    _count++;
  }

  _mutex_unlock(false);

  return 1;
}

/******************************************************************************/
/*!
    @brief  Write array of items to the FIFO

    @param[in] data
               Memory address of the item's array
    @param[in] n
               Number of items to write

    @return    Number of written items
*/
/******************************************************************************/
uint16_t Adafruit_FIFO::write(void const * data, uint16_t n)
{
  if ( n == 0 ) return 0;

  uint8_t* buf = (uint8_t*) data;

  uint16_t len = 0;
  while( (len < n) && write(buf) )
  {
    len++;
    buf += _item_size;
  }

  return len;
}

/******************************************************************************/
/*!
    @brief  Read an item from FIFO

    @param[in] buffer
               Memory address to store item
*/
/******************************************************************************/
uint16_t Adafruit_FIFO::read(void* buffer)
{
  if( empty() ) return 0;

  _mutex_lock(false);

  memcpy(buffer,
         _buffer + (_rd_idx * _item_size),
         _item_size);
  _rd_idx = (_rd_idx + 1) % _depth;
  _count--;

  _mutex_unlock(false);

  return 1;
}

/******************************************************************************/
/*!
    @brief  Read multiple items to an array

    @param[in] buffer
               Memory address of the item's array
    @param[in] n
               Number of items to read

    @return    Number of read items
*/
/******************************************************************************/
uint16_t Adafruit_FIFO::read(void * buffer, uint16_t n)
{
  if( n == 0 ) return 0;

  uint8_t* buf = (uint8_t*) buffer;

  uint16_t len = 0;
  while( (len < n) && read(buf) )
  {
    len++;
    buf += _item_size;
  }

  return len;
}

/******************************************************************************/
/*!
    @brief  Read an item without removing it from the FIFO

    @param[in] buffer
               Memory address to store item
*/
/******************************************************************************/
bool Adafruit_FIFO::peek(void* buffer)
{
  if( empty() ) return false;

  memcpy(buffer,
         _buffer + (_rd_idx * _item_size),
         _item_size);

  return true;
}


/******************************************************************************/
/*!
    @brief  Read an item without removing it from the FIFO at the specific index

    @param[in] position
               Position to read from in the FIFO buffer

    @param[in] buffer
               Memory address to store item
*/
/******************************************************************************/
bool Adafruit_FIFO::peekAt(uint16_t position, void * p_buffer)
{
  if( empty() || (position >= _count) ) return false;

  uint16_t index = (_rd_idx + position) % _depth; // rd_idx is position=0
  memcpy(p_buffer,
         _buffer + (index * _item_size),
         _item_size);

  return true;
}

