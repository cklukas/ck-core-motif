# MWM icon geometry type boundaries

MWM configuration resources store icon dimensions as signed values, while Xlib
pixmap geometry and copy dimensions are unsigned.  Icon composition now
validates the signed resources before conversion, uses explicit unsigned
dimensions for pixmap operations, rejects source offsets that cannot fit Xlib's
signed coordinate arguments, and validates the display depth before converting
it for pixmap creation.  This preserves normal icon sizing while making
malformed or extreme geometry fail safely.

The focused source contract is covered by
`tests/automated/check-mwm-icon-dimension-types.py`.
