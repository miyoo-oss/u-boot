#include <common.h>
#include <command.h>
#include <malloc.h>
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include <mapmem.h>
#include <image.h>
#include <ubi_uboot.h>

// Global Function
#define USBUPGRDE_SCRIPT_BUF_SIZE 16*1024

// Define (CUSTOMIZATION)
#define ENV_SD_UPGRADEIMAGE               "SdUpgradeImage"

#define SCRIPT_FILE_COMMENT         '#'                 // symbol for comment
#define SCRIPT_FILE_END             '%'                 // symbol for file end

// Macro
#define MAX_LINE_SIZE       8000
#define IS_COMMENT(x)       (SCRIPT_FILE_COMMENT == (x))
#define IS_FILE_END(x)      (SCRIPT_FILE_END == (x))
#define IS_LINE_END(x)      ('\r' == (x)|| '\n' == (x))

#if defined(CONFIG_MS_SDMMC) || defined(CONFIG_MS_USB) || defined(CONFIG_CMD_NET) || defined(CONFIG_MS_EMMC)
static char *get_script_next_line(char **line_buf_ptr)
{
    char *line_buf;
    char *next_line;
    int i = 0;

    line_buf = (*line_buf_ptr);

    // strip '\r', '\n' and comment
    while (1)
    {
        // strip '\r' & '\n'
        if (IS_LINE_END(line_buf[0]))
        {
            line_buf++;
        }
        // strip comment
        else if (IS_COMMENT(line_buf[0]))
        {
            for (i = 0; !IS_LINE_END(line_buf[0]) && i <= MAX_LINE_SIZE; i++)
            {
                line_buf++;
            }

            if (i > MAX_LINE_SIZE)
            {
                line_buf[0] = SCRIPT_FILE_END;

                printf ("Error: the max size of one line is %d!!!\n", MAX_LINE_SIZE); // <-@@@

                break;
            }
        }
        else
        {
            break;
        }
    }

    // get next line
    if (IS_FILE_END(line_buf[0]))
    {
        next_line = NULL;
    }
    else
    {
        next_line = line_buf;

        for (i = 0; !IS_LINE_END(line_buf[0]) && i <= MAX_LINE_SIZE; i++)
        {
            line_buf++;
        }

        if (i > MAX_LINE_SIZE)
        {
            next_line = NULL;

            printf ("Error: the max size of one line is %d!!!\n", MAX_LINE_SIZE); // <-@@@
        }
        else
        {
            line_buf[0] = '\0';
            *line_buf_ptr = line_buf + 1;
        }
    }

    return next_line;
}
#endif


#if defined(CONFIG_MS_SDMMC)

int do_sdstar     (struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
    char* buffer=NULL;
    char* UpgradeImage=NULL;
    char *script_buf;
    char *next_line;
    char tmp[64];
    int ret = -1;
#if defined(CONFIG_SSTAR_UPGRADE_UI)
    int total_cmd = 0;
    int cur_cmd = 0;
    char* buffer_caculate=NULL;
#endif
    buffer=(char *)malloc(USBUPGRDE_SCRIPT_BUF_SIZE);
    if((buffer==NULL))
    {
        printf("no memory for command string!!\n");
        return -1;
    }

    memset(buffer, 0 , USBUPGRDE_SCRIPT_BUF_SIZE);
    UpgradeImage = env_get(ENV_SD_UPGRADEIMAGE);
    if(UpgradeImage == NULL)
    {
        printf("UpgradeImage env is null,use default SigmastarUpgradeSD.bin\n");
        UpgradeImage = "SigmastarUpgradeSD.bin";
        run_command("setenv SdUpgradeImage SigmastarUpgradeSD.bin", 0);
        run_command("saveenv",0);

    }
    memset(tmp,0,sizeof(tmp));
    snprintf(tmp, sizeof(tmp) - 1,"fatload mmc 0 %X %s 0x%x 0x0", (U32)buffer, UpgradeImage, USBUPGRDE_SCRIPT_BUF_SIZE);
    run_command(tmp, 0);    
#if defined(CONFIG_SSTAR_UPGRADE_UI)
    buffer_caculate = (char *)malloc(USBUPGRDE_SCRIPT_BUF_SIZE);
    if((buffer_caculate == NULL))
    {
        printf("no memory for command string!!\n");
        return -1;
    }
    memcpy(buffer_caculate, buffer, USBUPGRDE_SCRIPT_BUF_SIZE);
    script_buf = buffer_caculate;
    while (get_script_next_line(&script_buf) != NULL)
    {
        total_cmd++;
    }
    free(buffer_caculate);
    memset(tmp,0,sizeof(tmp));
    snprintf(tmp, sizeof(tmp) - 1,"dcache on");
    run_command(tmp, 0);
    memset(tmp,0,sizeof(tmp));
    snprintf(tmp, sizeof(tmp) - 1,"bootlogo 1 0 0 0 0");
    run_command(tmp, 0);
    memset(tmp,0,sizeof(tmp)); 
    snprintf(tmp, sizeof(tmp) - 1,"bootframebuffer bar 0");
    run_command(tmp, 0);
    memset(tmp,0,sizeof(tmp));
    snprintf(tmp, sizeof(tmp) - 1,"dcache off");
    run_command(tmp, 0);
#endif
    script_buf = buffer;
    while ((next_line = get_script_next_line(&script_buf)) != NULL)
    {
        run_command(next_line, 0);
#if defined(CONFIG_SSTAR_UPGRADE_UI)
        cur_cmd++;
        memset(tmp,0,sizeof(tmp));
        snprintf(tmp, sizeof(tmp) - 1,"dcache on");
        run_command(tmp, 0);
        memset(tmp,0,sizeof(tmp));
        snprintf(tmp, sizeof(tmp) - 1,"bootframebuffer bar %d", cur_cmd * 100 / total_cmd);
        run_command(tmp, 0);
        memset(tmp,0,sizeof(tmp));
        snprintf(tmp, sizeof(tmp) - 1,"dcache off");
        run_command(tmp, 0);
#endif
    }
#if defined(CONFIG_SSTAR_UPGRADE_UI)
    memset(tmp,0,sizeof(tmp));    
    snprintf(tmp, sizeof(tmp) - 1,"dcache on");
    run_command(tmp, 0);
    memset(tmp,0,sizeof(tmp));    
    snprintf(tmp, sizeof(tmp) - 1, "bootframebuffer bar 100");
    run_command(tmp, 0);
    memset(tmp,0,sizeof(tmp));    
    snprintf(tmp, sizeof(tmp) - 1,"dcache off");
    run_command(tmp, 0);
#endif

    free(buffer);
    return ret;

}

