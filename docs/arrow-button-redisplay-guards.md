---
nav_exclude: true
---
# ArrowButton redisplay guards

`XmArrowButton` can receive an expose event while its resources are being
replaced or after a resource allocation has failed.  Redisplay now checks the
shadow and arrow GCs before passing them to the X drawing helpers.  The arrow
geometry helper also returns for zero-sized bounds, which avoids issuing
invalid drawing requests when highlight and shadow margins consume the whole
widget.

Classic rendering remains unchanged when the required resources and
geometry are available.  The superclass expose handler is still invoked so
standard Xt event processing continues for partially initialized widgets.

## Modern arrow bounds

Modern light and dark profiles center the arrow within the content rectangle,
after excluding the control's focus and shadow margins. The glyph is at most
18 logical pixels in comfortable density or 16 in compact density, with at
least four logical pixels of surrounding padding. Small allocations shrink
the glyph; allocations with no remaining content draw no arrow. Display scaling
is applied once. Widget and gadget variants use the same geometry rule.

Modern Forms and BulletinBoards default to no shadow: layout containers should
not introduce a bevel around their children. Explicit shadow resources still
take precedence. Classic and high-contrast profiles retain their own defaults.

`tests/automated/run-arrow-bounds.py` runs an installed-library regression that
reads the actual X pixels for all four directions, both widget types, light and
dark profiles, both densities, and 1×, 1.5×, 2× and 3× scales. It checks enabled
and disabled controls, tiny allocations, and default/explicit container shadows.
