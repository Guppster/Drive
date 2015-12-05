#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

union endian_tester
{
  int integer;
  uint8_t bytes[4];
};

int main()
{
  union endian_tester t = { .integer = 0xA0B1C2D3 };
  int i;

  for (i = 0; i < 4; ++i)
    printf("%x ", t.bytes[i]);

  puts("");

  if (t.bytes[0] == 0xA0)
    printf("This system is big-endian\n");
  else
    printf("This system is little-endian\n");

  exit(EXIT_SUCCESS);
}

