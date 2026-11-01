# MWM menu accelerator button bounds

MWM associates menu accelerator specifications with menu-button entries while
deciding whether the matched action should be sent to the toolkit.  A partially
constructed or rebuilt menu can have an empty button table, or an accelerator
list that no longer has a matching slot.

The association walk now consumes an explicit remaining-slot count.  It does
not form `menuButtons - 1`, and it cannot decrement beyond the first table
element.  Ordinary accelerator dispatch remains unchanged; the sensitivity
lookup is simply skipped when no corresponding button exists.

`tests/automated/check-mwm-menu-accelerator-bounds.py` checks this source-level
memory-safety contract.  The change preserves MWM's public menu and accelerator
resource syntax.
