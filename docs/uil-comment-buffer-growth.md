---
nav_exclude: true
---
# UIL comment-buffer growth

The UIL lexer preserves adjacent source comments in a dynamically grown
buffer. Growth now uses a temporary pointer and updates the global buffer only
after the Xt allocation succeeds. If allocation fails, the lexer reports the
failure and returns without exposing a null or partially updated buffer to the
comment concatenation path.

Object comment association applies the same failure rule: a failed symbol
comment allocation leaves the destination pointer null and clears the pending
comment, preventing a later symbol from receiving stale parser state.
