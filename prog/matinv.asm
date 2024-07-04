//
// version
//
	.vers	7

//
// execution framework
//
__start:
	call	_main
	call	_exit
__stop:
	jmp	__stop

//
// Integer readInteger()
//
_readInteger:
	asf	0
	rdint
	popr
	rsf
	ret

//
// void writeInteger(Integer)
//
_writeInteger:
	asf	0
	pushl	-3
	wrint
	rsf
	ret

//
// Character readCharacter()
//
_readCharacter:
	asf	0
	rdchr
	popr
	rsf
	ret

//
// void writeCharacter(Character)
//
_writeCharacter:
	asf	0
	pushl	-3
	wrchr
	rsf
	ret

//
// Integer char2int(Character)
//
_char2int:
	asf	0
	pushl	-3
	popr
	rsf
	ret

//
// Character int2char(Integer)
//
_int2char:
	asf	0
	pushl	-3
	popr
	rsf
	ret

//
// void exit()
//
_exit:
	asf	0
	halt
	rsf
	ret

//
// void writeString(String)
//
_writeString:
	asf	1
	pushc	0
	popl	0
	jmp	_writeString_L2
_writeString_L1:
	pushl	-3
	pushl	0
	getfa
	call	_writeCharacter
	drop	1
	pushl	0
	pushc	1
	add
	popl	0
_writeString_L2:
	pushl	0
	pushl	-3
	getsz
	lt
	brt	_writeString_L1
	rsf
	ret

//
// Integer gcd(Integer, Integer)
//
_gcd:
	asf	1
	jmp	__2
__1:
	pushl	-4
	pushl	-3
	mod
	popl	0
	pushl	-3
	popl	-4
	pushl	0
	popl	-3
__2:
	pushl	-3
	pushc	0
	ne
	brt	__1
__3:
	pushl	-4
	popr
	jmp	__0
__0:
	rsf
	ret

//
// record { Integer num; Integer den; } newFraction(Integer, Integer)
//
_newFraction:
	asf	4
	pushl	-4
	pushc	0
	lt
	brf	__5
	pushc	0
	pushl	-4
	sub
	popl	0
	jmp	__6
__5:
	pushl	-4
	popl	0
__6:
	pushl	-3
	pushc	0
	lt
	brf	__7
	pushc	0
	pushl	-3
	sub
	popl	1
	jmp	__8
__7:
	pushl	-3
	popl	1
__8:
	pushl	0
	pushl	1
	call	_gcd
	drop	2
	pushr
	popl	2
	new	2
	popl	3
	pushl	-4
	pushc	0
	lt
	pushl	-3
	pushc	0
	lt
	ne
	brf	__9
	pushl	3
	pushc	0
	pushl	0
	sub
	pushl	2
	div
	putf	0
	jmp	__10
__9:
	pushl	3
	pushl	0
	pushl	2
	div
	putf	0
__10:
	pushl	3
	pushl	1
	pushl	2
	div
	putf	1
	pushl	3
	popr
	jmp	__4
__4:
	rsf
	ret

//
// void writeFraction(record { Integer num; Integer den; })
//
_writeFraction:
	asf	0
	pushl	-3
	getf	0
	call	_writeInteger
	drop	1
	pushc	1
	newa
	dup
	pushc	0
	pushc	47
	putfa
	call	_writeString
	drop	1
	pushl	-3
	getf	1
	call	_writeInteger
	drop	1
__11:
	rsf
	ret

//
// record { Integer num; Integer den; } negFraction(record { Integer num; Integer den; })
//
_negFraction:
	asf	0
	pushc	0
	pushl	-3
	getf	0
	sub
	pushl	-3
	getf	1
	call	_newFraction
	drop	2
	pushr
	popr
	jmp	__12
__12:
	rsf
	ret

//
// record { Integer num; Integer den; } addFraction(record { Integer num; Integer den; }, record { Integer num; Integer den; })
//
_addFraction:
	asf	0
	pushl	-4
	getf	0
	pushl	-3
	getf	1
	mul
	pushl	-3
	getf	0
	pushl	-4
	getf	1
	mul
	add
	pushl	-4
	getf	1
	pushl	-3
	getf	1
	mul
	call	_newFraction
	drop	2
	pushr
	popr
	jmp	__13
