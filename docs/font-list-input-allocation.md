---
nav_exclude: true
---
# Font-list input-copy allocation

Font-list conversion parses a mutable copy of the resource string. The normal
converter and default-render-table fallback now check that copy before passing
it to the parser. Allocation failure returns safely while preserving existing
font-list ownership and successful parsing behavior.
