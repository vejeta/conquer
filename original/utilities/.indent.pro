-pmt						/* Preserve make times */

--line-length96					/* Overall width of code */
--comment-line-length120			/* width of comment lines */
--indent-level4					/* indent N spaces per level */
--case-indentation4				/* indent case statements */
--parameter-indentation4			/* indent old-style parameter declarations */
--declaration-indentation40			/* variable declarations line up here */
--tab-size8					/* tabs get expanded into N characters */

--break-before-boolean-operator			/* break before boolean operators if possible */
--dont-break-procedure-type			/* keep function type on the same line as the name */
--blank-lines-after-declarations		/* space out declarations */
--blank-lines-after-procedures			/* space out bodies */
--blank-lines-after-commas			/* seperate lines for comma seperated declarations */
--swallow-optional-blank-lines			/* remove "extra" blank lines */

--braces-on-if-line				/* place opening brace ON the if line */
--cuddle-else					/* put else with braces ala } else { */
--cuddle-do-while				/* put while inline ala do { } while */
--braces-on-struct-decl-line			/* put opening brace ON struct declaration line */
--continue-at-parentheses			/* line up continuations at parens */

--no-space-after-function-call-names		/* function() not function () */
--dont-space-special-semicolon			/* no extra spacing of semicolons */
--no-space-after-casts				/* (type)cast not (type) cast */

--comment-indentation64				/* line up comments at column N */
--declaration-comment-column64			/* line up comments after declarations in column N */
--else-endif-column64				/* preprocessor #else and #endif comments line up here */
--start-left-side-of-comments			/* add stars in block comments */
--format-all-comments				/* try to reformat comments */
--dont-format-first-column-comments		/* unless they are on the left! */
--comment-delimiters-on-blank-lines		/* one lineers with no code should be made larger */

/* Various typedefs used in our code */

-T L_DATA
-T L_PTR
-T defn
-T classrec
-T bitmap
