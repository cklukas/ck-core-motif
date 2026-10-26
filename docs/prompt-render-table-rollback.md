---
nav_exclude: true
---
# XmPrompt render-table rollback

When a prompt receives a new font list, it now releases the old render table
only after `XmFontListCopy` succeeds. A failed replacement keeps the previous
valid table and does not request a font-driven redraw.

The focused source contract is checked by
`tests/automated/check-prompt-render-table-rollback.py`.
