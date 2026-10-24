# Text inline preedit recovery bounds

Text widget overstrike preedit recovery now validates every multibyte step
before advancing through saved text or appending recovered bytes.  Invalid
locale sequences stop recovery safely instead of moving the scan pointer
backward or producing an invalid append length.  Valid composition behavior,
callbacks, and public widget interfaces remain unchanged.

The warning-enabled Text source build is the native smoke check for this
recovery path; the XIM preedit-range regression remains available for the
shared preedit state contract.
