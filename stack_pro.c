#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAKE_NAME(a, b) a##b

#define ELEMENT_FMT(type)  \
    _Generic(((type) 0),   \
    int:      "%d",        \
    float:    "%f",        \
    double:   "%f",        \
    unsigned: "%u",        \
    default:  "%f"         \
)



const size_t CANARY = 19;
const size_t CONTROL_DIMENS = 200;
const size_t NULL_HASH = 0xdd233d4;
void *const ERR_PTR    = (void*) 1; // DATA
const int ERR_RESIZE   = 0;
//const int ERR_POP_SIZE = 0;

#define FORMATE(type)               \
typedef struct MAKE_NAME(Stack_, type){\
    size_t first_canary;            \
                                    \
    type* data;                     \
    size_t size;                    \
    size_t capacity;                \
                                    \
    size_t hash;                    \
                                    \
    size_t second_canary;           \
} MAKE_NAME(Stack_, type);          \
                                    \
int stack_resize_##type (MAKE_NAME(Stack_, type)* stk);                                     \
int stack_init_##type (MAKE_NAME(Stack_, type)* stk, int capacity);                         \
void stack_dtor_##type (MAKE_NAME(Stack_, type)* stk);                                      \
int stack_push_##type (MAKE_NAME(Stack_, type)* stk, type element);                         \
int stack_pop_##type (MAKE_NAME(Stack_, type)* stk, type* element);                         \
void stack_dump_##type (MAKE_NAME(Stack_, type)* stk, int, int, const char*, const char*);  \
int is_stack_spoiled_##type (MAKE_NAME(Stack_, type)* stk);                                 \
void DragonHash_##type (MAKE_NAME(Stack_, type)* stk);                                      \





#define DragonHash(stk_ptr, ...)    \
    _Generic( ((*stk_ptr).data),    \
    int*:      DragonHash_int,      \
    float*:    DragonHash_float,    \
    double*:   DragonHash_double,   \
    unsigned*: DragonHash_unsigned, \
    default:   DragonHash_int       \
)(stk_ptr)

#define stack_init(stk_ptr, ...)    \
    _Generic( ((*stk_ptr).data),    \
    int*: stack_init_int,           \
    float*: stack_init_float,       \
    double*: stack_init_double,     \
    unsigned*:stack_init_unsigned,  \
    default: stack_init_double      \
)(stk_ptr, __VA_ARGS__)


#define stack_pop(stk_ptr, ...)     \
    _Generic( ((*stk_ptr).data),    \
    int*: stack_pop_int,            \
    float*: stack_pop_float,       \
    double*: stack_pop_double,      \
    unsigned*:stack_pop_unsigned,   \
    default: stack_pop_double       \
)(stk_ptr, __VA_ARGS__)



#define stack_resize(stk_ptr, ...)  \
    _Generic( ((*stk_ptr).data),    \
    int*: stack_resize_int,         \
    float*: stack_resize_float,       \
    double*: stack_resize_double,   \
    unsigned*:stack_resize_unsigned,\
    default: stack_resize_int       \
)(stk_ptr)

#define stack_dtor(stk_ptr, ...)    \
    _Generic( ((*stk_ptr).data),    \
    int*: stack_dtor_int,           \
    float*: stack_dtor_float,       \
    double*: stack_dtor_double,     \
    unsigned*:stack_dtor_unsigned,  \
    default: stack_dtor_int         \
)(stk_ptr)



#define stack_push(stk_ptr, ...)    \
    _Generic( ((*stk_ptr).data),    \
    int*: stack_push_int,           \
    float*: stack_push_float,       \
    double*: stack_push_double,     \
    unsigned*:stack_push_unsigned,  \
    default: stack_push_double      \
)(stk_ptr, __VA_ARGS__)



#define is_stack_spoiled(stk_ptr, ...)  \
    _Generic( ((*stk_ptr).data),        \
    int*: is_stack_spoiled_int,         \
    float*: is_stack_spoiled_float,       \
    double*: is_stack_spoiled_double,   \
    unsigned*:is_stack_spoiled_unsigned,\
    default: is_stack_spoiled_int       \
)(stk_ptr)



#define stack_dump(stk_ptr, ...)    \
    _Generic( ((*stk_ptr).data),    \
    int*: stack_dump_int,           \
    float*: stack_dump_float,       \
    double*: stack_dump_double,     \
    unsigned*:stack_dump_unsigned,  \
    default*: stack_dump_int        \
)(stk_ptr, __VA_ARGS__)





