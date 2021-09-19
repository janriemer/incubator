.(   Colon Compiler ) cr


\ Creates a header for a Forth word whose symbol name is a valid
\ assembler listing symbol name.
: header ( caddr u -- )
  2dup xref
  cr ." enter_" type ." :" cr ;

\ Creates a header for a Forth word whose symbol name is not a
\ valid assembler listing symbol name.  Two names are required,
\ one which is a valid assembler symbol, and one which is the
\ equivalent Forth symbol.  For example, `oneplus 1+`.
: 2header ( n[caddr u] s[caddr u] -- )
  2over 2over xname 2drop
  cr ." enter_" type ." :" cr ;

: call ( caddr u -- )
  xquery ."   jsr enter_" type cr ;

\ Attempt to parse another name/lexeme from the current input.
\ If not possible, try to refill the input buffer and try again.
\ If *that* doesn't work, then just return end of input via a
\ 0-length string.
: lexeme ( -- caddr u )
  name dup if exit then
  2drop refill if name exit then
  0 0 ;

create template
  0 c, 'c c, 'm c, 'p c, ': c,
  32 allot

: -compiler ( caddr u -- caddr u [if not a compiler word] [otherwise] )
  2dup 32 min template 5 + swap move
  dup 32 min 4 + template c!
  \ R> drops the return address for token.
  template find if nip nip r> drop execute else drop then ;

: token ( caddr u -- )
  -compiler call ;

: T] ( -- )
  begin lexeme dup if token then again ;

\ Target colon compiler.  Keep compiling tokens until
\ `;` or some similar immediate word breaks the loop.
\ See also: cmp:;
\ 
\ `T:` is used for defining words whose names are valid assembler symbols.
\ `TX:` is used for defining words whose names are invalid assembler symbols.
\ Examples: `T: foo` or `TX: oneplus 1+`.
: T:
  name check header T] ;
 
: TX:
  name check name check 2header T] ;

: cmp:exit
  ."   rts" cr ;

: cmp:;
  \ First r> drops return address for -compiler.
  \ Second r> drops return address for T:, thus ending
  \ the colon interpreter.
  cmp:exit r> r> 2drop ;



: cmp:begin ;
: cmp:again ;
: cmp:until ;
: cmp:repeat ;
: cmp:while ;
: cmp:if ;
: cmp:else ;
: cmp:then ;

: cmp:S"   34 parse 2drop ;

