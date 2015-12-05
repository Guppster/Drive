#include <stdint.h>

#ifndef CTRL_MESSAGE_H
#define CTRL_MESSAGE_H

typedef struct
{
  uint8_t type;                
  uint8_t numSeq;        
  uint16_t length;           
  uint32_t filesize;                    
  uint32_t checksum;          
  uint32_t token[2];
  uint32_t filename[180];
}ctrl_message;

#endif
