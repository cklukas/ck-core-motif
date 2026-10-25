# Virtual-key invalid-keycode guard

`XmTranslateKey` is an Xt key translator and can be called for a keycode that
is outside the server's current keymap range.  The virtual-key path now checks
the `XtGetKeysymTable` result and the display's `[min,max]` keycode range before
indexing the table.  Invalid input leaves the result produced by Xt unchanged;
it does not read before or after the keymap.

`tests/automated/run-virtkeys-invalid-regression.py` compiles the installed
consumer with `-Wall -Wextra -Werror`, calls both invalid boundary values on a
private Xvfb server, and relies on ASan/UBSan builds to catch an out-of-range
read.  The fixture does not redefine virtual-key bindings or claim behavior for
valid keyboard layouts.
