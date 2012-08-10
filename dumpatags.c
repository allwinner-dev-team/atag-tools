#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

typedef unsigned int u32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;

#include "atags.h"
#include "omap_tags.h"


unsigned char atag_buf[10240];

dump_omap(struct omap_tag * omap_tag, int cnt) 
{
    unsigned k;
    unsigned i = 0;
    while (omap_tag->hdr.size) {
        unsigned int tag = omap_tag->hdr.tag;
        unsigned int size = omap_tag->hdr.size;


        printf("       %04d - ", i);
        if (omap_tag->hdr.tag == 0) {
            printf("0000      OMAP TABLE END");
            return 0;
        }
        else
            printf("%04x:%04x ", size, omap_tag->hdr.tag);
        switch (omap_tag->hdr.tag) {
            case OMAP_TAG_VERSION_STR:
                         printf("VERSION:      %-12s = %-12s", omap_tag->u.version.component, omap_tag->u.version.version);
                         break;
            case OMAP_TAG_BOOT_REASON:
                         printf("BOOTREASON:   %-12s", omap_tag->u.boot_reason.reason_str);
                         break;
            case OMAP_TAG_PARTITION:
                         printf("PARTITION:    %-16s :", omap_tag->u.partition.name);
                         printf(" size=%08x", omap_tag->u.partition.size);
                         printf(" offset=%08x", omap_tag->u.partition.offset);
                         printf(" mask=%08x", omap_tag->u.partition.mask_flags);
                         break;
            case OMAP_TAG_UART:
                         printf("UART: enabled uarts (bitmask) %08x", omap_tag->u.uart.enabled_uarts);
                         break;
            case OMAP_TAG_GPIO_SWITCH:
                         printf("GPIO SWITCH:  %-12s :", omap_tag->u.gpio_switch.name);
                         printf(" gpio=%04x", omap_tag->u.gpio_switch.gpio);
                         printf(" flags=%02x", omap_tag->u.gpio_switch.flags);
                         printf(" type=%02x", omap_tag->u.gpio_switch.type);
                         printf(" keycode=%08x", omap_tag->u.gpio_switch.key_code);
                         break;
            case OMAP_TAG_LCD:
                         printf("LCD:          panel=%s controller=%s", omap_tag->u.lcd.panel_name, omap_tag->u.lcd.ctrl_name);
                         printf(" nreset_gpio=%04x", omap_tag->u.lcd.nreset_gpio);
                         printf(" data_lines=%04x", omap_tag->u.lcd.data_lines);
                         break;
            case OMAP_TAG_WLAN_CX3110X:
                         printf("WLAN CX3110X:");
                         printf(" chip_type=%02x", omap_tag->u.wlan_cx3110x.chip_type);
                         printf(" power_gpio=%04x", omap_tag->u.wlan_cx3110x.power_gpio);
                         printf(" irq_gpio=%04x", omap_tag->u.wlan_cx3110x.irq_gpio);
                         printf(" spi_cs_gpio=%04x", omap_tag->u.wlan_cx3110x.spi_cs_gpio);
                         break;
            case OMAP_TAG_CLOCK:
            case OMAP_TAG_SERIAL_CONSOLE:
            case OMAP_TAG_USB:
            case OMAP_TAG_FBMEM:
            case OMAP_TAG_STI_CONSOLE:
            case OMAP_TAG_CAMERA_SENSOR:
            case OMAP_TAG_TEA5761:
            case OMAP_TAG_TMP105:
            case OMAP_TAG_FLASH_PART_STR:
            case OMAP_TAG_NOKIA_BT:
            case OMAP_TAG_CBUS:
            case OMAP_TAG_EM_ASIC_BB5:
            default:     for (k = 0; k < omap_tag->hdr.size; k++)
                             printf(" %02x", *((unsigned char *) (&omap_tag->u) + k));
                         break;
        }
        printf("\n");           
        i += omap_tag->hdr.size + sizeof(struct omap_tag_header);
        omap_tag = omap_tag_next(omap_tag);
 
    }
    return 1;
}

