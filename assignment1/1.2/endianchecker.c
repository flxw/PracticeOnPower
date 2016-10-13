#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char** argv) {
  // convert to network byte order - and that is always big Endian
  if (htonl(123) == 123) {
    printf("Hello Big Endian World!\n");
  } else {
    printf("Hello Little Endian World!\n");
  }

  /* compile time, excluded for automated submission check.
  #if __BIG_ENDIAN__ == 1
  printf("compile time says big endian\n");
  #elif __BIG_ENDIAN__ != 1
  printf("compile time says little endian\n");
  #endif
  */
}
