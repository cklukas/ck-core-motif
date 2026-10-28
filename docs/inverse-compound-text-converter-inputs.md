# Inverse compound-text converter inputs

The inverse compound-text conversion path now rejects missing `XrmValue`
records before initializing output storage. This covers the public
`XmCvtXmStringToText` converter and the shared text/UTF-8 helpers used by
internal wrappers. Valid XmString conversions retain their existing output and
allocation behavior.

Regression coverage: `tests/automated/check-inverse-compound-text-inputs.py`.
