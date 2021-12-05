/***********************************************************************
 *@file        ble.c
 *
 *@version     0.0.1
 *
 *@brief       function to handle ble event
 *@brief
 *
 *@author      Jiabin Lin, jili9036@Colorado.edu
 *
 *@date        Sep 26, 2021
 *
 *@institution University of Colorado Boulder (UCB)
 *
 *@course      ECEN 5823-001: IoT Embedded Firmware (Fall 2021)
 *
 *@instructor  David Sluiter
 *
 *@assignment  final project
 *
 *@due         Dec 8th, 2021
 *
 *@resources   Utilized Silicon Labs' EMLIB peripheral libraries to
 *             implement functionality.
 *
 *
 *@copyright   All rights reserved. Distribution allowed only for the
 *             use of assignment grading. Use of code excerpts allowed at the
 *             discretion of author. Contact for permission.  */
#include "ble.h"
#include "app_assert.h"

// Include logging for this file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"
#include "autogen/gatt_db.h"

#include "ble_device_type.h"

ble_data_struct_t ble_data;

uint8_t connection_handle = 0xff;
/**Get private ble data*/
ble_data_struct_t *get_ble_data(){
  return (&ble_data);
}
/**Get private connection handle*/
uint8_t *get_connection_handle(){
  return (&connection_handle);
}


//00000003-38c8-433e-87ec-652a2d136289
const uint8_t sg_service[16]          = { 0x89, 0x62,0x13,0x2D,0x2A,0x65,0xEC,0x87,
                                          0x3E,0x43,0xC8,0x38,0x03,0x00,0x00,0x00 };
//00000004-38c8-433e-87ec-652a2d136289
const uint8_t sg_md_characteristic[16] = {0x89, 0x62,0x13,0x2D,0x2A,0x65,0xEC,0x87,
                                          0x3E,0x43,0xC8,0x38,0x04,0x00,0x00,0x00 };
//00000005-38c8-433e-87ec-652a2d136289
const uint8_t sg_ls_characteristic[16] = {0x89, 0x62,0x13,0x2D,0x2A,0x65,0xEC,0x87,
                                          0x3E,0x43,0xC8,0x38,0x05,0x00,0x00,0x00 };
//00000006-38c8-433e-87ec-652a2d136289
const uint8_t sg_cl_characteristic[16] = {0x89, 0x62,0x13,0x2D,0x2A,0x65,0xEC,0x87,
                                          0x3E,0x43,0xC8,0x38,0x06,0x00,0x00,0x00 };

/**
 * Verify the uuid matches matches the expected uuid
 * @param[in] get_uuid: the uuid received
 * @param[in] uuid_expect: the uuid expected to match
 * @param[in] len: length of the uuid
 * @return: return true if they matches
 * **/
bool uuid_verification(uint8_t get_uuid[], const uint8_t uuid_expect[], uint8_t len){
  for(int i=0; i<len; i++){
      LOG_INFO("%x", get_uuid[i]);
      if(get_uuid[i]!=uuid_expect[i]){
          LOG_INFO("xxxx");
          return false;
      }
  }
  LOG_INFO("----");
  return true;
}

bool address_verification(bd_addr addr, uint8_t addr_type){
  const bd_addr server_addr = SERVER_BT_ADDRESS;
  if(addr_type == 0) {
      for(int i=0; i<6;i++){
          if(server_addr.addr[i] != addr.addr[i])
            return false;
      }
      return true;
  }
  return false;
}

void client_response(sl_bt_msg_t *evt, uint8_t char_handle, uint8_t option){
  sl_status_t sc;

  if(char_handle == evt->data.evt_gatt_characteristic.characteristic){
      if(evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_indication){
        sc = sl_bt_gatt_send_characteristic_confirmation(connection_handle);
        if(sc!=SL_STATUS_OK){
          LOG_ERROR("pb characteristic confirmation failed %d", sc);
        }
      }
      if(evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_read_response ||
         evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_indication){
        switch(option){
          case 0:
            if(evt->data.evt_gatt_characteristic_value.value.data[0]==0){
               displayPrintf(DISPLAY_ROW_9, "Motion Detected");
            }else if (evt->data.evt_gatt_characteristic_value.value.data[0]==1){
               displayPrintf(DISPLAY_ROW_9, "No Motion Detected");
            }
            break;
          case 1:
            if(evt->data.evt_gatt_characteristic_value.value.data[0]==0){
                displayPrintf(DISPLAY_ROW_9, "Light is On");
            }else if (evt->data.evt_gatt_characteristic_value.value.data[0]==1){
                displayPrintf(DISPLAY_ROW_9, "Light is Off");
            }
            break;
          case 2:
            if(evt->data.evt_gatt_characteristic_value.value.data[0]==0){
                displayPrintf(DISPLAY_ROW_9, "Car is on position");
            }else if (evt->data.evt_gatt_characteristic_value.value.data[0]==1){
                displayPrintf(DISPLAY_ROW_9, "Car is not on position");
            }
            break;
          default:
            displayPrintf(DISPLAY_ROW_9, "");
        }
      }
    }
}


