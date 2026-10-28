# UIL lexer buffer allocation failures

The lexer allocates a comment accumulator during initialization and grows a
linked lexical-buffer chain as tokens exceed the fixed buffer size.  Both
paths now check `XtMalloc` results before dereferencing or linking the
buffers.  Initial-buffer failure also releases the comment buffer; later
growth failure reports through Xt and returns `NULL` rather than dereferencing
an unallocated buffer.  Every growth call in `yylex` checks that result and
returns the lexer EOF code, so a returning Xt error handler cannot turn the
allocation failure into a null-buffer write.
