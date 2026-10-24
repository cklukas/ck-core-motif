# Mwm session-command growth

WSM session restoration tokenizes a saved command into an array of argument
pointers.  The legacy loop wrote the next pointer before expanding its
100-entry array, so a command with exactly 100 segments wrote past the
allocation; allocation failures were also unchecked.

The parser now grows the array before insertion and aborts cleanly on either
initial or subsequent allocation failure.  Command ordering and restored
argument content remain unchanged.

`tests/automated/check-mwm-session-command-growth.py` protects this contract.
