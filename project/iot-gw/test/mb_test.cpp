#include <iostream>
#include <modbus/modbus.h>
#include <vector>
#include <cstring>
#include <unistd.h>  
#include <fcntl.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>

#define RS485_INST1_ENABLE      "/sys/bus/platform/devices/2500400.uart/rs485_enable"
#define RS485_INST2_ENABLE      "/sys/bus/platform/devices/2500c00.uart/rs485_enable"
#define RS485_INST1_DEVPATH     "/dev/ttyAS1"
#define RS485_INST2_DEVPATH     "/dev/ttyAS3"



unsigned long get_timestamp()
{
    struct timeval tv;
    unsigned long ms = 0;
    gettimeofday(&tv, NULL);
    ms = tv.tv_sec * 1000 + tv.tv_usec/1000;
    return ms;
}

static int rs485_enable(const char *devpath,int enable)
{
    char cmd[64] = {0};
    int ret = -1;

    if (strcmp(devpath,RS485_INST1_DEVPATH) == 0) {
        sprintf(cmd,"echo %d > %s",enable,RS485_INST1_ENABLE);
    }
    else if (strcmp(devpath,RS485_INST2_DEVPATH) == 0) {
        sprintf(cmd,"echo %d > %s",enable,RS485_INST2_ENABLE);
    }

    if (strlen(cmd) != 0) {
        ret = system(cmd);
    }
    printf("ret=%d\n",ret);
    return ret;
}

void display_usage()
{
    printf(
        "rs485  : rs485 test\n"
        "Usage      : rs485 [options]\n"
        "\n"
        "Options [default in brackets]:\n"
        "    --device,           -d         - Test device\n"
        "    --baudrate,         -b         - Test device baudrate\n"
        "    --slave,            -s         - Test modbus slave id\n"
        "    --address,          -a         - Test address\n"
        "    --value,            -v         - Test value\n"
        "\n"
    );
}

static struct option long_options[] = {
    {"device"        , required_argument, NULL, 'd'},
    {"baudrate"      , required_argument, NULL, 'b'},
    {"slave"         , required_argument, NULL, 's'},
    {"address"       , required_argument, NULL, 'a'},
    {"value"         , required_argument, NULL, 'v'},
    {"help"          , required_argument, NULL, 'h'}
};

int main(int argc,char *argv[])
{
    int opt;
    int option_idx = 0;
    char devpath[100] = {0};
    unsigned baud_rate = 9600;
    char parity = 'N';                         // 校验位：N（无校验）、E（偶校验）、O（奇校验）
    int data_bits = 8;                         // 数据位
    int stop_bits = 1;                         // 停止位
    int slave_id = 1;                          // Modbus 从站 ID
    int start_address = 0;  // 写入的起始地址
    int num_registers = 5; // 要读取的寄存器数量
    uint16_t value = 20;
    std::vector<uint16_t> values_to_write = {20};  // 要写入的值

    if (argc == 1) {
        display_usage();
    }

    while((opt = getopt_long(argc,argv,"d:b:s:a:v:h?",long_options,&option_idx)) != -1) {
        switch (opt)
        {
        case 'd':
            strncpy(devpath,optarg,sizeof(devpath) - 1);
            break;
        case 'b':
            baud_rate = atoi(optarg);
            break;
        case 's':
            slave_id = atoi(optarg);
            break;
        case 'a':
            start_address = atoi(optarg);
            break;
        case 'v':
            value = atoi(optarg);
            break;
        case 'h':
        case '?':
            display_usage();
            break;
        default:
            break;
        }
    }
    
    if (strlen(devpath) == 0) {
        return -1;
    }

    printf("devpath:%s,baudrate:%d,slave_id:%d,address:0x%x,value:%d\n",devpath,baud_rate,slave_id,start_address,value);
    // std::cout << "devpath " << devpath << "baudrate" << baud_rate << "slave_id:"<< slave_id << "value:" << value << std::endl;
    rs485_enable(devpath,1);

    modbus_t *ctx = modbus_new_rtu(devpath, baud_rate, parity, data_bits, stop_bits);
    if (ctx == nullptr) {
        std::cerr << "Unable to create the libmodbus context." << std::endl;
        return -1;
    }
    
    if (modbus_set_slave(ctx, slave_id) == -1) {
        std::cerr << "Unable to set slave ID: " << modbus_strerror(errno) << std::endl;
        modbus_free(ctx);
        return -1;
    }
    modbus_set_response_timeout(ctx, 2, 0);
    modbus_set_debug(ctx, 0);
    if (modbus_connect(ctx) == -1) {
        std::cerr << "Connection failed: " << modbus_strerror(errno) << std::endl;
        modbus_free(ctx);
        return -1;
    }

    std::vector<uint16_t> registers(num_registers);
    
    int rc;
    unsigned long last_time,cur_time,diff;
    unsigned long start_time,end_time;
    start_time = get_timestamp();
    do {
	    last_time = get_timestamp();
        rc = modbus_read_registers(ctx, start_address, num_registers, registers.data());
        if (rc == -1) {
            std::cerr << "Failed to read registers: " << modbus_strerror(errno) << std::endl;
            goto out;
        }
        cur_time = get_timestamp();
	    diff = cur_time - last_time;
	    std::cout << "diff time is " << diff <<"ms"<< std::endl;
        std::cout << "Holding Registers:" << std::endl;
        for (int i = 0; i < num_registers; ++i) {
            std::cout << "Register " << start_address + i << ": " << registers[i] << std::endl;
        }
        usleep(300*1000);
    }while(1);


    // // 写入寄存器
    // // rc = modbus_write_registers(ctx, start_address, values_to_write.size(), values_to_write.data());
    // rc = modbus_write_registers(ctx, start_address, 1, &value);
    // if (rc == -1) {
    //     std::cerr << "Failed to write registers: " << modbus_strerror(errno) << std::endl;
    //     goto out;
    // }

    // num_registers = 1;
    // rc = modbus_read_registers(ctx, start_address, num_registers, registers.data());
    // if (rc == -1) {
    //     std::cerr << "Failed to read registers: " << modbus_strerror(errno) << std::endl;
    //     goto out;
    // }

    // // 打印读取的寄存器值
    // std::cout << "Holding Registers:" << std::endl;
    // for (int i = 0; i < num_registers; ++i) {
    //     std::cout << "Register " << start_address + i << ": " << registers[i] << std::endl;
    // }

    // 关闭连接并释放资源

out:
    modbus_close(ctx);
    modbus_free(ctx);
    end_time = get_timestamp();
    std::cout << "mb test last time: " << (end_time - start_time)/1000 << std::endl;
    return 0;
}
