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
}