#define MAKE_STACK(type)                                \
                                                        \
void DragonHash_##type (MAKE_NAME(Stack_, type)* stk)   \
{                                                       \
    assert(stk);                                        \
                                                        \
    stk->hash = 0;                                      \
                                                        \
    size_t records_key = 0;                             \
                                                        \
    const unsigned char* struct_records = (const unsigned char*) stk;       \
    const unsigned char* stack_data     = (const unsigned char*) stk->data; \
                                                                            \
    for(size_t ind = 3; ind < sizeof(MAKE_NAME(Stack_, type)); ind++)       \
    {                                                                       \
        records_key += 2 * struct_records[ind - 1] + struct_records[ind - 2] % 2 + struct_records[ind - 3]; \
    }                                                                                                       \
                                                                                                            \
    switch(records_key % 4)                     \
    {                                           \
        case 0:                                 \
        {                                       \
            records_key = records_key << 4 + 1; \
                                                \
            break;                              \
        }                                       \
                                                \
        case 1:                                 \
        {                                       \
            records_key = records_key << 8 + 3; \
                                                \
            break;                              \
        }                                       \
                                                \
        case 2:                                 \
        {                                       \
            records_key = records_key << 12 + 5;\
                                                \
            break;                              \
        }                                       \
                                                \
        case 3:                                 \
        {                                       \
            records_key = records_key << 16 + 7;\
                                                \
            break;                              \
        }                                       \
    }                                           \
                                                \
    size_t data_key = NULL_HASH;                \
                                                \
    const size_t size_converter_value = sizeof(type)/sizeof(char);                  \
                                                                                    \
    for(size_t ind = 1; ind < (stk->capacity + 2) * size_converter_value ; ind++)   \
    {                                                                               \
        data_key = data_key << 3 + 1;                                               \
        data_key = data_key ^ stack_data[ind] + data_key % 3 + stack_data[ind] + stack_data[ind - 1] | stack_data[ind];\
    }                                                                                           \
                                                                                                \
    size_t secureID[3 + sizeof(MAKE_NAME(Stack_, type))] = {NULL_HASH, records_key, data_key};  \
                                                                                                \
    size_t ind = 0;                                                                             \
                                                                                                \
    for(ind = 3; ind < sizeof(MAKE_NAME(Stack_, type)) / 2; ind++)                              \
    {                                                                                           \
        secureID[ind] = secureID[ind - 1] + secureID[ind - 1] % 5 + 2 * secureID[ind - 2] + secureID[ind - 3] % 13 ;\
    }                                                                                           \
                                                                                                \
    stk->hash = secureID[ind - 1];                                                              \
}                                                                                               \
                                                                                                \
                                                                                                \
int is_stack_spoiled_##type(MAKE_NAME(Stack_, type)* stk)                                       \
{                                                                                               \
    assert(stk);                                                                                \
                                                                                                \
    if(stk->first_canary != CANARY || stk->second_canary != CANARY)                             \
        return -1;                                                                              \
                                                                                                \
    if(stk->data[0] != CANARY || stk->data[stk->capacity + 1] != CANARY)                        \
        return -2;                                                                              \
                                                                                                \
    if(stk->size > CONTROL_DIMENS)                                                              \
        return -3;                                                                              \
                                                                                                \
    if(stk->capacity > CONTROL_DIMENS)                                                          \
        return -4;                                                                              \
                                                                                                \
    if(stk->size > stk->capacity)                                                               \
        return -5;                                                                              \
                                                                                                \
    if(stk->data == NULL)                                                                       \
        return -6;                                                                              \
                                                                                                \
                                                                                                \
    size_t hash_copy = stk->hash;                                                               \
                                                                                                \
    DragonHash_##type(stk);                                                                     \
                                                                                                \
    if(hash_copy != stk->hash)                                                                  \
        return -7;                                                                              \
                                                                                                \
    return 0;                                                                                   \
}                                                                                               \
                                                                                                \
                                                                                                \
                                                                                                \
