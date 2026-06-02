/**
 * ScopeFun EP6 裸诊断工具 v2
 *
 * 编译:
 *   gcc -o test_ep6.exe test_ep6.c -Ilib/libusb-1.0.25/libusb
 *       lib/libusb-1.0.25/Build/Release/libusb/core.o
 *       lib/libusb-1.0.25/Build/Release/libusb/descriptor.o
 *       lib/libusb-1.0.25/Build/Release/libusb/hotplug.o
 *       lib/libusb-1.0.25/Build/Release/libusb/io.o
 *       lib/libusb-1.0.25/Build/Release/libusb/strerror.o
 *       lib/libusb-1.0.25/Build/Release/libusb/sync.o
 *       lib/libusb-1.0.25/Build/Release/libusb/os/events_windows.o
 *       lib/libusb-1.0.25/Build/Release/libusb/os/threads_windows.o
 *       lib/libusb-1.0.25/Build/Release/libusb/os/windows_common.o
 *       lib/libusb-1.0.25/Build/Release/libusb/os/windows_winusb.o
 *       lib/libusb-1.0.25/Build/Release/libusb/os/windows_usbdk.o
 *       -lsetupapi -lole32
 *
 * 直接操作 libusb, 不依赖 ScopeFun GUI 框架.
 * 可检测 ScopeFun 模式和 FX3 bootloader 模式.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <libusb.h>

#define VID_SCOPEFUN    0x1D50
#define PID_SCOPEFUN    0x6104
#define VID_CYPRESS     0x04B4
#define PID_CYPRESS_FX3 0x00F3
#define TIMEOUT_MS      2000

#define EP2_OUT  0x02
#define EP6_IN   0x86

int main(void)
{
    libusb_device_handle *dev = NULL;
    int ret;
    int is_scope = 1;

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    printf("=== ScopeFun EP6 诊断工具 ===\n\n");

    ret = libusb_init(NULL);
    if (ret < 0) { printf("libusb_init 失败\n"); return 1; }

    /* 检测设备: 先 ScopeFun 模式, 再 bootloader 模式 */
    dev = libusb_open_device_with_vid_pid(NULL, VID_SCOPEFUN, PID_SCOPEFUN);
    if (!dev) {
        dev = libusb_open_device_with_vid_pid(NULL, VID_CYPRESS, PID_CYPRESS_FX3);
        if (dev) is_scope = 0;
    }

    if (!dev) {
        printf("[FAIL] 未检测到任何设备\n");
        printf("       ScopeFun: VID=0x%04X PID=0x%04X\n", VID_SCOPEFUN, PID_SCOPEFUN);
        printf("       Bootloader: VID=0x%04X PID=0x%04X\n", VID_CYPRESS, PID_CYPRESS_FX3);
        printf("\n请确认 USB 已连接.\n");
        printf("如需从 bootloader 恢复, 请打开 scopefun.exe 并使用\n");
        printf("\"Write USB to RAM\" 重新加载 FX3 固件.\n");
        libusb_exit(NULL);
        return 1;
    }

    printf("[OK] 找到设备!\n");
    if (is_scope)
        printf("     模式: ScopeFun (VID=0x%04X PID=0x%04X)\n", VID_SCOPEFUN, PID_SCOPEFUN);
    else
        printf("     模式: FX3 Bootloader (VID=0x%04X PID=0x%04X)\n"
               "     -> 请先通过 scopefun.exe 加载 FX3 固件\n",
               VID_CYPRESS, PID_CYPRESS_FX3);

    ret = libusb_claim_interface(dev, 0);
    if (ret < 0) { printf("claim_interface 失败: %s\n", libusb_error_name(ret)); goto done; }
    printf("[OK] 接口已声明\n\n");

    if (!is_scope) {
        printf("Bootloader 模式下不能进行 EP6 测试.\n");
        printf("请在 scopefun.exe 中加载 FX3 固件后重试.\n");
        goto done;
    }

    /* 测试1: 固件ID */
    {
        uint8_t buf[32] = {0};
        ret = libusb_control_transfer(dev, 0xC0, 0xB0, 0, 0, buf, 16, TIMEOUT_MS);
        if (ret == 16)
            printf("[OK] 固件 ID: \"%s\"\n", (char*)buf);
        else
            printf("[WARN] 固件 ID: ret=%d\n", ret);
    }

    /* 测试2: ADC时钟使能 */
    {
        ret = libusb_control_transfer(dev, 0x40, 0xE0, 0, 0, NULL, 0, TIMEOUT_MS);
        printf("[%s] ADC 时钟使能: ret=%d\n", ret==0?"OK":"FAIL", ret);
    }

    /* 测试3: FPGA状态 */
    {
        uint8_t buf[2] = {0};
        ret = libusb_control_transfer(dev, 0xC0, 0xB1, 0, 0, buf, 2, TIMEOUT_MS);
        printf("[%s] FPGA 状态: %s\n",
               ret>=1?(buf[0]?"OK":"INFO"):"FAIL",
               ret>=1?(buf[0]?"已配置":"未配置"):"读取失败");
    }

    /* 测试4: 写配置到EP2 */
    {
        uint8_t cfg[128] = {0};
        cfg[4*4+0] = 0x03;      /* trigger_mode=3 立即触发 */
        cfg[9*4+0] = 99;         /* framesize = 100-1 */
        int xfer = 0;
        ret = libusb_bulk_transfer(dev, EP2_OUT, cfg, sizeof(cfg), &xfer, TIMEOUT_MS);
        printf("[%s] EP2 配置: %d 字节 (ret=%d)\n", ret==0?"OK":"FAIL", xfer, ret);
    }

    /* 测试5: 读EP6 */
    {
        uint8_t ep6[1024] = {0};
        int xfer = 0;
        printf("\n[WAIT] EP6 读取 (%d ms 超时)...\n", TIMEOUT_MS);
        ret = libusb_bulk_transfer(dev, EP6_IN, ep6, sizeof(ep6), &xfer, TIMEOUT_MS);

        if (ret == 0 && xfer > 0) {
            printf("[OK] EP6 收到 %d 字节!\n", xfer);
            printf("      前16字节: ");
            for (int i = 0; i < 16; i++) printf("%02X ", ep6[i]);
            printf("\n");
            if (xfer>=4 && ep6[0]==0xDD && ep6[3]==0xDD)
                printf("[PASS] 帧头魔数正确! 链路 OK!\n");
            else
                printf("[WARN] 帧头魔数不匹配\n");
        } else if (ret == LIBUSB_ERROR_TIMEOUT) {
            printf("[TIMEOUT] EP6 超时 — 无帧数据\n");
        } else {
            printf("[FAIL] EP6: ret=%d (%s)\n", ret, libusb_error_name(ret));
        }
    }

    /* 测试6: GPIF错误计数器 */
    {
        uint8_t buf[12] = {0};
        ret = libusb_control_transfer(dev, 0xC0, 0xEB, 0, 0, buf, 12, TIMEOUT_MS);
        if (ret >= 12) {
            uint16_t wo0=buf[0]|(buf[1]<<8), wo2=buf[2]|(buf[3]<<8), wo3=buf[4]|(buf[5]<<8);
            uint16_t ru0=buf[6]|(buf[7]<<8), ru2=buf[8]|(buf[9]<<8), ru3=buf[10]|(buf[11]<<8);
            printf("\n[INFO] GPIF 错误: Wo:%u/%u/%u Ru:%u/%u/%u\n", wo0,wo2,wo3, ru0,ru2,ru3);
            if (wo0==0 && ru0==0)
                printf("  Socket0 无错误 — FPGA 从未向 EP6 写数据\n");
        }
    }

done:
    if (dev) { libusb_release_interface(dev, 0); libusb_close(dev); }
    libusb_exit(NULL);
    printf("\n=== 诊断完成 ===\n");
    return 0;
}
