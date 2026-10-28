---
nav_exclude: true
---
# Source-contract refreshes

Two focused checks were updated to describe the current, already-landed
implementations rather than removed intermediate forms:

- AutoSuggestBox callback checks now accept the combined null-and-widget-class
  guard used by every callback path.
- Calendar text-metric checks now require the wide baseline arithmetic used
  before the bounded Position conversion.
- ColorPicker redisplay checks now require its wide text baseline arithmetic
  instead of the removed narrow expression.

No runtime behavior changed in this maintenance update.
