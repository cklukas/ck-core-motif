# Accessibility key-symbol wire values

AT-SPI device-event notifications carry the key identity in a signed 32-bit
field, while X11 `KeySym` values are unsigned.  The accessibility D-Bus
adapter now copies the 32-bit key-symbol representation into that field
explicitly, preserving high-valued and Unicode keysyms without relying on an
implementation-defined signed cast.

The wire value for ordinary keysyms is unchanged.
