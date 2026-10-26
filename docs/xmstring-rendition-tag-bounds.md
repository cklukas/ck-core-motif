# XmString rendition-tag bounds

The XmString segment extractor copies rendition tags through an Xt allocation.
Both extraction paths now reject a tag count whose byte size exceeds either
`size_t` or Xt's `Cardinal` range before invoking the copy helper.  This keeps
the public extraction behavior unchanged while preventing a narrowed size on
large or malformed compound strings.
