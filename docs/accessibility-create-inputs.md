---
nav_exclude: true
---

# Accessibility creation input guards

`XmAccessibilityCreate` now rejects a NULL root before asking Xt for an
application context, and returns cleanly when allocating the accessibility
state fails.

Valid roots and the public ABI are unchanged.  The static regression check is
`tests/automated/check-accessibility-create-inputs.py`.

Display-hook registration also rejects a missing accessibility context or
non-object widget before querying Xt display state.
