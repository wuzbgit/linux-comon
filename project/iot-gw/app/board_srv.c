#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mount.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/vfs.h>


static bool check_device_exists(const char *file_path)
{
    if (access(file_path, F_OK) == 0) {
        return true;
    }

    return false;
}

static int create_mount_point(char *dir)
{
    struct stat st = {0};
    int ret = 0;

    if (stat(dir, &st) == -1) {
        ret = mkdir(dir, 0755);
        if (ret != 0) {
            return -1;
        }
    }

    return 0;
}

#define EMMC_DEV                "/dev/mmcblk0"
#define EMMC_DATA_PARTITION     "/dev/mmcblk0p8"
#define SD_DEV                  "/dev/mmcblk1"
#define SD_DATA_PARTITION       "/dev/mmcblk1p8"
#define MOUNT_POINT_DATA        "/mnt/data"

int mount_partition(char *dev_partition,char *mnt_point)
{
    char cmd[256] = {0};

    if (check_device_exists(dev_partition)) {
        printf("Device exists, attempting to mount...\n");
        if (create_mount_point(mnt_point) == 0) {
            
            sprintf(cmd,"systemd-mount %s %s",dev_partition,mnt_point);
            if (system(cmd) == 0) {
            // if (mount(dev_partition, mnt_point,"auto", MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL) == 0) {
                printf("Device mounted successfully.\n");
                return 0;
            }
            else {
                printf("Failed to mount %s on %s\n",dev_partition,mnt_point);
                return -1;
            }
        }
        else {
            printf("create mount point %s fail\n",mnt_point);
            return -1;
        }
    } else {
        printf("Device does not exist.\n");
    }
 
    return -1;
}

static int get_root_dev(char *buf,unsigned bufsize)
{
    FILE *fp;
    char line[512] = {0};
    char *root_dev = NULL;
    int ret = -1;
    unsigned i;

    fp = fopen("/proc/cmdline", "r");
    if (fp == NULL) {
        perror("Failed to open /proc/mounts");
        return -1;
    }

    if (fread(line, 1, sizeof(line) - 1, fp) > 0) {
        root_dev = strstr(line,"root=");
        if (root_dev != NULL) {
            root_dev += 5;
            
            for (i = 0;i < bufsize - 1;i++) {
                if ((*(root_dev + i) == ' ') || (*(root_dev + i) == '\0')) {
                    break;
                }
                buf[i] = *(root_dev + i);
            }

            ret = 0;
        }
    }

    fclose(fp);

    return ret;   
}
int mount_data_partition()
{
    char root_dev[64] = {0};
    char *data_partition = NULL;

    get_root_dev(root_dev, sizeof(root_dev) - 1);

    if (strncmp(root_dev, SD_DEV, strlen(SD_DEV)) == 0) {
        printf("Root filesystem is on %s\n",SD_DEV);

        return mount_partition(SD_DATA_PARTITION,MOUNT_POINT_DATA);
    } else if (strncmp(root_dev, EMMC_DEV, strlen(EMMC_DEV)) == 0) {
        printf("Root filesystem is on %s\n",EMMC_DEV);
        return mount_partition(EMMC_DATA_PARTITION, MOUNT_POINT_DATA);
    } else {
        printf("Root filesystem is not on /dev/mmcblk0* or /dev/mmcblk1*\n");
    }

    return 0;
}

extern int di_init();
extern int do_init();
extern int rs485_init();

int board_init()
{
    di_init();

    do_init();

    rs485_init();
}

int main() 
{
    board_init();
    
    mount_data_partition();

    while(1) {
        sleep(60);
    }
    
    return 0;
}