U_BOOT_CMD(
    sdstar,  CONFIG_SYS_MAXARGS,    1,    do_sdstar,
    "script via sd package",
    ""
);


#endif


#if defined(CONFIG_MS_PARTITION)
#include "drivers/mstar/partition/part_mxp.h"
#define MAX_RECORD_PRINT_COUNT 32
extern int mxp_save_table_from_mem(u32 mem_address);
static void print_mxp_record(int index,mxp_record* rc)
{
    printf("[mxp_record]: %d\n",index);
    printf("     name: %s\n",rc->name);
    printf("     type: 0x%02X\n",rc->type);
    printf("   format: 0x%02X\n",rc->format);
    printf("   backup: %s\n",( ((char)0xFF)==((char)rc->backup[0]) )?(""):((char *)rc->backup));
    printf("    start: 0x%08X\n",(unsigned int)rc->start);
    printf("     size: 0x%08X\n",(unsigned int)rc->size);
    printf("   status: 0x%02X\n",rc->status);
    printf("\n");

}
int do_mxp(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
#if defined(CONFIG_MS_ISP_FLASH)
extern int mxp_init_nor_flash(void);
    int ret=0;
    if((ret=mxp_init_nor_flash())<0)
    {
        return ret;
    }
#endif

    if(strncmp(argv[1], "t.list", 6) == 0)
    {
        if(argc>2)
        {
            int i=0;
            int mem_addr = simple_strtoul(argv[2], NULL, 16);
            mxp_record* recs=(mxp_record*)((void *)mem_addr);
            for(i=0;i<MAX_RECORD_PRINT_COUNT;i++)
            {

                if(MXP_PART_TYPE_TAG==recs[i].type)
                {
                    break;

                }
                print_mxp_record(i,&recs[i]);

            }
            printf("Available MXP record count:%d\n",i);
        }
        else
        {
            int count=mxp_get_total_record_count();
            int i=0;
            printf("Total MXP record count:%d\n",count);
            for(i=0;i<count;i++)
            {
                mxp_record rec;
                mxp_get_record_by_index(i,&rec);
                print_mxp_record(i,&rec);
            }
        }
    }
    else if(strncmp(argv[1], "t.update", 8) == 0)
    {
        u32 mem_addr = (u32)simple_strtoul(argv[2], NULL, 16);
        mxp_save_table_from_mem(mem_addr);
        //
    }
    else if(strncmp(argv[1], "t.load", 6) == 0)
    {
        mxp_load_table();
    }
    else if(strncmp(argv[1], "t.init", 6) == 0)
    {
        mxp_init_table();
    }
    else if(strncmp(argv[1], "r.del", 5) == 0)
    {
        int idx=mxp_get_record_index(argv[2]);
        if(idx>=0)
        {
            mxp_delete_record_by_index(idx);
        }
        else
        {
            printf("can not found mxp record: %s\n",argv[2]);
        }
    }
    else if(strncmp(argv[1], "r.set", 5) == 0)
    {

        if(argc<3)
        {
            printf("missing parameters\n");
            return CMD_RET_USAGE;
        }
        else if(3==argc)
        {
            mxp_record rec;
            u32 mem_addr = (u32)simple_strtoul(argv[2], NULL, 16);
            memcpy(&rec,(void *)mem_addr,sizeof(mxp_record));
            mxp_set_record(&rec);
        }
        else
        {
            int idx=mxp_get_record_index(argv[2]);
            if(idx>=0)
            {

                mxp_record rec;
                mxp_get_record_by_index(idx,&rec);

                if(strncmp(argv[3], "crc32", 5) == 0)
                {
                    rec.crc32=(u32)simple_strtoul(argv[4], NULL, 16);
                    mxp_set_record(&rec);
                }
                else if(strncmp(argv[3], "status", 5) == 0)
                {
                    rec.status=(u32)simple_strtoul(argv[4], NULL, 16);
                    mxp_set_record(&rec);
                }
                else if(strncmp(argv[3], "backup", 6) == 0)
                {
                    memcpy( rec.name,argv[4],(strlen(argv[4])>15)?15:strlen(argv[4]) );
                    rec.name[15]=0;
                    mxp_set_record(&rec);
                }
                else
                {
                    printf("unsupported mxp record setting property: %s\n",argv[3]);
                    ret = -1;
                }

            }
            else
            {
                printf("can not found mxp record: %s\n",argv[2]);
                ret = -1;
            }
        }
    }
    else if(strncmp(argv[1], "r.info", 6) == 0)
    {

        if(argc<3)
        {
            printf("missing parameters\n");
            return CMD_RET_USAGE;
        }
        else
        {
            int idx=mxp_get_record_index(argv[2]);
            if(idx>=0)
            {

                mxp_record rec;
                env_set_hex("sf_part_start", 0);
                env_set_hex("sf_part_size", 0);
                env_set_hex("cpu_part_start", 0);

                if(0==mxp_get_record_by_index(idx,&rec))
                {
                    print_mxp_record(0,&rec);
                    env_set_hex("sf_part_start", rec.start);
                    env_set_hex("sf_part_size", rec.size);
                    env_set_hex("cpu_part_start", rec.start+MS_SPI_ADDR);

                    if(strncmp(argv[2], "KERNEL", 6)==0)
                    {
                        env_set_hex("sf_kernel_start", rec.start);
                        env_set_hex("sf_kernel_size", rec.size);
                    }
                }
                else
                {
                    printf("failed to get MXP record with name: %s\n",argv[2]);
                    ret = -1;
                }


            }
            else
            {
                printf("can not found mxp record: %s\n",argv[2]);
                ret = -1;
            }
        }
    }
    else
    {
            return CMD_RET_USAGE;
    }

    return ret;
}
U_BOOT_CMD(
    mxp,  CONFIG_SYS_MAXARGS,    1,    do_mxp,
    "MXP function for Mstar MXP partition",
    "mxp t.list [memory] - list table records, if [memory] then list from [memory]\n"
    "mxp t.load - load table from storage\n"
    "mxp t.init - clean the table in storage with default empty records\n"
    "mxp t.update memory - update the table in storage from memory\n"
    "mxp r.del name - remove MXP record with name\n"
    "mxp r.set index crc32,status,backup value - set the MXP record property using index with value\n"
    "mxp r.set memory - set a MXP record using name from memory, if name is not exsited in table yet, new record will be created\n"
    "mxp r.info name - show the info of the record using name, the $sf_part_start and the $sf_part_size will be set if success\n"
//    "emmc remove [name] - remove mmc partition [name]\n"
//    "emmc rmgpt - clean all mmc partition table\n"
//    "emmc part - list partitions \n"
//    "emmc slc size relwr - set slc in the front of user area,  0xffffffff means max slc size\n"
//    "emmc unlzo Src_Address Src_Length Partition_Name [empty_skip:0-disable,1-enable]- decompress lzo file and write to mmc partition \n"
//    "emmc read.p addr partition_name size\n"
//    "emmc read.p.continue addr partition_name offset size\n"
//    "emmc write.p addr partition_name size [empty_skip:0-disable,1-enable]\n"
//    "emmc write.p.continue addr partition_name offset size [empty_skip:0-disable,1-enable]\n"
//    "emmc erase.p partition_name\n"
);

#endif


