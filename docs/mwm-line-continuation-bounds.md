# MWM configuration line-continuation bounds

The non-multibyte configuration reader recognizes a continued line by looking
for a backslash before the line ending.  It now checks that the line contains
at least two characters before subtracting two from its length.  Empty and
one-character lines therefore remain ordinary input instead of turning a
`size_t` underflow into an out-of-bounds buffer access.
