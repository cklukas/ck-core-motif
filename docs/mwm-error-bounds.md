# Mwm error-diagnostic bounds

The window manager's WSM warning wrapper and X I/O error handler receive
arbitrary display, locale, and diagnostic text.  They previously formatted
those values into fixed 1024-byte and 100-byte stack buffers, respectively.

Both paths now measure the complete message, allocate it with Xt memory, and
release it after dispatch.  Warning and fatal-exit behavior remains unchanged;
allocation failure emits a short fallback diagnostic instead of overflowing a
stack buffer.

`tests/automated/check-mwm-error-bounds.py` protects this contract.
