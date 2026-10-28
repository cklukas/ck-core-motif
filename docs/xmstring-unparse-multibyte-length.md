# XmString unparse multibyte lengths

The unparse path now checks the `mblen()` result for a multibyte parse pattern
before passing it to the component renderer.  Invalid sequences are omitted
from that substitution instead of converting `-1` into an oversized unsigned
length.
