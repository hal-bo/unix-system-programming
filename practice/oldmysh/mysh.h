typedef char * token;

typedef struct block {
    token tok;
    struct block *next_block;
}

typedef struct line {
    int redirect_flg;
    struct line *next_proc;
}