__13:
	rsf
	ret

//
// record { Integer num; Integer den; } subFraction(record { Integer num; Integer den; }, record { Integer num; Integer den; })
//
_subFraction:
	asf	0
	pushl	-4
	getf	0
	pushl	-3
	getf	1
	mul
	pushl	-3
	getf	0
	pushl	-4
	getf	1
	mul
	sub
	pushl	-4
	getf	1
	pushl	-3
	getf	1
	mul
	call	_newFraction
	drop	2
	pushr
	popr
	jmp	__14
__14:
	rsf
	ret

//
// record { Integer num; Integer den; } mulFraction(record { Integer num; Integer den; }, record { Integer num; Integer den; })
//
_mulFraction:
	asf	0
	pushl	-4
	getf	0
	pushl	-3
	getf	0
	mul
	pushl	-4
	getf	1
	pushl	-3
	getf	1
	mul
	call	_newFraction
	drop	2
	pushr
	popr
	jmp	__15
__15:
	rsf
	ret

//
// record { Integer num; Integer den; } divFraction(record { Integer num; Integer den; }, record { Integer num; Integer den; })
//
_divFraction:
	asf	0
	pushl	-4
	getf	0
	pushl	-3
	getf	1
	mul
	pushl	-4
	getf	1
	pushl	-3
	getf	0
	mul
	call	_newFraction
	drop	2
	pushr
	popr
	jmp	__16
__16:
	rsf
	ret

//
// Fraction[][] newMatrix(record { Integer num; Integer den; }, record { Integer num; Integer den; }, record { Integer num; Integer den; }, record { Integer num; Integer den; })
//
_newMatrix:
	asf	1
	pushc	2
	newa
	popl	0
	pushl	0
	pushc	0
	pushc	2
	newa
	putfa
	pushl	0
	pushc	1
	pushc	2
	newa
	putfa
	pushl	0
	pushc	0
	getfa
	pushc	0
	pushl	-6
	putfa
	pushl	0
	pushc	0
	getfa
	pushc	1
	pushl	-5
	putfa
	pushl	0
	pushc	1
	getfa
	pushc	0
	pushl	-4
	putfa
	pushl	0
	pushc	1
	getfa
	pushc	1
	pushl	-3
	putfa
	pushl	0
	popr
	jmp	__17
__17:
	rsf
	ret

//
// void writeMatrix(Fraction[][])
//
_writeMatrix:
	asf	2
	pushc	0
	popl	0
	jmp	__20
__19:
	pushc	0
	popl	1
	jmp	__23
__22:
	pushl	-3
	pushl	0
	getfa
	pushl	1
	getfa
	call	_writeFraction
	drop	1
	pushc	2
	newa
	dup
	pushc	0
	pushc	32
	putfa
	dup
	pushc	1
	pushc	32
	putfa
	call	_writeString
	drop	1
	pushl	1
	pushc	1
	add
	popl	1
__23:
	pushl	1
	pushl	-3
	pushl	0
	getfa
	getsz
	lt
	brt	__22
__24:
	pushc	1
	newa
	dup
	pushc	0
	pushc	10
	putfa
	call	_writeString
	drop	1
	pushl	0
	pushc	1
	add
	popl	0
__20:
	pushl	0
	pushl	-3
	getsz
	lt
	brt	__19
__21:
	pushc	1
	newa
	dup
	pushc	0
	pushc	10
	putfa
	call	_writeString
	drop	1
__18:
	rsf
	ret

