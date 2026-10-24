# DataField multibyte conversion scratch space

DataField validates and converts wide characters with `wctomb`.  That
function writes up to the locale's `MB_LEN_MAX` bytes, but three conversion
paths used fixed eight-byte scratch arrays.  Locales with a larger maximum
could therefore overwrite the stack during input validation or warning
generation.

Those arrays now use `MB_LEN_MAX` from `<limits.h>`.  Normal locale behavior
is unchanged, while conversion remains safe for every supported locale.  The
source regression in `tests/automated/check-datafield-multibyte-scratch.py`
keeps all three paths correctly sized.
