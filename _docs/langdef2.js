
expression:

// class defn must be global scope and func only glob and class scope
random literals in src code not allowed
ex: 12; or "random string for comment";

statement :
 0. import expr    : import "path/str" ;
 1. initialize     : dtype idf [ = expr] ;  // done
 2. class var init : [static] dtype idf [ = expr ] ;
 3. assignment     : idf [=, +=, -=, *=, ...] expr ; // not == < > <= >= and or ...
 4. function call  : idf_func ( expr, ... ) [ . idf_meth ( expr, ... ) ... ] ;
 5. method call    : idf . idf_meth ( expr, ... ) [ . dif_meth ( expr, ... ) ... ] ;

 6. if statement   : if ( expr_bool ) { statement; ... } [ else if ( expr_bool ) { statement; ... } ... [ else { statement; ... } ] ]
 7. while loop     : while ( expr_bool ) { statement; ... }
 8. for loop       : for ( [expr]; [expr]; [expr] ) { statement; ... }
 9. for each loop  : foreach ( expr_initialize ; idf_iter ) { statement; ... }
10. func defn      : function idf_func ( dtype idf, dtype idf, ... ) [: dtype] { statement; statement; ... } // handle if not returned 
11. method defn    : [static] function idf_func( ...
12. class defn     : class idf_class [( idf_class_parent )] { statement; ... }

// everything is a statement (some with semi collon) and has expressions;
// expression ends with ; or , - func defn or ) func defn

statement 			 	: if while for function class static(?)
if_statement_continue?  : else if, else  import, char, short, int, float, double, long, satatic(?), idf_registered in globals or locals
( // ex:
	keyword 			 : import for while if static ...
	dtype   			 : int string char bool ...
	idf/built/func/names : func_name, isnst_name, imported_name, ...
)
):
// procedure
create a global map for variables, functions, class defn and local for variables
scane expr or statement // recursive - expr only contain expr, statement can contain both
in expr if . operator comes find number of args for that, join ., method, number as a single token/ast - need for infix

the __getitem__ ( list[index] ) -> [ is .getitme + '(' with 1 args, and ] is close brakcet ')'

precedance min to max:

( func .methods
== != >= <= < > 
or and
! not
+ (-) binary minus
<< >> & | ^
* /
%
**
(-) single operator

consider minus - as single before minus:
	LPRAN OPERATOR nothing-minus-is-first comma [ binary-operator(and not ...) assignment(+= *= ...)
consider minus - as binary op:
	RPRAN IDENTIFIER number ] 

// tree maker : statement start with
dtype : initialize statement


