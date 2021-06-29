#include <string.h>
#include <stdio.h>

int main(){
	char seven_byte[7];
	seven_byte[0] = 1;
	seven_byte[1] = 0;
	seven_byte[2] = 0;
	seven_byte[3] = 0;
	seven_byte[4] = 0;
	seven_byte[5] = 0;
	seven_byte[6] = 1;
	unsigned long long ull;
	memcpy(&ull, seven_byte, 7);
	unsigned int ui = ull;
	printf("%d\n", ui);
	return 0;
}
