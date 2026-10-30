# Xpm fallback case-insensitive comparison

The optional fallback `xpmstrcasecmp` now converts input bytes to
`unsigned char` before calling `tolower` and compares the terminating bytes
without signed-char arithmetic. Non-ASCII byte values therefore no longer
trigger undefined behavior or platform-dependent ordering.
