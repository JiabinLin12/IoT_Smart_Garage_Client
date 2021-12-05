/*
 * client_state_machine.c
 *
 *  Created on: Nov 23, 2021
 *      Author: 17207
 */
#include "client_state_machine.h"
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"
extern const uint8_t sg_service[16];
extern const uint8_t sg_md_characteristic[16];
extern const uint8_t sg_ls_characteristic[16];
extern const uint8_t sg_cl_characteristic[16];

void discovery_state_machine(sl_bt_msg_t *evt)
{
  sl_status_t sc_c;

  discovery_state_t  current_state;
  static discovery_state_t  next_state=SCANNING;
  current_state=next_state;

  uint32_t signal;
  uint8_t *connection_handle_loc = get_connection_handle();
  ble_data_struct_t *ble_data_loc = get_ble_data();
  if (SL_BT_MSG_ID(evt->header) != sl_bt_evt_system_external_signal_id) {
        return;
  }

  signal = evt->data.evt_system_external_signal.extsignals;

  ble_data_struct_t *bleDataPtr = get_ble_data();

  switch (current_state){
      case SCANNING:
        next_state = SCANNING;
        if( signal== connection_open){
            const bd_addr server_addr = SERVER_BT_ADDRESS;
            displayPrintf(DISPLAY_ROW_BTADDR2, "%02X:%02X:%02X:%02X:%02X:%02X",
                                    server_addr.addr[0],server_addr.addr[1],server_addr.addr[2],
                                    server_addr.addr[3],server_addr.addr[4],server_addr.addr[5]);

            sc_c = sl_bt_gatt_discover_primary_services_by_uuid(*connection_handle_loc,
                                                                 sizeof(sg_service),
                                                                (const uint8_t*)sg_service);
            if(sc_c!=SL_STATUS_OK){
               LOG_INFO("sl_bt_gatt_discover_primary_services_by_uuid()  returned != 0 status=0x%04x", (unsigned int) sc_c);
            }
            LOG_INFO("To MD_DISCOVER_SERVICES");
            next_state=DISCOVER_SERVICES;
          }
      break;

      case DISCOVER_SERVICES:
        next_state = DISCOVER_SERVICES;
        if(signal==procedure_completed){
           LOG_INFO("(*bleDataPtr).sg_service_handle=%d",(*bleDataPtr).sg_service_handle);
           sc_c = sl_bt_gatt_discover_characteristics_by_uuid(*connection_handle_loc,
                                                              (*bleDataPtr).sg_service_handle,
                                                              sizeof(sg_md_characteristic),
                                                              (const uint8_t*)sg_md_characteristic);
           if(sc_c!=SL_STATUS_OK){
              LOG_INFO("sl_bt_gatt_discover_characteristics_by_uuid()  returned != 0 status=0x%04x", (unsigned int) sc_c);
           }else{
               LOG_INFO("To MD_DISCOVER_CHARACTERISTICS");
               next_state =MD_DISCOVER_CHARACTERISTICS;
           }
         }
      break;

      case MD_DISCOVER_CHARACTERISTICS:
        LOG_INFO("in MD_DISCOVER_CHARACTERISTICS");
        next_state = MD_DISCOVER_CHARACTERISTICS;
        if(signal==procedure_completed){
          sc_c = sl_bt_gatt_set_characteristic_notification(*connection_handle_loc,
                                                            (*bleDataPtr).md_characteristic_handle,
                                                            sl_bt_gatt_indication);

          if(sc_c!=SL_STATUS_OK){
              LOG_INFO("sl_bt_gatt_set_characteristic_notification()  returned != 0 status=0x%04x", (unsigned int) sc_c);
          }else{
              LOG_INFO("To MD_ENABLE_INDICATIONS");
              next_state=MD_ENABLE_INDICATIONS;
          }
         }
      break;

      case MD_ENABLE_INDICATIONS:
        next_state = MD_ENABLE_INDICATIONS;
        if(signal == procedure_completed){
            sc_c = sl_bt_gatt_discover_characteristics_by_uuid(*connection_handle_loc,
                                                             (*ble_data_loc).sg_service_handle,
                                                              sizeof(sg_ls_characteristic),
                                                             (const uint8_t*)sg_ls_characteristic);
            if(sc_c!=SL_STATUS_OK){
                LOG_ERROR("discover char failed %d", sc_c);
            }else{
              LOG_INFO("To LS_DISCOVER_CHARACTERISTICS");
              next_state = LS_DISCOVER_CHARACTERISTICS;
            }
         }
      break;


      case LS_DISCOVER_CHARACTERISTICS:
        next_state = LS_DISCOVER_CHARACTERISTICS;
        if(signal == procedure_completed){
            sc_c = sl_bt_gatt_set_characteristic_notification(*connection_handle_loc,
                                                              bleDataPtr->ls_characteristic_handle,
                                                              sl_bt_gatt_indication);

          if(sc_c!=SL_STATUS_OK){
              LOG_ERROR("sl_bt_gatt_set_characteristic_notification()  returned != 0 status=0x%04x", (unsigned int) sc_c);
           }else{
               LOG_INFO("To LS_ENABLE_INDICATIONS");
               next_state = LS_ENABLE_INDICATIONS;
           }
        }
      break;

      case LS_ENABLE_INDICATIONS:
        next_state = LS_ENABLE_INDICATIONS;
        if(signal == procedure_completed){
              sc_c = sl_bt_gatt_discover_characteristics_by_uuid(*connection_handle_loc,
                                                               (*ble_data_loc).sg_service_handle,
                                                                sizeof(sg_cl_characteristic),
                                                               (const uint8_t*)sg_cl_characteristic);
              if(sc_c!=SL_STATUS_OK){
                  LOG_ERROR("discover char failed %d", sc_c);
              }else{
                  LOG_INFO("To CL_DISCOVER_CHARACTERISTICS");

                next_state = CL_DISCOVER_CHARACTERISTICS;
              }
         }
      break;

      case CL_DISCOVER_CHARACTERISTICS:
        next_state = CL_DISCOVER_CHARACTERISTICS;
        if(signal == procedure_completed){
            sc_c = sl_bt_gatt_set_characteristic_notification(*connection_handle_loc,
                                                              bleDataPtr->cl_characteristic_handle,
                                                              sl_bt_gatt_indication);

            if(sc_c!=SL_STATUS_OK){
                LOG_ERROR("sl_bt_gatt_set_characteristic_notification()  returned != 0 status=0x%04x", (unsigned int) sc_c);
            }else{
                LOG_INFO("To CL_ENABLE_INDICATION");
                next_state = CL_ENABLE_INDICATION;
            }
        }
        break;

      case CL_ENABLE_INDICATION:
        next_state = CL_ENABLE_INDICATION;
        if(signal == procedure_completed){
            next_state = RUNNING;
            LOG_INFO("To RUNNING");

        }
        break;

      case RUNNING:
        next_state = RUNNING;
        LOG_INFO("RUNNING");
        if(ble_data_loc->smart_garage_bonded)
          displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");

        if(signal==connection_close){
           sc_c = sl_bt_sm_delete_bondings();
           if(sc_c!=SL_STATUS_OK){
             LOG_ERROR("bonding handle delete failed %d", sc_c);
           }

           sc_c = sl_bt_scanner_start(sl_bt_gap_1m_phy, sl_bt_scanner_discover_generic);
           if(sc_c!=SL_STATUS_OK){
              LOG_INFO("sl_bt_scanner_start()  returned != 0 status=0x%04x", (unsigned int) sc_c);
           }
           ble_data_loc->smart_garage_bonded  = false;
           displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
           displayPrintf(DISPLAY_ROW_BTADDR2, "");
           next_state =SCANNING;
        }
      break;

      default:
        break;
     }//end of switch
}
