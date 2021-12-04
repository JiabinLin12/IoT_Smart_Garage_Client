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

static uint8_t advertising_set_handle = 0xff;
uint8_t connection_handle = 0xff;
/**Get private ble data*/
ble_data_struct_t *get_ble_data(){
  return (&ble_data);
}
/**Get private connection handle*/
uint8_t *get_connection_handle(){
  return (&connection_handle);
}

/**private ble data initialization*/
void ble_data_init(){
  ble_data_struct_t *ble_data_loc = get_ble_data();
  ble_data_loc->htm_indication_enable = false;
  ble_data_loc->connection_enable = false;
  ble_data_loc->indication_in_flight = false;
  ble_data_loc->motion_indication_enable = false;
  ble_data_loc->light_indication_enable = false;
  ble_data_loc->smart_garage_bonded = false;
  ble_data_loc->smart_garage_confirmation_require = false;
}

void server_char_status_flag_set(sl_bt_msg_t *evt, uint16_t chardb,bool *indication_enable, bool *indication_in_flight){
  if((evt->data.evt_gatt_server_characteristic_status.characteristic ==chardb)){
    if(evt->data.evt_gatt_server_characteristic_status.status_flags ==sl_bt_gatt_server_client_config){            //Configuration changed
      if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_server_indication) {     //Indication enable
         *indication_enable = true;
      } else if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_server_disable) { // Indication disable
         *indication_enable = false;
      }
    }else if(evt->data.evt_gatt_server_characteristic_status.status_flags ==sl_bt_gatt_server_confirmation){       //confirmation from client
         *indication_in_flight = false;
    }
  }
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
  uint8_t flag;
  switch(SL_BT_MSG_ID(evt->header)){
    //Events common to both servers and clients

    case sl_bt_evt_system_boot_id:
    #if DEVICE_IS_BLE_SERVER
        ble_data_init();
        displayInit();
        sc = sl_bt_advertiser_create_set(&advertising_set_handle);
        if(sc!=SL_STATUS_OK){
          LOG_ERROR("Advertiser handle create failed %d", sc);
        }


        sc = sl_bt_advertiser_set_timing( advertising_set_handle,
                                          MIN_ADV_INTERVAL,
                                          MAX_ADV_INTERVAL,
                                          ADV_DURATION,
                                          MAX_ADV_EVENT);
        if(sc!=SL_STATUS_OK){
          LOG_ERROR("Advertiser set timing failed %d", sc);
        }


        sc = sl_bt_advertiser_start(advertising_set_handle,
                                    sl_bt_advertiser_general_discoverable,
                                    sl_bt_advertiser_connectable_scannable);
        if(sc!=SL_STATUS_OK){
          LOG_ERROR("Advertiser start failed %d", sc);
        }

        sc = sl_bt_sm_delete_bondings();
        if(sc!=SL_STATUS_OK){
          LOG_ERROR("delete bondings failed %d", sc);
        }

        sc = sl_bt_sm_configure(SM_CONFIG_FLAG,sl_bt_sm_io_capability_displayyesno);
        if(sc!=SL_STATUS_OK){
          LOG_ERROR("security configure failed %d", sc);
        }

        sc = sl_bt_system_get_identity_address(&addr, &address_type);
        if(sc!=SL_STATUS_OK){
          LOG_ERROR("Get identity address failed %d", sc);
        }
        displayPrintf(DISPLAY_ROW_NAME, "Server");
        displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
        displayPrintf(DISPLAY_ROW_ASSIGNMENT, "A6");
        displayPrintf(DISPLAY_ROW_BTADDR, "%02X:%02X:%02X:%02X:%02X:%02X",addr.addr[0],
                      addr.addr[1],addr.addr[2],addr.addr[3],addr.addr[4], addr.addr[5]);
   #else
        sc = sl_bt_system_get_identity_address(&addr , &address_type);
        displayInit();
        displayPrintf(DISPLAY_ROW_NAME,"Client");
                displayPrintf(DISPLAY_ROW_BTADDR,"%02X:%02X:%02X:%02X:%02X:%02X",addr.addr[5],
                              addr.addr[4],
                              addr.addr[3],
                              addr.addr[2],
                              addr.addr[1],
                              addr.addr[0]);
                sc = sl_bt_scanner_set_mode(sl_bt_gap_1m_phy, 0);
                if(sc!=SL_STATUS_OK)
                  {
                  LOG_INFO("sl_bt_scanner_set_mode()  returned != 0 status=0x%04x", (unsigned int) sc);
                  }

                sc = sl_bt_scanner_set_timing(sl_bt_gap_1m_phy, 80, 40);
                if(sc!=SL_STATUS_OK)
                  {
                   LOG_INFO("sl_bt_scanner_set_timing()  returned != 0 status=0x%04x", (unsigned int) sc);
                  }
                displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
                //ble_data.PB0client_button_pressed=false;
                sc = sl_bt_connection_set_default_parameters(60,
                                                             60,
                                                             3,
                                                             850,
                                                             0,
                                                             0);
                if(sc!=SL_STATUS_OK)
                    {
                    LOG_INFO("sl_bt_scanner_default_parameters()  returned != 0 status=0x%04x", (unsigned int) sc);
                    }

                sc= sl_bt_scanner_start(sl_bt_gap_1m_phy, sl_bt_scanner_discover_generic);
                if(sc!=SL_STATUS_OK)
                    {
                    LOG_INFO("sl_bt_scanner_start()  returned != 0 status=0x%04x", (unsigned int) sc);
                    }
                sc = sl_bt_sm_delete_bondings();
                sc = sl_bt_sm_configure (
                     SM_CONFIG_FLAG,
                     sm_io_capability_displayyesno
                   );
                   if(sc!=SL_STATUS_OK)
                      {
                     LOG_INFO(" sl_bt_sm_configure() returned != 0 status=0x%04x", (unsigned int) sc);
                      }
        #endif

        break;

    case sl_bt_evt_scanner_scan_report_id:
    #if DEVICE_IS_BLE_SERVER==0
      printf("yes");
       for(int i=0;i<=5;i++)
      {
        if(evt->data.evt_scanner_scan_report.address.addr[i]!=addr.addr[i])
        {
         flag=0;
        }
        else
         flag=1;
       }
        if(flag!=0)
         {
          sc=sl_bt_connection_open(evt->data.evt_scanner_scan_report.address,
                                   evt->data.evt_scanner_scan_report.address_type,
                                   sl_bt_gap_1m_phy,
                                   NULL);
          if(sc!=SL_STATUS_OK)
            {
             LOG_INFO("sl_bt_connection_open()  returned != 0 status=0x%04x", (unsigned int) sc);
            }
            sc=sl_bt_scanner_stop(evt->data.evt_scanner_scan_report.address);
            if(sc!=SL_STATUS_OK)
             {
              LOG_INFO("sl_bt_scanner_stop()  returned != 0 status=0x%04x", (unsigned int) sc);
             }
            flag=0;
            }
       #endif
       break;
      /**Call when client connected**/
    case sl_bt_evt_connection_opened_id:
    #if DEVICE_IS_BLE_SERVER
      ble_data_loc->connection_enable = true;
      connection_handle = evt->data.evt_connection_opened.connection;
      sc = sl_bt_connection_set_parameters( connection_handle,
                                            MIN_CNT_INTERVAL,
                                            MAX_CNT_INVERVAL,
                                            SLAVE_LATENCY,
                                            SUPERVISON_TIMEOUT,
                                            MIN_CNT_EVT_LENGTH,
                                            MAX_CNT_EVT_LENGTH);
      if(sc!=SL_STATUS_OK){
        LOG_ERROR("connection set parameters failed %d", sc);
      }

      sc = sl_bt_advertiser_stop(advertising_set_handle);

      if(sc!=SL_STATUS_OK){
        LOG_ERROR("advertiser stop failed %d", sc);
      }

      displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
  #else
  ble_data.connection_handle=evt->data.evt_connection_opened.connection;
  #endif
      break;

      /**Call when a connection closed**/
    case sl_bt_evt_connection_closed_id:
   #if DEVICE_IS_BLE_SERVER
      ble_data_init();
      ble_data_loc->connection_enable = false;
      sc = sl_bt_sm_delete_bondings();
      if(sc!=SL_STATUS_OK){
        LOG_ERROR("delete bondings failed %d", sc);
      }

      sc = sl_bt_advertiser_start(
             advertising_set_handle,
             sl_bt_advertiser_general_discoverable,
             sl_bt_advertiser_connectable_scannable);
      if(sc!=SL_STATUS_OK){
        LOG_ERROR("connection enable failed %d", sc);
      }
      displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
      displayPrintf(DISPLAY_ROW_SMLVALUE, "");
      PbCirQ_init();
   #else
   #endif
      break;



    /**Call when parameters are set**/
    case sl_bt_evt_connection_parameters_id:
#if DEVICE_IS_BLE_SERVER

  #if (PRINT_PARAMS) // DOS: compiler error in formatting these parameters, included with ( ) and typecast to int
      LOG_INFO("interval=%d, latency=%d, timeout=%d",
               (int) (evt->data.evt_connection_parameters.interval*1.25),
               (int) (evt->data.evt_connection_parameters.latency),
               (int) (evt->data.evt_connection_parameters.timeout) );
   #endif
#else
#endif
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
#if DEVICE_IS_BLE_SERVER
       displayPrintf(DISPLAY_ROW_PASSKEY,"%d",evt->data.evt_sm_confirm_passkey.passkey);
       displayPrintf(DISPLAY_ROW_ACTION,"Confirm with PB0");
       if(evt->data.evt_sm_confirm_bonding.connection==connection_handle &&
           evt->data.evt_sm_confirm_bonding.bonding_handle!=SL_BT_INVALID_BONDING_HANDLE){
           ble_data_loc->smart_garage_confirmation_require = true;
       }
 #else
      displayPrintf(DISPLAY_ROW_PASSKEY, "%d", evt->data.evt_sm_confirm_passkey.passkey);
      displayPrintf(DISPLAY_ROW_ACTION, "Confirm with PB0");
#endif
      break;

    case sl_bt_evt_sm_bonded_id:
#if DEVICE_IS_BLE_SERVER
       displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");
       displayPrintf(DISPLAY_ROW_PASSKEY,"");
       displayPrintf(DISPLAY_ROW_ACTION,"");
       if(evt->data.evt_sm_bonded.connection==connection_handle&&
          evt->data.evt_sm_confirm_bonding.bonding_handle!=SL_BT_INVALID_BONDING_HANDLE){
         ble_data_loc->smart_garage_bonded  = true;
       }
#else
       displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");
       displayPrintf(DISPLAY_ROW_PASSKEY, "");
       displayPrintf(DISPLAY_ROW_ACTION, "");
#endif
       break;

     case  sl_bt_evt_sm_bonding_failed_id:
       ble_data_loc->smart_garage_bonded  = false;
       LOG_ERROR("Bonding Failed");
       break;
     case sl_bt_evt_gatt_service_id://Save the handles.Similar to samplecode.Character handle Service handle.
    #if DEVICE_IS_BLE_SERVER==0
       ble_data.sml_service_handle=evt->data.evt_gatt_service.service;
    #endif
       break;

     case sl_bt_evt_gatt_characteristic_id:
    #if DEVICE_IS_BLE_SERVER==0
       ble_data.sml_characteristic_handle=evt->data.evt_gatt_characteristic.characteristic;
    #endif
       break;


    case sl_bt_evt_system_external_signal_id:
#if DEVICE_IS_BLE_SERVER
      switch(evt->data.evt_system_external_signal.extsignals){
        case pb0_pressed:
          pushbutton0_response();
          break;
        default:
          break;
      }
#else
      switch(evt->data.evt_system_external_signal.extsignals){
          case pb0_pressed:
           {
          sl_bt_sm_passkey_confirm(ble_data.connection_handle, 1);
           }}
#endif
      break;

      /**Call when change of CCCD or indication confirmation received by client**/
    case sl_bt_evt_gatt_server_characteristic_status_id:
      server_char_status_flag_set(evt, gattdb_motion_state,&(ble_data_loc->light_indication_enable), &(ble_data_loc->indication_in_flight));
      server_char_status_flag_set(evt, gattdb_light_state, &(ble_data_loc->motion_indication_enable), &(ble_data_loc->indication_in_flight));
      break;

    case sl_bt_evt_system_soft_timer_id:
        displayUpdate();
        soft_timer_deq_indication();
        break;

    case sl_bt_evt_gatt_server_indication_timeout_id:
      LOG_ERROR("Client did not respond");
      break;
    case sl_bt_evt_gatt_procedure_completed_id:
    #if DEVICE_IS_BLE_SERVER==0
       if(evt->data.evt_gatt_procedure_completed.result==0x110F)
               {
                sc=sl_bt_sm_increase_security(ble_data.connection_handle);
               }
     #endif
     break;

  }
}