void stack_dump_##type(MAKE_NAME(Stack_, type)* stk, int key, int line, const char* file, const char* function)\
{                                                                                               \
    printf("ERROR WAS HAPPENED IN FUNCTION:\n%s\n LINE = %d\n FILE = %s\n\n",function, line, file);\
                                                                                                \
    switch(key)                                                                                 \
    {                                                                                           \
        case -1:                                                                                \
        {                                                                                       \
            printf("YOUR STRUCT CANARY WAS CHANGED!!!");                                        \
                                                                                                \
            break;                                                                              \
        }                                                                                       \
                                                                                                \
        case -2:                                                                                \
        {                                                                                       \
            printf("YOUR DATA CANARY WAS CHANGED!!!!!!");                                       \
                                                                                                \
            break;                                                                              \
        }                                                                                       \
                                                                                                \
        case -3:                                                                                \
        {                                                                                       \
            printf("SIZE OF DATA BIGGER THAN CONTROL_DIMENS!!! GO AWAY!!! SAVE YOUR (dark) SOULS!!!");\
                                                                                                \
            break;                                                                              \
        }                                                                                       \
                                                                                                \
        case -4:                                                                                \
        {                                                                                       \
            printf("CAPACITY OF DATA BIGGER THAN CONTROL_DIMENS!!! ZOMBIE EMERGENCY!!!!!!!!");  \
                                                                                                \
            break;                                                                              \
        }                                                                                       \
                                                                                                \
        case -5:                                                                                \
        {                                                                                       \
            printf("SIZE BIGGER THAN CAPACITY. THAT'S NOT SO GOOD. FORMATTING YOUR DRIVES..."); \
                                                                                                \
            break;                                                                              \
        }                                                                                       \
                                                                                                \
        case -6:                                                                                \
        {                                                                                       \
            printf("DATA POINTER IS NULL. SEND $100 TO THE AUTHOR.");                           \
                                                                                                \
            break;                                                                              \
        }                                                                                       \
                                                                                                \
        case -7:                                                                                \
        {                                                                                       \
            printf("HASH WAS CHANGED. GO LEARN MATHAN, BABY");                                  \
                                                                                                \
            break;                                                                              \
        }                                                                                       \
                                                                                                \
        default:                                                                                \
        {                                                                                       \
            printf("DUMP ERROR. I CANT BEAR IT. ABORTING... (no)");                             \
                                                                                                \
            break;                                                                              \
        }                                                                                       \
    }                                                                                           \
                                                                                                \
    printf("\n\nDUMP RESULT:"                    "\n"                                           \
           "first canary:   %3zu;  REAL CANARY = %zu\n"                                         \
           "second canary:  %3zu;  REAL CANARY = %zu\n"                                         \
           "capacity:       %3zu"                 "\n"                                          \
           "size:           %3zu"                 "\n"                                          \
           "data[%p]\n{ \n",                                                                    \
                                                                                                \
            stk->first_canary,                                                                  \
            CANARY,                                                                             \
            stk->second_canary,                                                                 \
            CANARY,                                                                             \
            stk->capacity,                                                                      \
            stk->size,                                                                          \
            stk->data);                                                                         \
                                                                                                \
    int ind_capacity = stk->capacity;                                                           \
                                                                                                \
    printf("  data[%d] =  ", ind_capacity + 1);                                                 \
                                                                                                \
    printf(ELEMENT_FMT(type), stk->data[ind_capacity + 1]);                                     \
                                                                                                \
    printf("  This is a CANARY; REAL CANARY = %zu\n", CANARY);                                  \
                                                                                                \
    for (; ind_capacity > 0; --ind_capacity)                                                    \
    {                                                                                           \
        printf("  data[%d] =  ", ind_capacity);                                                 \
        printf(ELEMENT_FMT(type), stk->data[ind_capacity]);                                     \
        printf("\n");                                                                           \
                                                                                                \
    }                                                                                           \
                                                                                                \
    printf("  data[%d] =  ", 0);                                                                \
    printf(ELEMENT_FMT(type), stk->data[0]);                                                    \
    printf("  This is a CANARY; REAL CANARY = %zu\n} \n", CANARY);                              \
}                                                                                               \
                                                                                                \
                                                                                                \
