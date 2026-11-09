# SearchBox clear hit rectangle

Pointer activation of the SearchBox clear control now uses a centered 20×20
hit rectangle, matching the compact clear glyph instead of treating the entire
right-hand strip at every vertical coordinate as clickable. Escape and the
programmatic clear API retain their existing behavior.  The rectangle's X11
origins are also checked before narrowing to signed 16-bit protocol fields;
an unrepresentable very-wide placement is omitted rather than wrapping the
icon and its hit area to the opposite edge.