/**private ble data initialization*/
void ble_data_init(){
  ble_data_struct_t *ble_data_loc = get_ble_data();
  ble_data_loc->smart_garage_client_passkey_confirmation_require = false;

  ble_data_loc->smart_garage_bonded = false;
  ble_data_loc->sg_service_handle = -1;
  ble_data_loc->ls_characteristic_handle = -1;
  ble_data_loc->cl_characteristic_handle = -1;
  ble_data_loc->md_characteristic_handle = -1;

}



/**
 * Handle ble event based on the ble stack signal
 * @param[in] ble stack signal
 * */
void handle_ble_event(sl_bt_msg_t *evt){
  ble_data_struct_t *ble_data_loc = get_ble_data();
  sl_status_t sc;
  bd_addr addr;
  uint8_t address_type;
  switch(SL_BT_MSG_ID(evt->header)){
    //Events common to both servers and clients

    case sl_bt_evt_system_boot_id:
        displayInit();
        ble_data_init();
        sc = sl_bt_sm_delete_bondings();
        if(sc!=SL_STATUS_OK){
            LOG_ERROR(" delete bonding failed status=0x%04x", (unsigned int) sc);
        }

        sc = sl_bt_sm_configure (SM_CONFIG_FLAG,sm_io_capability_displayyesno);
        if(sc!=SL_STATUS_OK){
            LOG_ERROR(" sl_bt_sm_configure() returned != 0 status=0x%04x", (unsigned int) sc);
        }

        sc = sl_bt_scanner_set_mode(sl_bt_gap_1m_phy, SCAN_PASSIVE);
        if(sc!=SL_STATUS_OK){
            LOG_ERROR("sl_bt_scanner_set_mode()  returned != 0 status=0x%04x", (unsigned int) sc);
        }

        sc = sl_bt_scanner_set_timing(sl_bt_gap_1m_phy, SCAN_INTERVAL, SCAN_WINDOW);
        if(sc!=SL_STATUS_OK){
            LOG_ERROR("sl_bt_scanner_set_timing()  returned != 0 status=0x%04x", (unsigned int) sc);
        }
        sc = sl_bt_connection_set_default_parameters(MIN_CNT_INTERVAL,
                                                     MAX_CNT_INVERVAL,
                                                     SLAVE_LATENCY,
                                                     SUPERVISON_TIMEOUT,
                                                     MIN_CNT_EVT_LENGTH,
                                                     MAX_CNT_EVT_LENGTH);
        if(sc!=SL_STATUS_OK){
          LOG_ERROR("Set default parameters failed %x", sc);
        }


        sc= sl_bt_scanner_start(sl_bt_gap_1m_phy, sl_bt_scanner_discover_generic);
        if(sc!=SL_STATUS_OK){
            LOG_ERROR("sl_bt_scanner_start()  returned != 0 status=0x%04x", (unsigned int) sc);
        }

        sc = sl_bt_system_get_identity_address(&addr , &address_type);
        displayPrintf(DISPLAY_ROW_NAME,"Client");
        displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
        displayPrintf(DISPLAY_ROW_BTADDR,"%02X:%02X:%02X:%02X:%02X:%02X",addr.addr[0],addr.addr[1],addr.addr[2],
                      addr.addr[3],addr.addr[4],addr.addr[5]);
        break;

     /**Call when client connected**/
     case sl_bt_evt_connection_opened_id:
         connection_handle=evt->data.evt_connection_opened.connection;
         sl_bt_external_signal(connection_open);
         displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
        break;


       /**Call when a connection closed**/
    case sl_bt_evt_connection_closed_id:
        sl_bt_external_signal(connection_close);
        break;

    case sl_bt_evt_scanner_scan_report_id:
      if((address_verification(evt->data.evt_scanner_scan_report.address,
                                 evt->data.evt_scanner_scan_report.address_type)==true) &&
             (evt->data.evt_scanner_scan_report.packet_type==0))
           {
             sc = sl_bt_scanner_stop();
             if(sc!=SL_STATUS_OK){
               LOG_ERROR("scanner stop failed %d", sc);
             }
             sc = sl_bt_connection_open(evt->data.evt_scanner_scan_report.address,
                                        evt->data.evt_scanner_scan_report.address_type,
                                        sl_bt_gap_1m_phy,
                                        NULL);
             if(sc!=SL_STATUS_OK){
               LOG_ERROR("connection open failed %d", sc);
             }
         }
       break;


      //call when user push the read characteristic
     case sl_bt_evt_sm_confirm_bonding_id:
        if(evt->data.evt_sm_confirm_bonding.connection==connection_handle){
          sc = sl_bt_sm_bonding_confirm(connection_handle,1);
        }else{
          sc = sl_bt_sm_bonding_confirm(connection_handle,0);
        }

        if(sc!=SL_STATUS_OK){
          LOG_ERROR("bonding confirm failed %d", sc);
        }
        break;

     case sl_bt_evt_sm_confirm_passkey_id:
       displayPrintf(DISPLAY_ROW_PASSKEY,"%d",evt->data.evt_sm_confirm_passkey.passkey);
       displayPrintf(DISPLAY_ROW_ACTION,"Confirm with PB0");
       if(evt->data.evt_sm_confirm_bonding.connection==connection_handle &&
           evt->data.evt_sm_confirm_bonding.bonding_handle!=SL_BT_INVALID_BONDING_HANDLE){
           ble_data_loc->smart_garage_client_passkey_confirmation_require = true;
       }

       break;

    case sl_bt_evt_sm_bonded_id:
       displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");
       displayPrintf(DISPLAY_ROW_PASSKEY, "");
       displayPrintf(DISPLAY_ROW_ACTION, "");
       if(evt->data.evt_sm_bonded.connection==connection_handle&&
          evt->data.evt_sm_confirm_bonding.bonding_handle!=SL_BT_INVALID_BONDING_HANDLE){
         ble_data_loc->smart_garage_bonded  = true;
       }
       break;

     case  sl_bt_evt_sm_bonding_failed_id:
       ble_data_loc->smart_garage_bonded  = false;
       LOG_ERROR("Bonding Failed");
       break;

     case sl_bt_evt_system_external_signal_id:
       //Add push button stuff
       break;

     case sl_bt_evt_gatt_server_indication_timeout_id:
       LOG_ERROR("Client did not respond");
       break;

     case sl_bt_evt_system_soft_timer_id:
         displayUpdate();
         break;

    //discovered services from a remote gatt database
    case sl_bt_evt_gatt_service_id:
      if(uuid_verification(evt->data.evt_gatt_service.uuid.data,sg_service,16)){
          ble_data_loc->sg_service_handle =  evt->data.evt_gatt_service.service;
          LOG_INFO("sg_service_handle=%d",ble_data_loc->sg_service_handle);
      }
      break;

    //discovered characteristic from a remote gatt database
    case sl_bt_evt_gatt_characteristic_id:
      LOG_INFO("%d Hello",evt->data.evt_gatt_characteristic.uuid.len);
      if(uuid_verification(evt->data.evt_gatt_characteristic.uuid.data,sg_md_characteristic,16)){
          ble_data_loc->md_characteristic_handle = evt->data.evt_gatt_characteristic.characteristic;
      }
      if(uuid_verification(evt->data.evt_gatt_characteristic.uuid.data,sg_ls_characteristic,16)){
          ble_data_loc->ls_characteristic_handle = evt->data.evt_gatt_characteristic.characteristic;
      }
      if(uuid_verification(evt->data.evt_gatt_characteristic.uuid.data,sg_cl_characteristic,16)){
          ble_data_loc->cl_characteristic_handle = evt->data.evt_gatt_characteristic.characteristic;
      }
      break;

    case sl_bt_evt_gatt_procedure_completed_id:
      if(evt->data.evt_gatt_procedure_completed.connection==connection_handle
           && evt->data.evt_gatt_procedure_completed.result==0){
           sl_bt_external_signal(procedure_completed);
       }
       if(evt->data.evt_gatt_procedure_completed.connection==connection_handle
           && (evt->data.evt_gatt_procedure_completed.result==SL_STATUS_BT_ATT_INSUFFICIENT_ENCRYPTION)){
           sc = sl_bt_sm_increase_security(connection_handle);
           if(sc!=SL_STATUS_OK){
               LOG_ERROR("increase security failed %d", sc);
           }
       }
       break;

       //Indication or notification is received from the remote gatt server
      case sl_bt_evt_gatt_characteristic_value_id:
         client_response(evt, ble_data_loc->md_characteristic_handle,0);
         client_response(evt, ble_data_loc->ls_characteristic_handle,1);
         client_response(evt, ble_data_loc->cl_characteristic_handle,2);
         break;

  }
}
