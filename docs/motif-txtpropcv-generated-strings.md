---
nav_exclude: true
---
# Plain text-property XmString generation failures

When converting locale, ISO-8859-1, or UTF-8 text properties,
`XmStringGenerate` failures are now detected for both the first and subsequent
items.  Already generated entries and the table are released, application
locking is balanced, and the conversion returns `BadAlloc` instead of a
successful table containing null entries.

The regression check is
`tests/automated/check-txtpropcv-generated-strings.py`.
