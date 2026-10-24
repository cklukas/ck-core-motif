# Unterminated render-table strings

The render-table lexer now distinguishes a closing quote from the terminating
NUL.  A string without a closing quote consumes through the NUL and leaves the
cursor at the end of input instead of advancing one byte beyond it.
