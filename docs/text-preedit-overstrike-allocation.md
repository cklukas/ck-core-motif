# Text preedit overstrike allocation

The overstrike preedit path now checks the character-count multiplication and
the resulting allocation before converting the recovery text.  An invalid or
unavailable buffer resets the input-method preedit state and returns cleanly
instead of writing through a null or undersized buffer.

The focused source contract is checked by
`tests/automated/check-text-preedit-overstrike-allocation.py`.
