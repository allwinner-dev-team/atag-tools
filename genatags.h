#define GENATAGS_INITIAL_SIZE 10240

typedef unsigned int u32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;

typedef struct genatags {
    void *mem;
    unsigned int size;
    unsigned int used;
} genatags;

genatags *genatags_initialize(void);

int genatags_core(genatags *atags, u32 flags, u32 pagesize, u32 rootdev);
int genatags_mem(genatags *atags, u32 size, u32 start);
int genatags_initrd(genatags *atags, u32 size, u32 start);
int genatags_cmdline(genatags *atags, char *cmdline);
int genatags_none(genatags *atags);

int genatags_dump(genatags *atags, FILE* fp);
