# Charset conversion state inputs

The compound-text charset helpers now reject a missing charset tag or output
state before comparing tags or appending converted data. This protects both
the narrow and UTF-8 conversion paths from malformed XmString component
streams while preserving valid conversion behavior.

Regression coverage: `tests/automated/check-charset-conversion-state.py`.