//
// Fraction[][] invertMatrix(Fraction[][])
//
_invertMatrix:
	asf	1
	pushl	-3
	pushc	0
	getfa
	pushc	0
	getfa
	pushl	-3
	pushc	1
	getfa
	pushc	1
	getfa
	call	_mulFraction
	drop	2
	pushr
	pushl	-3
	pushc	0
	getfa
	pushc	1
	getfa
	pushl	-3
	pushc	1
	getfa
	pushc	0
	getfa
	call	_mulFraction
	drop	2
	pushr
	call	_subFraction
	drop	2
	pushr
	popl	0
	pushl	0
	getf	0
	pushc	0
	eq
	brf	__26
	pushc	33
	newa
	dup
	pushc	0
	pushc	101
	putfa
	dup
	pushc	1
	pushc	114
	putfa
	dup
	pushc	2
	pushc	114
	putfa
	dup
	pushc	3
	pushc	111
	putfa
	dup
	pushc	4
	pushc	114
	putfa
	dup
	pushc	5
	pushc	58
	putfa
	dup
	pushc	6
	pushc	32
	putfa
	dup
	pushc	7
	pushc	109
	putfa
	dup
	pushc	8
	pushc	97
	putfa
	dup
	pushc	9
	pushc	116
	putfa
	dup
	pushc	10
	pushc	114
	putfa
	dup
	pushc	11
	pushc	105
	putfa
	dup
	pushc	12
	pushc	120
	putfa
	dup
	pushc	13
	pushc	32
	putfa
	dup
	pushc	14
	pushc	99
	putfa
	dup
	pushc	15
	pushc	97
	putfa
	dup
	pushc	16
	pushc	110
	putfa
	dup
	pushc	17
	pushc	110
	putfa
	dup
	pushc	18
	pushc	111
	putfa
	dup
	pushc	19
	pushc	116
	putfa
	dup
	pushc	20
	pushc	32
	putfa
	dup
	pushc	21
	pushc	98
	putfa
	dup
	pushc	22
	pushc	101
	putfa
	dup
	pushc	23
	pushc	32
	putfa
	dup
	pushc	24
	pushc	105
	putfa
	dup
	pushc	25
	pushc	110
	putfa
	dup
	pushc	26
	pushc	118
	putfa
	dup
	pushc	27
	pushc	101
	putfa
	dup
	pushc	28
	pushc	114
	putfa
	dup
	pushc	29
	pushc	116
	putfa
	dup
	pushc	30
	pushc	101
	putfa
	dup
	pushc	31
	pushc	100
	putfa
	dup
	pushc	32
	pushc	10
	putfa
	call	_writeString
	drop	1
	call	_exit
__26:
	pushl	-3
	pushc	1
	getfa
	pushc	1
	getfa
	pushl	0
	call	_divFraction
	drop	2
	pushr
	pushl	-3
	pushc	0
	getfa
	pushc	1
	getfa
	call	_negFraction
	drop	1
	pushr
	pushl	0
	call	_divFraction
	drop	2
	pushr
	pushl	-3
	pushc	1
	getfa
	pushc	0
	getfa
	call	_negFraction
	drop	1
	pushr
	pushl	0
	call	_divFraction
	drop	2
	pushr
	pushl	-3
	pushc	0
	getfa
	pushc	0
	getfa
	pushl	0
	call	_divFraction
	drop	2
	pushr
	call	_newMatrix
	drop	4
	pushr
	popr
	jmp	__25
__25:
	rsf
	ret

//
// void main()
//
_main:
	asf	3
	pushc	1
	newa
	dup
	pushc	0
	pushc	10
	putfa
	call	_writeString
	drop	1
	pushc	7
	pushc	1
	call	_newFraction
	drop	2
	pushr
	pushc	4
	pushc	1
	call	_newFraction
	drop	2
	pushr
	pushc	6
	pushc	1
	call	_newFraction
	drop	2
	pushr
	pushc	5
	pushc	1
	call	_newFraction
	drop	2
	pushr
	call	_newMatrix
	drop	4
	pushr
	popl	0
	pushl	0
	call	_writeMatrix
	drop	1
	pushl	0
	call	_invertMatrix
	drop	1
	pushr
	popl	1
	pushl	1
	call	_writeMatrix
	drop	1
	pushl	1
	call	_invertMatrix
	drop	1
	pushr
	popl	2
	pushl	2
	call	_writeMatrix
	drop	1
__27:
	rsf
	ret
