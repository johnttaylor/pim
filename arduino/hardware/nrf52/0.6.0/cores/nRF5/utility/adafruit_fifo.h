/**************************************************************************/
/*!
    @file     adafruit_fifo.h
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

#ifndef _Adafruit_FIFO_H_
#define _Adafruit_FIFO_H_

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>

class Adafruit_FIFO
{
  private:
             uint8_t* _buffer       ; ///< buffer pointer
    const    uint16_t _depth        ; ///< max items
    const    uint8_t  _item_size    ; ///< size of each item
    const    bool     _overwritable ; ///< Overwrite when full
    volatile uint16_t _count        ; ///< number of items in queue
    volatile uint16_t _wr_idx       ; ///< write pointer
    volatile uint16_t _rd_idx       ; ///< read pointer

    SemaphoreHandle_t _mutex;

    bool _mutex_lock(bool isr);
    bool _mutex_unlock(bool isr);

  public:
    // Constructor
    Adafruit_FIFO(uint16_t depth, uint8_t item_size, bool overwrite = false);

    virtual ~Adafruit_FIFO();


    void clear(void);
    bool peek(void* buffer);
    bool peekAt(uint16_t position, void * p_buffer);

    uint16_t write(void const* item);
    uint16_t write(void const * data, uint16_t n);

    uint16_t read(void* buffer);
    uint16_t read(void * buffer, uint16_t n);

//    uint16_t read_isr(void* buffer);
//    uint16_t read_isr(void * buffer, uint16_t n);

    inline bool     empty(void)     { return _count == 0;      }
    inline bool     full(void)      { return _count == _depth; }
    inline uint16_t count(void)     { return _count;           }
    inline uint16_t remaining(void) { return _depth - _count;  }
};

#endif /* _Adafruit_FIFO_H_ */
