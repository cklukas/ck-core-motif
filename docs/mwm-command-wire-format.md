# MWM client-command wire format

MWM's Define, Include, Remove, Enable, Disable, and Rename command-selection
targets use a packed byte stream of CARD values and length-prefixed strings.
They now reject a null payload or a selection request whose format is not
8 bits before the payload reaches the command parser.  Existing correctly
formed clients continue to use format 8 unchanged; malformed 16- or 32-bit
requests fail conversion rather than being interpreted as byte data.

Client-message atom values are likewise range-checked before crossing Xlib's
signed `data.l` boundary, and a negative public message type is rejected.

The focused source contract is covered by
`tests/automated/check-mwm-command-wire-format.py`.
