# Preserve the drag receiver site-count wire contract

The Motif drag receiver property stores `num_drop_sites` as a 16-bit protocol
field.  The publisher now refuses to emit receiver metadata when the local
drop-site count exceeds `UINT16_MAX`, instead of silently truncating the count
and producing a property whose stream cannot be consumed consistently.

Normal receivers are unchanged.  The focused source regression is
`tests/automated/check-drag-receiver-site-count.py`.
