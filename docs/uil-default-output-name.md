---
nav_exclude: true
---

# UIL default output filename

When no `-o` option is supplied, UIL uses the conventional `a.uid` output
filename. That fixed default is now held in internal static storage rather than
being heap allocated and copied during command decoding. Normal command-line
behavior is unchanged, while allocation pressure can no longer turn the
default-output path into a null-pointer copy.

The focused regression is
`tests/automated/check-uil-default-output-name.py`.
