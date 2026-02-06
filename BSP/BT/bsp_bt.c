#include "bsp_bt.h"
#include "event.h"
#include "soft_time.h"
#include <stdio.h>
#include <string.h>

volatile uint8_t bt_rx_buffer[BT_RX_BUFFER_SIZE];
volatile uint8_t bt_rx_len = 0;
volatile uint8_t bt_rx_flag = 0;

static uint8_t temp_buffer[BT_RX_BUFFER_SIZE];

void BT_Init(void)
{
    bt_rx_len = 0;
    bt_rx_flag = 0;
    memset((void *)bt_rx_buffer, 0, BT_RX_BUFFER_SIZE);
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart2, (uint8_t *)bt_rx_buffer, BT_RX_BUFFER_SIZE);
}

void BT_IRQHandler(void)
{
    if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
        
        HAL_UART_AbortReceive_IT(&huart2);
        
        bt_rx_len = BT_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart2.hdmarx);
        bt_rx_flag = 1;
        
        HAL_UART_Receive_DMA(&huart2, (uint8_t *)bt_rx_buffer, BT_RX_BUFFER_SIZE);
    }
}

void BT_Process(void)
{
    if(bt_rx_flag)
    {
        bt_rx_flag = 0;
        
        memcpy(temp_buffer, (void *)bt_rx_buffer, bt_rx_len);
        temp_buffer[bt_rx_len] = '\0';
        
        BT_CmdPacket_t packet;
        if(BT_ParseCommand(temp_buffer, bt_rx_len, &packet))
        {
            Event_Queue(EVT_BT_CMD, (uint32_t)packet.cmd | ((uint32_t)packet.param[0] << 8) | 
                       ((uint32_t)packet.param[1] << 16) | ((uint32_t)packet.param[2] << 24));
        }
        
        memset((void *)bt_rx_buffer, 0, BT_RX_BUFFER_SIZE);
        bt_rx_len = 0;
    }
}

bool BT_ParseCommand(uint8_t *data, uint8_t len, BT_CmdPacket_t *packet)
{
    if(len < 2) return false;
    
    packet->cmd = BT_CMD_NONE;
    packet->param_len = 0;
    memset(packet->param, 0, 4);
    
    switch(data[0])
    {
        case 'O':
            if(data[1] == 'P' || data[1] == 'p')
                packet->cmd = BT_CMD_OPEN;
            break;
            
        case 'C':
            if(data[1] == 'L' || data[1] == 'l')
                packet->cmd = BT_CMD_CLOSE;
            else if(data[1] == 'S' || data[1] == 's')
                packet->cmd = BT_CMD_SET_TIME;
            break;
            
        case 'S':
            if(data[1] == 'T' || data[1] == 't')
                packet->cmd = BT_CMD_STOP;
            break;
            
        case 'G':
            if(data[1] == 'S' || data[1] == 's')
                packet->cmd = BT_CMD_GET_STATUS;
            break;
            
        case 'M':
            if(data[1] == 'O' || data[1] == 'o')
                packet->cmd = BT_CMD_SWITCH_MODE;
            break;
            
        case 'T':
            if(len >= 5)
            {
                if(data[1] == 'O' || data[1] == 'o')
                {
                    packet->cmd = BT_CMD_SET_OPEN_TIME;
                    packet->param[0] = BT_HexToDec(data[2], data[3]);
                    packet->param_len = 1;
                }
                else if(data[1] == 'C' || data[1] == 'c')
                {
                    packet->cmd = BT_CMD_SET_CLOSE_TIME;
                    packet->param[0] = BT_HexToDec(data[2], data[3]);
                    packet->param_len = 1;
                }
                else if(data[1] == 'S' || data[1] == 's')
                {
                    packet->cmd = BT_CMD_SET_TIME;
                    if(len >= 11)
                    {
                        packet->param[0] = BT_HexToDec(data[2], data[3]);
                        packet->param[1] = BT_HexToDec(data[4], data[5]);
                        packet->param[2] = BT_HexToDec(data[6], data[7]);
                        packet->param_len = 3;
                    }
                }
            }
            break;
            
        default:
            break;
    }
    
    return (packet->cmd != BT_CMD_NONE);
}

void BT_ExecuteCommand(BT_CmdPacket_t *packet)
{
    if(packet == NULL) return;
    
    SoftTime_t current_time;
    char response[32];
    
    switch(packet->cmd)
    {
        case BT_CMD_OPEN:
            BT_SendResponse("OK:OPEN");
            break;
            
        case BT_CMD_CLOSE:
            BT_SendResponse("OK:CLOSE");
            break;
            
        case BT_CMD_STOP:
            BT_SendResponse("OK:STOP");
            break;
            
        case BT_CMD_GET_STATUS:
            SoftTime_Get(&current_time);
            snprintf(response, sizeof(response), "ST:%02d%02d%02d-%d-%d-%d",
                     current_time.hour, current_time.min, current_time.sec,
                     Sys_Context.curtainState, Sys_Context.mode, Sys_Context.luxState);
            BT_SendResponse(response);
            break;
            
        case BT_CMD_SWITCH_MODE:
            BT_SendResponse("OK:MODE");
            break;
            
        case BT_CMD_SET_OPEN_TIME:
            snprintf(response, sizeof(response), "OK:OT%02d", packet->param[0]);
            BT_SendResponse(response);
            break;
            
        case BT_CMD_SET_CLOSE_TIME:
            snprintf(response, sizeof(response), "OK:CT%02d", packet->param[0]);
            BT_SendResponse(response);
            break;
            
        case BT_CMD_SET_TIME:
            snprintf(response, sizeof(response), "OK:TM%02d%02d%02d",
                     packet->param[0], packet->param[1], packet->param[2]);
            BT_SendResponse(response);
            break;
            
        default:
            BT_SendResponse("ERR:UNKNOWN");
            break;
    }
}

void BT_SendResponse(const char *response)
{
    if(response == NULL) return;
    HAL_UART_Transmit(&huart2, (uint8_t *)response, strlen(response), 100);
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, 100);
}

bool BT_IsValidCommand(uint8_t *data, uint8_t len)
{
    if(len < 2) return false;
    
    for(uint8_t i = 0; i < len; i++)
    {
        if(data[i] >= 'a' && data[i] <= 'z')
            data[i] = data[i] - 'a' + 'A';
    }
    
    return true;
}

uint8_t BT_HexToDec(uint8_t high, uint8_t low)
{
    uint8_t result = 0;
    
    if(high >= '0' && high <= '9')
        result = (high - '0') << 4;
    else if(high >= 'A' && high <= 'F')
        result = (high - 'A' + 10) << 4;
    
    if(low >= '0' && low <= '9')
        result |= (low - '0');
    else if(low >= 'A' && low <= 'F')
        result |= (low - 'A' + 10);
    
    return result;
}
