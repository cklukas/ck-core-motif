---
nav_exclude: true
---
# List highlight geometry

List traversal highlighting derives its width and height by subtracting margin
and shadow insets. Although the outer widget dimension was cast to `int`, an
unsigned shadow thickness inside the inner expression could still force the
subtraction through unsigned arithmetic and wrap a collapsed highlight.

All highlight inset terms now participate in signed arithmetic. Existing
positive-area checks continue to suppress drawing when no highlight span fits.
