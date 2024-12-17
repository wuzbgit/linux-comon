#ifndef IOT_RF_H
#define IOT_RF_H

#if defined(__cplusplus)|| defined(c_plusplus)
extern "C" {
#endif

bool chardev_exist();

bool rf_exist();

int rf_reset();

void rf_dev_clean();
#if defined(__cplusplus)|| defined(c_plusplus)
}
#endif
#endif
