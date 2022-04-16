// 61911650 高野遥斗

#include "buffer.h"

struct status_table {
    unsigned int stat;
    char stat_char;
}
stat_tbl[] = {
    {STAT_OLD, 'O'},
    {STAT_WAITED, 'W'},
    {STAT_KRDWR, 'K'},
    {STAT_DWR, 'D'},
    {STAT_VALID, 'V'},
    {STAT_LOCKED, 'L'},
    {0, 0}
};

int buf_number = 0;
struct buf_header hash_head[NHASH];
struct buf_header free_head;

/* cmd */

int init_cmd()
{
    int i;
    int init_buf[12] = {28, 4, 64, 17, 5, 97, 98, 50, 10, 3, 35, 99};
    int init_free[6] = {10, 97, 28, 4, 5, 3};

    remove_all();
    for (i = 0; i < 12; i++) {
        int blkno = init_buf[i];
        struct buf_header *p = buf_create(blkno);
        if (p) {
            set_status(p, STAT_LOCKED | STAT_VALID, SET);
            insert(&hash_head[hash(blkno)], p , TYPE_HASH, LIST_TAIL);
        } else {
            fprintf(stderr, "malloc error\n");
            return -1;
        }
    }
    for (i = 0; i < 6; i++) {
        struct buf_header *p = search_hash(init_free[i]);
        if (p) {
            insert(&free_head, p, TYPE_FREE, LIST_HEAD);
        } else {
            fprintf(stderr, "invalid initial free buffer of blkno %d\n", init_free[i]);
            return -1;
        }
    }
    return 0;
}

int buf_cmd()
{
    int i;
    struct buf_header *p;

    for(i = 0;i < NHASH; i++) {
        for (p = hash_head[i].hash_fp; p != &hash_head[i]; p = p->hash_fp) {
            print_buf(p);
            printf("\n");
        }
    }
    return 0;
}

int buf1_cmd(int bufno)
{
    struct buf_header *p;

    p = search_buf(bufno);
    if (p) {
        print_buf(p);
        printf("\n");
    } else {
        fprintf(stderr, "buf %d not found\n", bufno);
        return -1;
    }
    return 0;
}

int hash_cmd()
{
    int i;

    for(i = 0;i < NHASH; i++) {
        if (hash1_cmd(i) < 0) {
            return -1;
        };
    }
    return 0;
}

int hash1_cmd(int n)
{
    struct buf_header *p;

    printf("%d:", n);
    for (p = hash_head[n].hash_fp; p != &hash_head[n]; p = p->hash_fp) {
        printf(" ");
        print_buf(p);
    }
    printf("\n");
    return 0;
}

int free_cmd()
{
    struct buf_header *p;

    for (p = free_head.free_fp; p != &free_head; p = p->free_fp) {
        printf(" ");
        print_buf(p);
    }
    if (free_head.free_fp != &free_head) {
        printf("\n");
    }
    return 0;
}

int set_cmd(int blkno, char stat_char)
{
    return do_set(blkno, stat_char, SET);
}

int reset_cmd(int blkno, char stat_char)
{
    return do_set(blkno, stat_char, RESET);
}

void quit_cmd()
{
    remove_all();
    exit(0);
}

int getblk_cmd(int blkno)
{
    struct buf_header *p = do_getblk(blkno);
    if (p) {
        return 0;
    }
    return 0; // return -1;
}

int brelse_cmd(int blkno)
{
    struct buf_header *p = search_hash(blkno);
    if (p) {
        return do_brelse(search_hash(blkno));
    }
    fprintf(stderr, "blkno %d not found\n", blkno);
    return -1;
}

/* utility */

struct buf_header *buf_create(int blkno)
{
    struct buf_header *p;

    if ((p = malloc(sizeof(struct buf_header)))) {
        p->blkno = blkno;
        p->bufno = buf_number++;
    }

    return p;
}

void auto_init()
{
    int i;

    for(i = 0;i < NHASH; i++) {
        hash_head[i].hash_fp = &hash_head[i];
        hash_head[i].hash_bp = &hash_head[i];
    }
    free_head.free_fp = &free_head;
    free_head.free_bp = &free_head;
}

int do_set(int blkno, char stat_char, int set)
{
    struct buf_header *bp;
    struct status_table *sp;

    bp = search_hash(blkno);
    if (bp) {
        for (sp = stat_tbl; sp->stat; sp++) {
            if (stat_char == sp->stat_char) {
                set_status(bp, sp->stat, set);
                return 0;
            }
        }
        fprintf(stderr, "stat %c is invalid\n", stat_char);
    } else {
        fprintf(stderr, "blkno %d not found\n", blkno);
    }
    return -1;
}

void set_status(struct buf_header *p, unsigned int stat, enum set_type set)
{
    // printf("%2d:%3d ", p->bufno, p->blkno);
    // print_status(p);
    // printf(" -> ");
    if (set == SET) {
        p->stat |= stat;
    } else if (set == RESET) {
        p->stat &= ~stat;
    }
    // print_status(p);
    // printf("\n");
}

