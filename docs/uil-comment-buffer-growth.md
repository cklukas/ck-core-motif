# UIL comment-buffer growth

The UIL lexer preserves adjacent source comments in a dynamically grown
buffer. Growth now uses a temporary pointer and updates the global buffer only
after the Xt allocation succeeds. If allocation fails, the lexer reports the
failure and returns without exposing a null or partially updated buffer to the
comment concatenation path.
