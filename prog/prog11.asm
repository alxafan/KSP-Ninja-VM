//
// version
//
	.vers	5

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
// void main()
//
_main:
	asf	2
	call	_readInteger
	pushr
	popl	0
	pushl	0
	call	_factorial
	drop	1
	pushr
	popl	1
	pushl	0
	call	_writeInteger
	drop	1
	pushc	33
	call	_writeCharacter
	drop	1
	pushc	32
	call	_writeCharacter
	drop	1
	pushc	61
	call	_writeCharacter
	drop	1
	pushc	32
	call	_writeCharacter
	drop	1
	pushl	1
	call	_writeInteger
	drop	1
	pushc	10
	call	_writeCharacter
	drop	1
__0:
	rsf
	ret

//
// Integer factorial(Integer)
//
_factorial:
	asf	0
	pushl	-3
	pushc	0
	eq
	brf	__2
	pushc	1
	popr
	jmp	__1
	jmp	__3
__2:
	pushl	-3
	pushl	-3
	pushc	1
	sub
	call	_factorial
	drop	1
	pushr
	mul
	popr
	jmp	__1
__3:
__1:
	rsf
	ret
