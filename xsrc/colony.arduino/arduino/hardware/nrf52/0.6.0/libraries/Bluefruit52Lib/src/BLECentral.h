/**************************************************************************/
/*!
    @file     BLECentral.h
    @author   hathach

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2017, Adafruit Industries (adafruit.com)
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
#ifndef BLECENTRAL_H_
#define BLECENTRAL_H_

#include <Arduino.h>
#include "bluefruit_common.h"

#include "BLEUuid.h"
#include "BLECharacteristic.h"
#include "BLEClientCharacteristic.h"
#include "BLEService.h"

#include "BLEClientService.h"


class AdafruitBluefruit;

class BLECentral
{
  public:
    typedef void (*scan_callback_t       ) (ble_gap_evt_adv_report_t*);
    typedef void (*connect_callback_t    ) (uint16_t conn_handle);
    typedef void (*disconnect_callback_t ) (uint16_t conn_handle, uint8_t reason);

    BLECentral(void); // Constructor
    void begin(void);

    /*------------------------------------------------------------------*/
    /* Scan & Parser
     *------------------------------------------------------------------*/
    void     setScanCallback(scan_callback_t fp);
    bool     startScanning(uint16_t timeout = 0);
    bool     stopScanning(void);

    uint8_t* extractScanData(uint8_t const* scandata, uint8_t scanlen, uint8_t type, uint8_t* result_len);
    uint8_t* extractScanData(const ble_gap_evt_adv_report_t* report, uint8_t type, uint8_t* result_len);
    bool     checkUuidInScan(const ble_gap_evt_adv_report_t* adv_report, BLEUuid ble_uuid);

    /*------------------------------------------------------------------*/
    /*
     *------------------------------------------------------------------*/
    bool     connect(const ble_gap_evt_adv_report_t* adv_report,
                     uint16_t min_conn_interval = BLE_GAP_CONN_MIN_INTERVAL_DFLT,
                     uint16_t max_conn_interval = BLE_GAP_CONN_MAX_INTERVAL_DFLT);

    bool     connect(const ble_gap_addr_t *peer_addr,
                     uint16_t min_conn_interval = BLE_GAP_CONN_MIN_INTERVAL_DFLT,
                     uint16_t max_conn_interval = BLE_GAP_CONN_MAX_INTERVAL_DFLT);

    bool     connected  (void);
    uint16_t connHandle (void);

    /*------------------------------------------------------------------*/
    /* CALLBACKS
     *------------------------------------------------------------------*/
    void setConnectCallback   ( connect_callback_t    fp);
    void setDisconnectCallback( disconnect_callback_t fp);

  private:
    uint16_t _conn_hdl;

    ble_gap_scan_params_t _scan_param;
    scan_callback_t       _scan_cb;

    connect_callback_t    _connect_cb;
    disconnect_callback_t _disconnect_cb;

    bool  _checkUuidInScan(const ble_gap_evt_adv_report_t* report, const uint8_t uuid[], uint8_t uuid_len);

    void  _event_handler(ble_evt_t* evt);

    friend class AdafruitBluefruit;
};



#endif /* BLECENTRAL_H_ */
