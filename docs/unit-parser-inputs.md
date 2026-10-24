# Unit-string parser inputs

The internal unit parser now validates its specification and output pointers
before scanning the string.  A malformed resource conversion returns
`XmPARSE_ERROR` instead of dereferencing a null pointer.
