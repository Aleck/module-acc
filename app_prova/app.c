#include <stdio.h>
#include <sys/ioctl.h>

int main() {
	printf("APP: start\n");

	FILE *acc;

	acc = fopen("/dev/acc0", "r+");
	printf("APP: inode opened");

	ioctl(acc, 0, 5); // 0-> cmd, 5->argument




	fclose(acc);
	printf("APP: inode closed");

	printf("APP: end\n");


	return 0;
}