int stack_init_##type(MAKE_NAME(Stack_, type)* stk, int capacity)                               \
{                                                                                               \
    assert(stk);                                                                                \
                                                                                                \
    if(capacity <= 0)                                                                           \
    {                                                                                           \
        printf(" Your capacity is wrong, it must be positive.");                                \
                                                                                                \
        return 0;/*exit(1);*/                                                                   \
    }                                                                                           \
                                                                                                \
    stk->data = (type*) calloc(capacity + 2, sizeof(type));                                     \
    assert(stk->data);                                                                          \
                                                                                                \
    stk->capacity = capacity;                                                                   \
    stk->size = 0;                                                                              \
                                                                                                \
    stk->data[0] = CANARY;                                                                      \
    stk->data[capacity + 1] = CANARY;                                                           \
                                                                                                \
    stk->first_canary  = CANARY;                                                                \
    stk->second_canary = CANARY;                                                                \
                                                                                                \
    DragonHash_##type(stk);                                                                     \
                                                                                                \
    return 1;                                                                                   \
}                                                                                               \
                                                                                                \
                                                                                                \
int stack_pop_##type(MAKE_NAME(Stack_, type)* stk, type* element)                               \
{                                                                                               \
    assert(stk);                                                                                \
    assert(stk->size >= 1);                                                                     \
                                                                                                \
    int key = 0;                                                                                \
                                                                                                \
    if(key = is_stack_spoiled_##type(stk))                                                      \
    {                                                                                           \
        stack_dump_##type(stk, key, __LINE__, __FILE__, __PRETTY_FUNCTION__);                   \
    }                                                                                           \
                                                                                                \
    *element = stk->data[stk->size];                                                            \
                                                                                                \
    --(stk->size);                                                                              \
                                                                                                \
    DragonHash_##type(stk);                                                                     \
                                                                                                \
    return 1;                                                                                   \
}                                                                                               \
                                                                                                \
                                                                                                \
int stack_resize_##type(MAKE_NAME(Stack_, type)* stk)                                           \
{                                                                                               \
    stk->data = (type*) realloc(stk->data, (stk->capacity + 1) * 2                              \
                                            * sizeof(type));                                    \
                                                                                                \
    if(stk->data == NULL)                                                                       \
        return 0;                                                                               \
                                                                                                \
    stk->capacity = 2 * stk->capacity;                                                          \
    stk->data[stk->capacity + 1] = CANARY;                                                      \
                                                                                                \
    return 1;                                                                                   \
}                                                                                               \
                                                                                                \
                                                                                                \
void stack_dtor_##type(MAKE_NAME(Stack_, type)* stk)                                            \
{                                                                                               \
    assert(stk);                                                                                \
                                                                                                \
    free(stk->data);                                                                            \
                                                                                                \
    stk->data = ERR_PTR;                                                                        \
}                                                                                               \
                                                                                                \
                                                                                                \
                                                                                                \
                                                                                                \
int stack_push_##type(MAKE_NAME(Stack_, type)* stk, type element)           \
{                                                                           \
    assert(stk);                                                            \
                                                                            \
    int key = 0;                                                            \
                                                                            \
    if(key = is_stack_spoiled_##type(stk))                                  \
    {                                                                       \
        stack_dump_##type(stk, key, __LINE__, __FILE__, __PRETTY_FUNCTION__);\
    }                                                                       \
                                                                            \
    if(stk->size >= stk->capacity)                                          \
        if(!stack_resize_##type(stk))                                       \
            return ERR_RESIZE;                                              \
                                                                            \
    stk->data[stk->size + 1] = element;                                     \
                                                                            \
    ++(stk->size);                                                          \
                                                                            \
    DragonHash_##type(stk);                                                 \
                                                                            \
    return 1;                                                               \
}




//=======================================================================
FORMATE(int);
MAKE_STACK(int);

FORMATE(float);
MAKE_STACK(float);

FORMATE(double);
MAKE_STACK(double);

FORMATE(unsigned);
MAKE_STACK(unsigned);


/*
int main(void)
{
    Stack_int a = {};
    Stack_double b = {};

    Stack_float ff = {};
    stack_init(&b, 2);
    stack_init(&a, 2);

    int flora = 11;

    stack_push(&a, 18);
    stack_push(&a, flora);

    int x = 0; double y = 0;

    stack_pop(&a, &x);

    printf("__%d___", x);

    stack_pop(&a, &x);

    printf("__%d___\n", x);

    stack_push(&b, 19);
    stack_push(&b, 20);

    stack_pop(&b, &y);

    printf("%.2f\n", y);

    stack_pop(&b, &y);

    printf("%.2f\n", y);


    Stack_unsigned in = {};

    stack_init(&in, 1);

    stack_push(&in, 2234567890);

    unsigned uns = 0;

    stack_pop(&in, &uns);

    printf("%u", uns);

    stack_dtor(&a);
    stack_dtor(&b);
    stack_dtor(&in);
}*/