main(int argc, char** argv)
{
    struct tag * atag;
    char *progname = *argv;
    int f = open(*++argv, O_RDONLY);
    if (f == -1) {
        printf("%s binary_atag_data_file\n", progname);
        exit(1);
    }

    int cnt = read(f, atag_buf, 10000);
    int i;
     
    printf("read %d bytes from %s in buffer of size 10000\n", cnt, *argv);
    i = 0;
    atag = (struct tag*) atag_buf;
    while (atag->hdr.tag != ATAG_NONE) {

        printf("%04d - %04d:%08x", i, atag->hdr.size, atag->hdr.tag);
            
        switch (atag->hdr.tag) {
            case ATAG_NONE: printf(" END ATAG\n");
                            break;
            case ATAG_BOARD:printf(" OMAP table (%d bytes)\n", atag->hdr.size << 2);
                            dump_omap((struct omap_tag *) &(atag->u), (atag->hdr.size - (sizeof(atag->hdr)>>2)) << 2);
                            break;
            case ATAG_CORE: printf(" ATAG CORE");
                            printf(" flags=%08x", atag->u.core.flags);
                            printf(" pagesize=%08x", atag->u.core.pagesize);
                            printf(" rootdev=%08x", atag->u.core.rootdev);
                            printf("\n");
                            break;
            case ATAG_MEM:  printf(" ATAG MEM");
                            printf(" size=%08x", atag->u.mem.size);
                            printf(" start=%08x", atag->u.mem.start);
                            printf("\n");
                            break;
            case ATAG_VIDEOTEXT: printf(" ATAG VIDEOTEXT");
                            printf(" x=%02x", atag->u.videotext.x);
                            printf(" y=%02x", atag->u.videotext.y);
                            printf(" video_page=%04x", atag->u.videotext.video_page);
                            printf(" video_mode=%02x", atag->u.videotext.video_mode);
                            printf(" video_cols=%02x", atag->u.videotext.video_cols);
                            printf(" video_ega_bx=%04x", atag->u.videotext.video_ega_bx);
                            printf(" video_lines=%02x", atag->u.videotext.video_lines);
                            printf(" video_isvga=%02x", atag->u.videotext.video_isvga);
                            printf(" video_points=%04x", atag->u.videotext.video_points);
                            printf("\n");
                            break;
            case ATAG_RAMDISK: printf(" ATAG RAMDISK");
                            printf(" flags=%08x", atag->u.ramdisk.flags);
                            printf(" size=%08x", atag->u.ramdisk.size);
                            printf(" start=%08x", atag->u.ramdisk.start);
                            printf("\n");
                            break;
            case ATAG_INITRD2: printf(" ATAG INITRD2");
                            printf(" start=%08x", atag->u.initrd.start);
                            printf(" size=%08x", atag->u.initrd.size);
                            printf("\n");
                            break;
            case ATAG_SERIAL: printf(" ATAG SERIAL");
                            printf(" low=%08x", atag->u.serialnr.low);
                            printf(" high=%08x", atag->u.serialnr.high);
                            printf("\n");
                            break;
            case ATAG_REVISION: printf(" ATAG REVISION");
                            printf(" revision=%08x", atag->u.revision.rev);
                            printf("\n");
                            break;
            case ATAG_VIDEOLFB: printf(" ATAG VIDEOLFB");
                            printf(" lfb_width=%04x", atag->u.videolfb.lfb_width);
                            printf(" lfb_height=%04x", atag->u.videolfb.lfb_height);
                            printf(" lfb_depth=%04x", atag->u.videolfb.lfb_depth);
                            printf(" lfb_linelength=%04x", atag->u.videolfb.lfb_linelength);
                            printf(" lfb_base=%08x", atag->u.videolfb.lfb_base);
                            printf(" lfb_size=%08x", atag->u.videolfb.lfb_size);
                            printf(" red_size=%02x", atag->u.videolfb.red_size);
                            printf(" res_pos=%02x", atag->u.videolfb.red_pos);
                            printf(" green_size=%02x", atag->u.videolfb.green_size);
                            printf(" green_pos=%02x", atag->u.videolfb.green_pos);
                            printf(" blue_size=%02x", atag->u.videolfb.blue_size);
                            printf(" blue_pos=%02x", atag->u.videolfb.blue_pos);
                            printf(" rsvd_size=%02x", atag->u.videolfb.rsvd_size);
                            printf(" rsvd_pos=%02x", atag->u.videolfb.rsvd_pos);
                            printf("\n");
                            break;
            case ATAG_CMDLINE:
                            printf(" ATAG CMDLINE");
                            printf(" %s", atag->u.cmdline.cmdline);
                            printf("\n");
                            break;
            default: printf("unknown tag\n");
        }
        i += atag->hdr.size<<2;
        atag = tag_next(atag);
    }
    exit(0);
}

