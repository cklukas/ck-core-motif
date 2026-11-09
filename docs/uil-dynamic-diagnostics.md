---
nav_exclude: true
---

# UIL diagnostic text

UIL diagnostics no longer silently truncate formatted messages, source
locations, listing entries, or resource-manager context at the former
132-character temporary-buffer limit.  The compiler measures and allocates
each message safely, then retains the existing diagnostic callback and listing
contracts.

Source-column carets are clamped to the retrieved source line, so an invalid
column cannot overrun the diagnostic pointer buffer.
