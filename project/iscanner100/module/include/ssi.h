#ifndef SSI_H
#define SSI_H

#ifdef __cplusplus
extern "C" {
#endif

#define SSI_CLK_CMD			0x01
#define SSI_BYTEBITS_CMD	0x02

int ssi_init(int clk);

int ssi_deinit(int fd_ssi);

int ssi_read(int fd_ssi,unsigned char *buf,int len,int timeout);

int ssi_ctrl(int fd_ssi,unsigned cmd,void *args);

#ifdef __cplusplus
}
#endif

#endif