#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

#define MAX_EVENT_SIZE  (sizeof(struct inotify_event) + NAME_MAX + 1)
#define MAX_BUF_SIZE    (1024 * (MAX_EVENT_SIZE))
#define DEVICE_WAIT_TIMEOUT 20  // 等待设备生成的超时（秒）

extern void sd_log_close();
extern void sd_log(const char *format, ...);
extern void sd_log_init();

int is_mount_point_in_use(const char *mount_point) {
    FILE *file = fopen("/proc/mounts", "r");
    if (!file) {
        sd_log("fopen /proc/mounts failed\n");
        return 0;
    }

    char line[512];
    size_t mount_point_len = strlen(mount_point);
    int mounted = 0;

    while (fgets(line, sizeof(line), file)) {
        char *match = strstr(line, mount_point);
        if (match) {
            // 确保匹配到的是完整的挂载点
            if ((match == line || *(match - 1) == ' ') &&  // 前面是行首或空格
                (match[mount_point_len] == ' ' || match[mount_point_len] == '\t' || match[mount_point_len] == '\n')) { // 后面是空格或换行
                mounted = 1;
                break;
            }
        }
    }

    fclose(file);
    return mounted;
}

int get_partition_number(const char *device_name) {
    int len = strlen(device_name);
    int partition_number = -1;
    for (int i = len - 1; i >= 0; i--) {
        if (device_name[i] == 'p') {
            partition_number = atoi(device_name + i + 1);
            break;
        }
    }
    return partition_number;
}

static char *get_mount_point(char *device_name, char *mount_point, unsigned mount_point_len)
{
    int len = strlen(device_name);
    int partition_number = -1;
    int ret = 0;
    
    partition_number = get_partition_number(device_name);

    if (partition_number <= 0) {
        return NULL;
    } else if (partition_number == 1) {
        snprintf(mount_point, mount_point_len, "/mnt/sdcard");
    } else {
        snprintf(mount_point, mount_point_len, "/mnt/sdcard_%d", partition_number);
    }

    return mount_point; 
}

void create_mount_point(const char *mount_point) {
    struct stat st;
    if (stat(mount_point, &st) != 0) {
        if (mkdir(mount_point, 0755) == -1) {
            sd_log("mkdir %s failed\n",mount_point);
        } else {
            sd_log("Created mount point: %s\n", mount_point);
        }
    }
}

#define SD_DEVICE_NAME  "mmcblk1"

static int is_sd_block_partition(const char *block_node)
{
    char uevent_path[256];
    int ret = 0;

    if (strncmp(block_node,SD_DEVICE_NAME,strlen(SD_DEVICE_NAME))) {
        return 0;
    }

    snprintf(uevent_path, sizeof(uevent_path), "/sys/class/block/%s/uevent", block_node);
    FILE *fp = fopen(uevent_path, "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "DEVTYPE=partition")) {
                ret = 1;
                break;
            }
        }
        fclose(fp);
    }

    return ret;
}

int get_filesystem_type(const char *device) {
    char cmd[256];
    FILE *fp;
    snprintf(cmd, sizeof(cmd), "blkid -o value -s TYPE %s", device);
    fp = popen(cmd, "r");
    if (!fp) {
        sd_log("Get %s filesystem type failed:popen failed\n",device);
        return -1;
    }

    char fs_type[128];
    if (fgets(fs_type, sizeof(fs_type), fp)) {
        fs_type[strcspn(fs_type, "\n")] = 0;
        if (strlen(fs_type) > 0) {
            fclose(fp);
            return 0;
        }
    }
    
    fclose(fp);
    return -1;
}

int mount_partition(const char *device, const char *mount_point) {
    if (is_mount_point_in_use(mount_point)) {
        sd_log("Mount point %s is already in use.\n", mount_point);
        return 0;
    }
    
    create_mount_point(mount_point);

    char cmd[256] = {0};
    snprintf(cmd,255,"mount %s %s",device,mount_point);
    if (system(cmd) != 0) {
        sd_log("mount %s %s failed\n",device,mount_point);
        return -1;
    }

    sd_log("Mounted %s at %s\n", device, mount_point);
    return 0;
}

int unmount_partition(const char *mount_point) {
    if (umount(mount_point) == -1) {
        if (umount2(mount_point, MNT_FORCE|MNT_DETACH) != 0) {
            sd_log("umount %s force failed\n",mount_point);
            return -1;
        }
        char cmd[64] = "echo 3 > /proc/sys/vm/drop_caches";
        system(cmd);
    }

    sd_log("Unmounted %s\n", mount_point);
    return 0;
}

void check_and_mount_existing_devices() {
    DIR *dir = opendir("/dev");
    if (dir == NULL) {
        sd_log("opendir /dev failed\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_BLK) {
            char device_path[256] = {0};
            char mount_point[256] = {0};

            snprintf(device_path, sizeof(device_path), "/dev/%s", entry->d_name);
            if (is_sd_block_partition(entry->d_name) && get_mount_point(entry->d_name, mount_point, 255)) {

                if (get_filesystem_type(device_path) == 0) {
                    if (!is_mount_point_in_use(mount_point)) {
                        mount_partition(device_path, mount_point);
                    }
                }
            }
        }
    }

    closedir(dir);
}

void monitor_sdcard() {
    int fd, length, i = 0;
    char buffer[MAX_BUF_SIZE];
    struct inotify_event *event;

    fd = inotify_init();
    if (fd == -1) {
        sd_log("inotify_init failed\n");
        exit(1);
    }

    if (inotify_add_watch(fd, "/dev", IN_CREATE | IN_DELETE) == -1) {
        sd_log("inotify_add_watch failed\n");
        exit(1);
    }

    check_and_mount_existing_devices();

    while (1) {
        length = read(fd, buffer, MAX_BUF_SIZE);
        if (length == -1) {
            sd_log("read inotify buffer failed\n");
        }

        for (i = 0; i < length; i += sizeof(struct inotify_event) + event->len) {
            event = (struct inotify_event *)&buffer[i];

            if (event->mask & IN_CREATE) {
                if (event->len > 0) {
                    char device_path[256];
                    snprintf(device_path, sizeof(device_path), "/dev/%s", event->name);

                    struct stat st;
                    if (stat(device_path, &st) == 0 && ((st.st_mode & S_IFBLK) == S_IFBLK)) {
                        char mount_point[256] = {0};
                        char device[256] = {0};

                        if (is_sd_block_partition(event->name) && get_mount_point(event->name, mount_point, 255)) {
                            snprintf(device, sizeof(device), "/dev/%s", event->name);
                            if (get_filesystem_type(device) == 0) {
                                if (is_mount_point_in_use(mount_point)) {
                                    unmount_partition(mount_point);
                                }

                                mount_partition(device, mount_point);
                            }
                        }
                    }
                }
            }

            if (event->mask & IN_DELETE) {
                if (event->len > 0) {
                    char device_path[256];
                    char mount_point[256] = {0};

                    snprintf(device_path, sizeof(device_path), "/dev/%s", event->name);
                    
                    if (get_mount_point(event->name,mount_point, 255)) {
                        if (is_mount_point_in_use(mount_point)) {
                            unmount_partition(mount_point);
                        }
                    }
                }
            }
        }
    }

    close(fd);
}

int main() {
    sd_log_init();
    sd_log("Monitoring SD card events...\n");
    monitor_sdcard();
    return 0;
}
