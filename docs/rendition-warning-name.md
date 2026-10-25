---
nav_exclude: true
---

# Rendition warning name

Some internal rendition construction paths intentionally pass a null tag and
let Motif substitute the default string tag.  If validation then emitted an
Xt warning, the old code passed that null tag as Xt's warning *name*.  Xt
expects the name to be non-null and could crash while formatting the warning,
which made ordinary List and other render-table initialization unsafe.

Warnings now use `XmRendition` as the Xt name when the supplied tag is null;
the warning type, class, message, and resource behavior are unchanged.  The
source-level regression is
`tests/automated/check-rendition-warning-name.py`.
