---
nav_exclude: true
---
# EditRes diagnostic bounds

EditRes reports conversion and protocol errors through fixed stack storage when
messages are short, and allocates larger storage for long messages.  The
`noColormap` path previously estimated the raw format string and then used
`sprintf`, so width or precision expansions could exceed the estimate.

The error reporter now measures the rendered `noColormap` message and uses
bounded `snprintf` calls for every diagnostic variant.  Normal text and the
EditRes protocol remain unchanged; allocation failure still produces the
existing short fallback message.

`tests/automated/check-editres-error-bounds.py` protects this contract.
