# Safe rejection of invalid multibyte MWM command lines

When workspace-session support parses a command line supplied as an in-memory
string, it now rejects a line whose first multibyte character is invalid before
entering the byte-copy loop.  Previously the parser marked its output pointer
as unavailable but could still enter that loop after encountering a later valid
byte sequence, which could dereference the null sentinel.

The parser continues to report the line as unavailable and preserves the
existing handling for valid multibyte input.  This only changes malformed input
handling; it does not reinterpret valid session commands or configuration
syntax.

`tests/automated/check-mwm-invalid-multibyte-line.py` protects the control-flow
contract: the invalid-initial-sequence guard must enclose the copy loop.
