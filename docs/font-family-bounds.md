# Font selector family fallback bounds

When an XLFD family is wildcarded, `XmFontSelector` obtains a replacement
family string from an `XmString`. That value can be longer than the local
`BUFSIZ` scratch buffer, so the fallback now copies at most `BUFSIZ - 1` bytes
and explicitly terminates the result before quark lookup.

The focused source check is
`tests/automated/check-font-family-bounds.py`.
