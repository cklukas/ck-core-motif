# Render-table token classification

The render-table lexer skips whitespace and recognizes alphabetic tokens with
C ctype functions.  These calls now receive unsigned byte values, preventing
undefined behavior when a malformed or non-ASCII render-table string is
parsed.
