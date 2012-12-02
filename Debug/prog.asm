dseg	SEGMENT
sum	DW	?
dseg	ENDS

cseg	SEGMENT
ASSUME	ds:dseg, cs:cseg
__program_start__:

	MOV	g, 1

	MOV	AX, 4C00h
	INT	21h

cseg	ENDS
	END __program_start__
