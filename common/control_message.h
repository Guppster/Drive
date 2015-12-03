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
  uint8_t token[16];
  uint8_t filename[65511];
}ctrl_message;

#endif
