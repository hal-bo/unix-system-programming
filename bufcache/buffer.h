// 61911650 高野遥斗

#ifndef BUFFER_H
# define BUFFER_H

# include <stdio.h>
# include <string.h>
# include <stdlib.h>

# define NHASH 4
# define STAT_LOCKED 0x00000001
# define STAT_VALID  0x00000010
# define STAT_DWR    0x00000100
# define STAT_KRDWR  0x00001000
# define STAT_WAITED 0x00010000
# define STAT_OLD    0x00100000

# define hash(n) n%NHASH

struct buf_header {
    int bufno;
    int blkno;
    struct buf_header *hash_fp;
    struct buf_header *hash_bp;
    unsigned int stat;
    struct buf_header *free_fp;
    struct buf_header *free_bp;
    char *cache_data;
};

enum list_where{
    LIST_HEAD,
    LIST_TAIL
} ;
enum list_type{
    TYPE_HASH,
    TYPE_FREE
};
enum set_type{
    SET,
    RESET
};

/* cmd */

int init_cmd();
int buf_cmd();
int buf1_cmd(int bufno);
int hash_cmd();
int hash1_cmd(int n);
int free_cmd();
int set_cmd(int blkno, char stat_char);
int reset_cmd(int blkno, char stat_char);
int getblk_cmd(int blkno);
int brelse_cmd(int blkno);
void quit_cmd();

/* utility */

struct buf_header *buf_create(int blkno);
void auto_init();
int do_set(int blkno, char stat_char, int set);
struct buf_header *do_getblk(int blkno);
int do_brelse(struct buf_header *p);
void set_status(struct buf_header *p, unsigned int stat, enum set_type set);
void insert(struct buf_header *h, struct buf_header *p, enum list_type type, enum list_where where);
void print_status(unsigned int stat);
void print_buf(struct buf_header *p);
struct buf_header *search_buf(int bufno);
struct buf_header *search_hash(int blkno);
void remove_all();
void remove_buf(struct buf_header *p);
void remove_from_hash(struct buf_header *p);
void remove_from_free(struct buf_header *p);
#endif