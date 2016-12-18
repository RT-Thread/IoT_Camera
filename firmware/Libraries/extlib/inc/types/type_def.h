#ifndef __TYPE_DEF_H__
#define __TYPE_DEF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef unsigned long long FH_UINT64;
typedef long long FH_SINT64;
typedef unsigned int FH_UINT32;
typedef int FH_SINT32;
typedef unsigned short FH_UINT16;
typedef short FH_SINT16;
typedef unsigned char FH_UINT8;
typedef char FH_SINT8;
typedef FH_UINT8* FH_ADDR;

typedef char FH_CHAR;
typedef void FH_VOID;

typedef enum {
    FH_FALSE = 0,
    FH_TRUE  = 1,
    DUMMY    = 0xffffffff,
} FH_BOOL;

#define FH_NULL 0L
#define FH_SUCCESS 0
#define FH_FAILURE (-1)

/*fixed for enum type*/
#define INT_MINI (-2147483647 - 1) /* minimum (signed) int value */
#define INT_MAXI 2147483647        /* maximum (signed) int value */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TYPE_DEF_H__ */
