---
nav_exclude: true
---
# XmStringGenerate parse failure

`XmStringGenerate` now returns immediately when `XmStringParseText` fails,
before inspecting or decorating the result with a rendition. This prevents a
null representation dereference while preserving successful generation.

The regression check is
`tests/automated/check-xmstring-generate-result.py`.
