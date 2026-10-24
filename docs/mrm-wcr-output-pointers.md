# Mrm widget-record output pointers

The widget-record string append helpers now reject a missing context or output
offset before handling NULL and empty input strings. The shared space helper
also validates both output pointers before inspecting the resource context.
This preserves successful append behavior while preventing NULL output-pointer
crashes.

The source-level regression is `tests/automated/check-mrm-wcr-output-pointers.py`.
