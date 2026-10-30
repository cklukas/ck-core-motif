# SearchBox clear hit rectangle

Pointer activation of the SearchBox clear control now uses a centered 20×20
hit rectangle, matching the compact clear glyph instead of treating the entire
right-hand strip at every vertical coordinate as clickable. Escape and the
programmatic clear API retain their existing behavior.
