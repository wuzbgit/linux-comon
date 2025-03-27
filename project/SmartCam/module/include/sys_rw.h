#ifndef __SYS_RW_H__
#define __SYS_RW_H__

int read_sysfs_int(const char *path);
int write_sysfs_int(const char *path, int value);
int read_sysfs_string(const char *path, char *buffer, unsigned buf_size);
int write_sysfs_string(const char *path, const char *value);

#endif
