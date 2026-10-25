#!/usr/bin/env python3
"""Check that the internal charset comparison helper accepts NULL safely."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c"
text = source.read_text(encoding="utf-8")
needle = """_XmStringIsCurrentCharset( XmStringCharSet c )
{
  /* Charset tags may come from optional application metadata.  Treat a
     missing tag as a non-match instead of passing NULL to strcmp(). */
  if (c == NULL)
    return False;
  return (strcmp(c, _XmStringGetCurrentCharset()) == 0);
}"""
if needle not in text:
    raise SystemExit("charset null guard is missing")

print("XmString charset comparison null guard present")
