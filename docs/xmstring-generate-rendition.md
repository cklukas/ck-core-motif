---
nav_exclude: true
---
# XmStringGenerate rendition cache failures

Rendition decoration is now transactional. If the requested rendition cannot
be resolved in the tag cache, `XmStringGenerate` frees the generated result
and returns `NULL` instead of attaching a null rendition or silently changing
the output.

The regression check is
`tests/automated/check-xmstring-generate-rendition.py`.
