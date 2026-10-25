# Render-table export entry guards

Property export now rejects a null rendition, null tag, or missing font name
when a font resource must be serialized.  It frees the temporary buffer and
returns zero, with the output pointer reset to null, rather than dereferencing
malformed state or passing a null string to the formatter.
