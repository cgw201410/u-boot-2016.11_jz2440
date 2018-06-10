#include <common.h>
#include <asm/arch-s3c24x0/s3c2440.h>
#include <asm/arch-s3c24x0/s3c24x0.h>

#define BUSY            1

#define NAND_SECTOR_SIZE    512
#define NAND_BLOCK_MASK     (NAND_SECTOR_SIZE - 1)

#define NAND_SECTOR_SIZE_LP    2048
#define NAND_BLOCK_MASK_LP     (NAND_SECTOR_SIZE_LP - 1)

/* S3C2440的NAND Flash操作函数 */


/* 等待NAND Flash就绪 */
static void s3c2440_wait_idle(void)
{
    int i;
	struct s3c24x0_nand * nand_chip = s3c24x0_get_base_nand();
    volatile unsigned char *p = (volatile unsigned char *)&nand_chip->nfstat;

    while(!(*p & BUSY))
        for(i=0; i<10; i++);
}

/* 发出片选信号 */
static void s3c2440_nand_select_chip(void)
{
    int i;
	struct s3c24x0_nand * nand_chip = s3c24x0_get_base_nand();

    nand_chip->nfcont &= ~(1<<1);
    for(i=0; i<10; i++);    
}

/* 取消片选信号 */
static void s3c2440_nand_deselect_chip(void)
{
	struct s3c24x0_nand * nand_chip = s3c24x0_get_base_nand();

    nand_chip->nfcont |= (1<<1);
}

/* 发出命令 */
static void s3c2440_write_cmd(int cmd)
{
	struct s3c24x0_nand * nand_chip = s3c24x0_get_base_nand();

    volatile unsigned char *p = (volatile unsigned char *)&nand_chip->nfcmd;
    *p = cmd;
}

/* 发出地址 */
static void s3c2440_write_addr(unsigned int addr)
{
    int i;
	struct s3c24x0_nand * nand_chip = s3c24x0_get_base_nand();
    volatile unsigned char *p = (volatile unsigned char *)&nand_chip->nfaddr;
    
    *p = addr & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 9) & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 17) & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 25) & 0xff;
    for(i=0; i<10; i++);
}


/* 发出地址 */
static void s3c2440_write_addr_lp(unsigned int addr)
{
    int i;
	struct s3c24x0_nand * nand_chip = s3c24x0_get_base_nand();
    volatile unsigned char *p = (volatile unsigned char *)&nand_chip->nfaddr;
	int col, page;

	col = addr & NAND_BLOCK_MASK_LP;
	page = addr / NAND_SECTOR_SIZE_LP;
	
    *p = col & 0xff;			/* Column Address A0~A7 */
    for(i=0; i<10; i++);		
    *p = (col >> 8) & 0x0f;		/* Column Address A8~A11 */
    for(i=0; i<10; i++);
    *p = page & 0xff;			/* Row Address A12~A19 */
    for(i=0; i<10; i++);
    *p = (page >> 8) & 0xff;	/* Row Address A20~A27 */
    for(i=0; i<10; i++);
    *p = (page >> 16) & 0x03;	/* Row Address A28~A29 */
    for(i=0; i<10; i++);
}

/* 读取数据 */
static unsigned char s3c2440_read_data(void)
{
	struct s3c24x0_nand * nand_chip = s3c24x0_get_base_nand();
    volatile unsigned char *p = (volatile unsigned char *)&nand_chip->nfdata;
    return *p;
}

/* 复位 */
static void s3c2440_nand_reset(void)
{
    s3c2440_nand_select_chip();
    s3c2440_write_cmd(0xff);  // 复位命令
    s3c2440_wait_idle();
    s3c2440_nand_deselect_chip();
}
/* 在第一次使用NAND Flash前，复位一下NAND Flash */
static void nand_reset(void)
{
    s3c2440_nand_reset();
}

static void wait_idle(void)
{
    s3c2440_wait_idle();
}

static void nand_select_chip(void)
{
    int i;
	
    s3c2440_nand_select_chip();
	
    for(i=0; i<10; i++);
}

static void nand_deselect_chip(void)
{
	s3c2440_nand_deselect_chip();
}

static void write_cmd(int cmd)
{
    s3c2440_write_cmd(cmd);
}
static void write_addr(unsigned int addr)
{
    s3c2440_write_addr(addr);
}

static void write_addr_lp(unsigned int addr)
{
    s3c2440_write_addr_lp(addr);
}

static unsigned char read_data(void)
{
    return s3c2440_read_data();
}

/* 初始化NAND Flash */
void nand_init_ll(void)
{
	struct s3c24x0_nand * nand_chip = s3c24x0_get_base_nand();

#define TACLS   0
#define TWRPH0  4
#define TWRPH1  2

		/* 设置时序 */
    nand_chip->nfconf = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
        /* 使能NAND Flash控制器, 初始化ECC, 禁止片选 */
    nand_chip->nfcont = (1<<4)|(1<<1)|(1<<0);

	/* 复位NAND Flash */
	nand_reset();
}


/* 读函数 */
void nand_read_ll(unsigned char *buf, unsigned long start_addr, int size)
{
    int i, j;
    
    if ((start_addr & NAND_BLOCK_MASK) || (size & NAND_BLOCK_MASK)) {
        return ;    /* 地址或长度不对齐 */
    }

    /* 选中芯片 */
    nand_select_chip();

    for(i=start_addr; i < (start_addr + size);) {
      /* 发出READ0命令 */
      write_cmd(0);

      /* Write Address */
      write_addr(i);
      wait_idle();

      for(j=0; j < NAND_SECTOR_SIZE; j++, i++) {
          *buf = read_data();
          buf++;
      }
    }

    /* 取消片选信号 */
    nand_deselect_chip();
    
    return ;
}


/* 读函数 
  * Large Page
  */
void nand_read_ll_lp(unsigned char *buf, unsigned long start_addr, int size)
{
    int i, j;
    
    if ((start_addr & NAND_BLOCK_MASK_LP) || (size & NAND_BLOCK_MASK_LP)) {
        return ;    /* 地址或长度不对齐 */
    }

    /* 选中芯片 */
    nand_select_chip();

    for(i=start_addr; i < (start_addr + size);) {
      /* 发出READ0命令 */
      write_cmd(0);

      /* Write Address */
      write_addr_lp(i);
	  write_cmd(0x30);
      wait_idle();

      for(j=0; j < NAND_SECTOR_SIZE_LP; j++, i++) {
          *buf = read_data();
          buf++;
      }
    }

    /* 取消片选信号 */
    nand_deselect_chip();
    
    return ;
}

void nand_init(void)
{
    /* 初始化NAND Flash */
    nand_init_ll();
}

int nand_spl_load_image(uint32_t offs, unsigned int size, void *dst)
{
    /* 从 NAND Flash启动 */
    nand_read_ll_lp(dst, offs, (size + NAND_BLOCK_MASK_LP)&~(NAND_BLOCK_MASK_LP));
	return 0;
}

/* Unselect after operation */
void nand_deselect(void)
{
}

