#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main () {
	uint32_t temp = -0xae38;
	printf("%x", temp >> 2);
}