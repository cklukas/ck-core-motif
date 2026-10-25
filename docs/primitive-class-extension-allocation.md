---
nav_exclude: true
---
# Primitive class-extension allocation

`XmPrimitive` class initialization creates a private class-extension record
when a derived widget does not provide one.  The initializer now checks the
`XtCalloc` result and returns without dereferencing a missing extension.  This
keeps class registration fail-closed under allocation pressure while
preserving the existing extension inheritance and public widget ABI when the
allocation succeeds.

The guard applies to the shared Primitive class path, so all widgets derived
from `XmPrimitive` receive the same failure behavior.
