---
nav_exclude: true
---
# Container disclosure state

Container outline mode creates a small synthetic push-button gadget beside
each expandable row.  The gadget's constraint record stores the link back to
the row; the row's constraint record owns `outline_state`.

`ChangeOutlineButtons` now follows that link before selecting the expanded or
collapsed pixmap.  Refreshes therefore preserve the visible disclosure state
instead of silently repainting every button as collapsed.  The refresh path
also skips detached or incomplete synthetic gadgets safely.

`tests/automated/check-container-disclosure-state.py` protects the association
lookup and both constraint guards.
