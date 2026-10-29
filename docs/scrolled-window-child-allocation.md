---
nav_exclude: true
---
# ScrolledWindow child allocation

`XmScrolledWindow` creates an internal clip window and scrollbars during
initialization. The scroll-frame trait is now invoked only when the clip child
and trait callback exist. If the clip child cannot be created, resize and
automatic-layout paths return safely, and slider callbacks ignore a missing
closure instead of dereferencing it. Normal clip and scrollbar construction is
unchanged.

The contract is protected by
`tests/automated/check-scrolled-child-allocation.py`.
