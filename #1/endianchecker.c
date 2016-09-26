#include <stdio.h>

int main(int argc, char** argv) {
  char endianteller = 1;
  char* c_pointer   = &endianteller;
  
  if (c_pointer = 1) {
    printf("Hello Little Endian World!");
  } else {
    printf("Hello Big Endian World!");
  }
}
