#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    FILE *fp;
    char buf[100] = {0};
    size_t ret = 0;
    fp = fopen("/mnt/sdcard/test.c","rw");
    
    while(1) {
        ret = fread(buf, 1, 100, fp);
        if (ret > 0) {
            printf("%s\n",buf);
        }
        else if (ret == 0) {
            fseek(fp,0,SEEK_SET);
        }
        else {
            perror("read");
        }
    }

    return 0;
}