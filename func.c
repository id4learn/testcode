#include <stdio.h>
#include <stdint.h>
#include <array>


void one(int a, int b)
{
    printf("Addition is %d\n", a+b);
}
void two(int a, int b)
{
    printf("Subtraction is %d\n", a-b);
}
void three(int a, int b)
{
    printf("Multiplication is %d\n", a*b);
}
  
void four(int a, int b)
{
    printf("four\n");
}

void five(int a, int b)
{
    printf("five\n");
}

void six(int a, int b)
{
    printf("six\n");
}

typedef struct {
    uint32_t         valid;
    uint32_t         msgType;   // may or may not be available here depending on security header
    void             *allPdus;  // gprs-ns all message struct
    uint32_t         *msg;        // beginning of NAS message
    uint32_t         msgLen;    // total msg length
    uint32_t         curPos;    // points to beginning of message just after msg type
    uint32_t         ieLen;     // IE length
    uint32_t         isErrorValid;
    uint32_t         ieTag;
    uint32_t          error;
} NsEncDec;

struct NsMsgIeTableS;

uint32_t parse1(NsEncDec *info, NsMsgIeTableS *ieTbl);
uint32_t parse2(NsEncDec *info, NsMsgIeTableS *ieTbl);


typedef uint32_t (*NsEncDecFn)(NsEncDec *info, NsMsgIeTableS *ieTbl);

typedef struct NsMsgIeTableS 
{
    uint32_t          tag;
    uint32_t          ieFormat;
    uint32_t          iePresence;
    uint32_t          ieLen;       // For fixed length IEs, length in bits
    size_t            offsetVal;
    NsEncDecFn        encFn[1];
    NsEncDecFn        decFn[1];
} NsMsgIeTable;


#define MAX_STATE 0x10
#define MAX_EVT   0x10


std::array<NsMsgIeTable, 3> ns_reset_table =  {{ {1, 2, 3, 4 , 0x30000, &parse1 , &parse2} ,
                                                {6, 7, 34, 64 , 0x40000, &parse1 , &parse2} 
                                              }};

int main()
{

  NsEncDec  msg;
  uint32_t result = 0;

  //result =  ns_reset_table.encFn(&msg, );
  /*
    // fun_ptr_arr is an array of function pointers
    void (*fun_ptr_arr[MAX_STATE][MAX_EVT])(int, int) = {
                                        {one, two, three}, 
                                        {four, five, six} 
                                      }; */
    void (*fun_ptr_arr[MAX_STATE][MAX_EVT])(int, int) = {{}};

    for ( int row = 0 ; row <MAX_STATE ; row++) {
      for ( int col = 0 ; col <MAX_EVT ; col++) {

        fun_ptr_arr[row][col] = one;
      }
    }

    unsigned int ch, a = 15, b = 10;
  
#if 0
    printf("Enter Choice: 0 for add, 1 for subtract and 2 "
            "for multiply\n");
    scanf("%d", &ch);
  
    if (ch > 2) return 0;
#endif
  
    for ( int row = 0 ; row <2 ; row++) {
      for ( int col = 0 ; col <3 ; col++) {

        (*fun_ptr_arr[row][col])(a, b);
      }
    }

    return 0;
}
uint32_t parse1(NsEncDec *info, NsMsgIeTableS *ieTbl)
{
  printf("\n parse1 function call");

  return 1;

}
uint32_t parse2(NsEncDec *info, NsMsgIeTableS *ieTbl)
{
  printf("\n parse2 function call");
  return 1;

}

