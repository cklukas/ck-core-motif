---
nav_exclude: true
---
# ScrolledWindow scrollbar resize requests

When a scrollbar changes size, ScrolledWindow derives the parent request by
removing the old scrollbar extent and adding the new one.  Those operands are
Motif dimensions (unsigned), so a scrollbar larger than a collapsed window
could previously wrap the parent request to a huge size.  The calculation now
uses signed intermediates and clamps a negative result to zero before sending
the request upstream.

The source-level regression is
`tests/automated/check-scrolledwindow-scrollbar-request-bounds.py`.
