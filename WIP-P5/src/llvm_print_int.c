#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int monga_int(int a);

int main(void) {
	int i;

	printf("\nCalling monga_int(10)" );
	printf("\nreturn:\n");
	
	i = monga_int(10);
	printf("%d",i);
	
	printf("\n\n");
	
	return 0;
}
