# Representation-type name case conversion

When representation type names are formatted with the `Xm` prefix, Motif
uppercases the remaining label.  The case-conversion loop now casts each byte
to `unsigned char` before calling the C ctype functions, avoiding undefined
behavior for locale strings containing bytes above ASCII.
