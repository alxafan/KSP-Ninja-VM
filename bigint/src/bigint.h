/*
 * bigint.h -- big integer library
 */


#ifndef _BIGINT_H_
#define _BIGINT_H_


/* object representation */
typedef void* BigObjRef;


#include <stdio.h>


typedef struct {
  int nd;			/* number of digits; array may be bigger */
				/* nd = 0 exactly when number = 0 */
  unsigned char sign;		/* one of BIG_NEGATIVE or BIG_POSITIVE */
				/* zero always has BIG_POSITIVE here */
  unsigned char digits[1];	/* the digits proper; number base is 256 */
				/* LS digit first; MS digit is not zero */
} Big;

#include "support.h"


/* big integer processor registers */

typedef struct {
  BigObjRef op1;			/* first (or single) operand */
  BigObjRef op2;			/* second operand (if present) */
  BigObjRef res;			/* result of operation */
  BigObjRef rem;			/* remainder in case of division */
} BIP;

extern BIP bip;			/* registers of the processor */


/* big integer processor functions */

int bigSgn(void);			/* sign */
int bigCmp(void);			/* comparison */
void bigNeg(void);			/* negation */
void bigAdd(void);			/* addition */
void bigSub(void);			/* subtraction */
void bigMul(void);			/* multiplication */
void bigDiv(void);			/* division */

void bigFromInt(int n);			/* conversion int --> big */
int bigToInt(void);			/* conversion big --> int */

void bigRead(FILE *in);			/* read a big integer */
void bigPrint(FILE *out);		/* print a big integer */

void bigDump(FILE *out, BigObjRef bigObjRef);	/* dump a big integer object */


#endif /* _BIGINT_H_ */
