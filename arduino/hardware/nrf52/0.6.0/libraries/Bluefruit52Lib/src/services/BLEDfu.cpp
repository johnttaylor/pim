/**************************************************************************/
/*!
    @file     BLEDfu.cpp
    @author   hathach

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2016, Adafruit Industries (adafruit.com)
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

#include "bluefruit.h"

#define DFU_REV_APPMODE     0x0001

/* DFU Serivce : 00001530-1212-EFDE-1523-785FEABCD123
 * DFU Control : 00001531-1212-EFDE-1523-785FEABCD123
 * DFU Packet  : 00001532-1212-EFDE-1523-785FEABCD123
 * DFU Revision: 00001534-1212-EFDE-1523-785FEABCD123
 */

const uint8_t UUID128_SVC_DFU_OTA[16] =
{
    0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,
    0xDE, 0xEF, 0x12, 0x12, 0x30, 0x15, 0x00, 0x00
};


const uint8_t UUID128_CHR_DFU_CONTROL[16] =
{
    0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,
    0xDE, 0xEF, 0x12, 0x12, 0x31, 0x15, 0x00, 0x00
};

const uint8_t UUID128_CHR_DFU_PACKET[16] =
{
    0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,
    0xDE, 0xEF, 0x12, 0x12, 0x32, 0x15, 0x00, 0x00
};

const uint8_t UUID128_CHR_DFU_REVISON[16] =
{
    0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,
    0xDE, 0xEF, 0x12, 0x12, 0x34, 0x15, 0x00, 0x00
};

extern "C" void bootloader_util_app_start(uint32_t start_addr);

static uint16_t crc16(const uint8_t* data_p, uint8_t length)
{
  uint8_t x;
  uint16_t crc = 0xFFFF;

  while ( length-- )
  {
    x = crc >> 8 ^ *data_p++;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((uint16_t) (x << 12)) ^ ((uint16_t) (x << 5)) ^ ((uint16_t) x);
  }
  return crc;
}

static void bledfu_control_wr_authorize_cb(BLECharacteristic& chr, ble_gatts_evt_write_t* request)
{
  if ( (request->handle == chr.handles().value_handle)  &&
       (request->op != BLE_GATTS_OP_PREP_WRITE_REQ)     &&
       (request->op != BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) &&
       (request->op != BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
  {
    ble_gatts_rw_authorize_reply_params_t reply = { .type = BLE_GATTS_AUTHORIZE_TYPE_WRITE };

    if ( !chr.notifyEnabled() )
    {
      reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_CPS_CCCD_CONFIG_ERROR;
      sd_ble_gatts_rw_authorize_reply(Bluefruit.connHandle(), &reply);
      return;
    }

    reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
    sd_ble_gatts_rw_authorize_reply(Bluefruit.connHandle(), &reply);

    enum { START_DFU  = 1 };
    if ( request->data[0] == START_DFU )
    {
      // Peer data information so that bootloader could re-connect after reboot
      typedef struct {
        ble_gap_addr_t    addr;
        ble_gap_irk_t     irk;
        ble_gap_enc_key_t enc_key;
        uint8_t           sys_attr[8];
        uint16_t          crc16;
      }peer_data_t;

      /* Save Peer data
       * Peer data address is defined in bootloader linker @0x20007F80
       * - If bonded : save Security information
       * - Otherwise : save Address for direct advertising
       *
       * TODO may force bonded only for security reason
       */
      peer_data_t* peer_data = (peer_data_t*) (0x20007F80UL);
      varclr(peer_data);

      uint16_t sysattr_len = sizeof(peer_data->sys_attr);
      sd_ble_gatts_sys_attr_get(Bluefruit.connHandle(), peer_data->sys_attr, &sysattr_len, BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS);

      if ( !Bluefruit.connPaired() )
      {
        peer_data->addr  = Bluefruit.peerAddr();
      }else
      {
        peer_data->addr    = Bluefruit._bond_data.peer_id.id_addr_info;
        peer_data->irk     = Bluefruit._bond_data.peer_id.id_info;

        peer_data->enc_key = Bluefruit._bond_data.own_enc;
      }
      peer_data->crc16 = crc16((uint8_t*) peer_data, offsetof(peer_data_t, crc16));

      // Initiate DFU Sequence and reboot into DFU OTA mode
      Bluefruit.disconnect();
      Bluefruit.Advertising.stop();

      // Set GPReset to DFU OTA
      enum { DFU_OTA_MAGIC = 0xB1 };
      sd_power_gpregret_clr(0xFF);
      VERIFY_STATUS( sd_power_gpregret_set(DFU_OTA_MAGIC), RETURN_VOID);
      VERIFY_STATUS( sd_softdevice_disable(), RETURN_VOID );

      // Disable Systick to prevent Interrupt happens after changing vector table
      bitClear(SysTick->CTRL, SysTick_CTRL_ENABLE_Pos);

      // Disable all interrupts
      NVIC_ClearPendingIRQ(SD_EVT_IRQn);
      for(int i=0; i <= FPU_IRQn; i++)
      {
        NVIC_DisableIRQ( (IRQn_Type) i );
      }

      VERIFY_STATUS( sd_softdevice_vector_table_base_set(NRF_UICR->NRFFW[0]), RETURN_VOID);

      __set_CONTROL(0); // switch to MSP, required if using FreeRTOS
      bootloader_util_app_start(NRF_UICR->NRFFW[0]);
    }
  }
}

BLEDfu::BLEDfu(void) : BLEService(UUID128_SVC_DFU_OTA), _chr_control(UUID128_CHR_DFU_CONTROL)
{

}

err_t BLEDfu::begin(void)
{
  VERIFY_STATUS( this->addToGatt() );

  // No need to keep packet & revision characteristics
  BLECharacteristic chr_packet(UUID128_CHR_DFU_PACKET);
  chr_packet.setTempMemory();
  chr_packet.setProperties(CHR_PROPS_WRITE_WO_RESP);
  chr_packet.setMaxLen(20);
  VERIFY_STATUS( chr_packet.begin() );

  _chr_control.setProperties(CHR_PROPS_WRITE | CHR_PROPS_NOTIFY);
  _chr_control.setMaxLen(23);
  _chr_control.setWriteAuthorizeCallbak(bledfu_control_wr_authorize_cb);
  VERIFY_STATUS( _chr_control.begin() );

  BLECharacteristic chr_revision(UUID128_CHR_DFU_REVISON);
  chr_packet.setTempMemory();
  chr_revision.setProperties(CHR_PROPS_READ);
  chr_revision.setFixedLen(2);
  VERIFY_STATUS( chr_revision.begin());
  chr_revision.write( (uint16_t) DFU_REV_APPMODE);

  return ERROR_NONE;
}
