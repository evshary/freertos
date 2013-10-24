#define ENABLE_DEBUG
//#include "FreeRTOS.h"
#define ALIGNMENT_BITS 2
//#define portBYTE_ALIGNMENT      (1 << ALIGNMENT_BITS)
//#define portBYTE_ALIGNMENT_MASK (portBYTE_ALIGNMENT - 1)
#define configUSE_MALLOC_FAILED_HOOK 0
#define configTOTAL_HEAP_SIZE 4096
//#define pdFALSE 0
//#define pdTRUE 1

typedef int portBASE_TYPE;

//#define vTaskSuspendAll()
//#define xTaskResumeAll()

#ifdef ENABLE_DEBUG
#define DBGPRINTF(x)          printf("%s %d: ", __FILE__, __LINE__); printf(x)
#define DBGPRINTF1(x,a)       printf("%s %d: ", __FILE__, __LINE__); printf(x, a)
#define DBGPRINTF2(x,a,b)     printf("%s %d: ", __FILE__, __LINE__); printf(x, a, b)
#define DBGPRINTF3(x,a,b,c)   printf("%s %d: ", __FILE__, __LINE__); printf(x, a, b, c)
#else
#define DBGPRINTF(x)
#define DBGPRINTF1(x,a)
#define DBGPRINTF2(x,a,b)
#define DBGPRINTF3(x,a,b,c)
#endif

/* heap_ww.c */
void *pvPortMalloc(size_t xWantedSize);
void vPortFree(void *pv);
size_t xPortGetFreeHeapSize(void);

struct slot {
    void *pointer;
    unsigned int size;
    unsigned int lfsr;
};

#define CIRCBUFSIZE 5000
unsigned int write_pointer, read_pointer;
static struct slot slots[CIRCBUFSIZE];
static unsigned int lfsr = 0xACE1;

static unsigned int circbuf_size(void)
{
    return (write_pointer + CIRCBUFSIZE - read_pointer) % CIRCBUFSIZE;
}

static void write_cb(struct slot foo)
{
    if (circbuf_size() == CIRCBUFSIZE - 1) {
        printf(/*stderr,*/ "circular buffer overflow\n");
        return;
		//exit(1);
    }
    slots[write_pointer++] = foo;
    write_pointer %= CIRCBUFSIZE;
}

static struct slot read_cb(void)
{
    struct slot foo;
    if (write_pointer == read_pointer) {
        // circular buffer is empty
        return (struct slot){ .pointer=NULL, .size=0, .lfsr=0 };
    }
    foo = slots[read_pointer++];
    read_pointer %= CIRCBUFSIZE;
    return foo;
}


// Get a pseudorandom number generator from Wikipedia
static int prng(void)
{
    static unsigned int bit;
    /* taps: 16 14 13 11; characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr =  (lfsr >> 1) | (bit << 15);
    return lfsr & 0xffff;
}

