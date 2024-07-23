.vers 8

asf	2
pushc	10
popl	0
pushc	1
popl	1
_1:
pushl	0
pushl	1
mul
popl	1
pushl	0
pushc	1
sub
popl	0
pushl	0
pushc	1
le
brf	_1
pushl	1
wrint
pushc	10
wrchr
halt
