# Color-name character classification

Color database names are normalized by filtering spaces and changing case.
The normalization and lookup paths now pass unsigned byte values to all C
ctype functions, so non-ASCII bytes in a database entry cannot trigger
undefined behavior.
