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
	asf	5
	pushc	1
	popl	0
	pushc	1
	popl	1
	call	_readInteger
	pushr
	popl	2
	pushc	2
	popl	3
	jmp	__2
__1:
	pushl	0
	pushl	1
	add
	popl	4
	pushl	1
	popl	0
	pushl	4
	popl	1
	pushl	3
	pushc	1
	add
	popl	3
__2:
	pushl	3
	pushl	2
	lt
	brt	__1
__3:
	pushc	70
	call	_writeCharacter
	drop	1
	pushc	105
	call	_writeCharacter
	drop	1
	pushc	98
	call	_writeCharacter
	drop	1
	pushc	40
	call	_writeCharacter
	drop	1
	pushl	2
	call	_writeInteger
	drop	1
	pushc	41
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