void insert(struct buf_header *h, struct buf_header *p, enum list_type type, enum list_where where)
{
    if (h && p) {
        if (type == TYPE_HASH) {
                if (where == LIST_HEAD) {
                    p->hash_bp = h;
                    if (h->hash_fp) {
                        p->hash_fp = h->hash_fp;
                        h->hash_fp->hash_bp = p;
                    } else {
                        p->hash_fp = h;
                        h->hash_bp = p;
                    }
                    h->hash_fp = p;
                } else if (where == LIST_TAIL) {
                    p->hash_fp = h;
                    if (h->hash_bp) {
                        p->hash_bp = h->hash_bp;
                        h->hash_bp->hash_fp = p;
                    } else {
                        p->hash_bp = h;
                        h->hash_fp = p;
                    }
                    h->hash_bp = p;
                }
            } else if (type == TYPE_FREE) {
                set_status(p, STAT_LOCKED, RESET);
                if (where == LIST_HEAD) {
                    p->free_bp = h;
                    if (h->free_fp) {
                        p->free_fp = h->free_fp;
                        h->free_fp->free_bp = p;
                    } else {
                        p->free_fp = h;
                        h->free_bp = p;
                    }
                    h->free_fp = p;
                } else if (where == LIST_TAIL) {
                    p->free_fp = h;
                    if (h->free_bp) {
                        p->free_bp = h->free_bp;
                        h->free_bp->free_fp = p;
                    } else {
                        p->free_bp = h;
                        h->free_fp = p;
                    }
                    h->free_bp = p;
                }
            }
    } else {
        fprintf(stderr, "pointer invalid \n");
    }
}

void print_status(unsigned int stat)
{
    struct status_table *p;

    for (p = stat_tbl; p->stat; p++) {
        if (stat & p->stat) {
            printf("%c", p->stat_char);
        } else {
            printf("-");
        }
    }
}

void print_buf(struct buf_header *p)
{
    if (p) {
        printf("[%2d:%3d ", p->bufno, p->blkno);
        print_status(p->stat);
        printf("]");   
    }
}

struct buf_header *search_buf(int bufno)
{
    int i;
    struct buf_header *p;

    for(i = 0; i < NHASH; i++) {
        for (p = hash_head[i].hash_fp; p != &hash_head[i]; p = p->hash_fp) {
            if (p->bufno == bufno) {
                return p;
            }
        }
    }
    return NULL;
}

struct buf_header *search_hash(int blkno)
{
    int h;
    struct buf_header *p;

    h = hash(blkno);
    for (p = hash_head[h].hash_fp; p != &hash_head[h]; p = p->hash_fp) {
        if (p->blkno == blkno){
            return p;
        }
    }
    return NULL;
}

void remove_all() {
    int i;
    struct buf_header *p;

    for(i = 0; i < NHASH; i++) {
        p = hash_head[i].hash_fp;
        while (p != &hash_head[i]) {
            struct buf_header *next_p = p->hash_fp;
            remove_buf(p);
            p = next_p;
        }
    }
}

void remove_buf(struct buf_header *p)
{
    remove_from_hash(p);
    remove_from_free(p);
    buf_number--;
    free(p);
}

void remove_from_hash(struct buf_header *p)
{
    set_status(p, STAT_VALID, RESET);
    p->hash_bp->hash_fp = p->hash_fp;
    p->hash_fp->hash_bp = p->hash_bp;
    p->hash_fp = NULL;
    p->hash_bp = NULL;
}

void remove_from_free(struct buf_header *p)
{
    set_status(p, STAT_LOCKED, SET);
    if (p->free_bp) {
        p->free_bp->free_fp = p->free_fp;
        p->free_fp->free_bp = p->free_bp;
        p->free_fp = NULL;
        p->free_bp = NULL;
    }
}

void move_hash(struct buf_header *p, int blkno) {
    if (p) {
        remove_from_hash(p);
        p->blkno = blkno;
        insert(&hash_head[hash(blkno)], p, TYPE_HASH, LIST_TAIL);
    } else {
        fprintf(stderr, "cannot move null\n");
    }
}

struct buf_header *do_getblk(int blkno)
{
    struct buf_header *p;
    while (1) {
        if ((p = search_hash(blkno))) {
            if (p->stat & STAT_LOCKED) {
                /* シナリオ 5 */
                // sleep()
                set_status(p, STAT_WAITED, SET);
                printf("Process goes to sleep");
                return NULL; //continue;
            }
            /* シナリオ 1 */
            remove_from_free(p);
            return p;
        } else {
            p = free_head.free_fp;
            if (p == &free_head) {
                /* シナリオ 4 */
                //sleep()
                set_status(p, STAT_WAITED, SET); // ヘッドをWAITEDにする
                printf("Process goes to sleep");
                return NULL; // continue;
            }
            remove_from_free(p);
            if (p->stat & STAT_DWR) {
                /* シナリオ 3 */
                set_status(p, STAT_DWR, RESET);
                set_status(p, STAT_KRDWR, SET);
                set_status(p, STAT_OLD, SET);

                //asynchronous write buffer to disk;
                continue;
            }
            
            /* シナリオ 2 */
            move_hash(p, blkno);
            set_status(p, STAT_KRDWR, SET);
            // データ読み込み
            set_status(p, STAT_KRDWR, RESET);
            set_status(p, STAT_VALID, SET);
            return p;
        }
    }
}
int do_brelse(struct buf_header *p)
{
    if (p->stat & STAT_WAITED) {
        // シナリオ 5
        // wakeup();
        set_status(p, STAT_WAITED, RESET);
        printf("Wakeup processes waiting for buffer of blkno %d\n", p->blkno);
    }
    if (free_head.stat & STAT_WAITED) {
        // シナリオ 4
        // wakeup();
        set_status(&free_head, STAT_WAITED, RESET);
        printf("Wakeup processes waiting for any buffer\n");
    }

    // raise_cpu_level();
    // raise processor execution level to block interrupts;

    if (((p->stat & STAT_VALID) | (p->stat & STAT_OLD)) == STAT_VALID) {
        insert(&free_head, p, TYPE_FREE, LIST_TAIL);
    } else {
        // シナリオ 3
        insert(&free_head, p, TYPE_FREE, LIST_HEAD);
        set_status(p, STAT_OLD, RESET);
    }
    set_status(p, STAT_LOCKED, RESET);
    return 0;
}
