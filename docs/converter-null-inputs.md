---
nav_exclude: true
---
# Resource converter null inputs

Button-type, keysym-table, and charset-table resource converters now reject a
missing source string before scanning or copying it.  This keeps malformed
`XrmValue` input on the normal conversion-failure path instead of crashing
while dereferencing a null address.
