---
nav_exclude: true
---
# XmString compact-layout safety

An optimized single-segment `XmString` stores its text directly after a
compact header.  The internal optimized-entry representation can place its
`data` union at a different, padded offset on some ABIs.  Treating the compact
allocation as an entry and reading `_XmEntryTextGet` can therefore inspect
bytes that were never allocated as an `XtPointer`.

Concatenation and underline matching now compare the compile-time layouts
before using the entry representation.  When padding differs, the existing
copy-to-an-entry path is used; when the layouts match, the zero-copy path is
retained.  This preserves rendering and ownership behavior while making
64-bit and other padded layouts memory-safe.

The source-contract regression is
`tests/automated/check-xmstring-compact-layout.py`.

The integrated Xft/SVG Periodic native fixture also exercises real menu mnemonic
rendering, notebook switching, resizing and widget teardown under Valgrind.
The original compact probe produced an eight-byte invalid read; the corrected
build finishes with zero invalid-access errors (leak and undefined-value checks
disabled). This native result supplements the source-contract regression.
