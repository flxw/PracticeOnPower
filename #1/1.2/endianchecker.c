#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char** argv) {
  // convert to network byte order - and that is always big Endian
  // Da hat Polze einen guten Tipp gegeben
  if (htonl(123) == 123) {
    printf("Hello Big Endian World!\n");
  } else {
    printf("Hello Little Endian World!\n");
  }

  /* klappt nur im Lehrbuch
  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN___
  printf("GNU compile time says little endian\n");
  #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN___
  printf("GNU compile time says big endian\n");
  #endif */

  #if __BIG_ENDIAN__ == 1
  printf("compile time says big endian\n");
  #elif __BIG_ENDIAN__ != 1
  printf("compile time says little endian\n");
  #endif
}
