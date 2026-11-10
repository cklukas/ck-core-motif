# Warning message templates

Motif warnings are built from a template: the default text in the source, or
the text a message catalog or the Xt error database supplies for it. A user can
replace both, so a template is text to expand, never a format string to pass to
`snprintf`. Earlier, `MotifWarningHandler` (which prints every `XmeWarning` and
`_XmWarningMsg` message) and the DataField and drop-site diagnostics passed such
templates to `snprintf` together with the warning's parameters, so a template
containing `%n` or more conversions than parameters could write to or read
from arbitrary addresses.

`_XmExpandMessage` (declared in `XmI.h`) now expands the templates:

- every conversion, that is `%` with any flags, width, precision and length
  modifiers up to its conversion letter, is replaced by the next string
  parameter; a conversion without a parameter expands to nothing;
- `%%` is a literal `%`, and a `%` that never reaches a conversion letter is
  ordinary text;
- the output is cut to the buffer and always terminated, and the function
  returns the length it needs so that a first call with no buffer can size the
  second.

Because every parameter is expanded as a string, callers pass strings only. The
representation type check, which used to pass the offending number as a
pointer value, now formats it as text, and the DataField character warning
shows the character or its octal code. A message without parameters, such as
the one `XmeWarning` takes, is still shown unchanged.

## Catalog messages

Message catalogs are optional (`--enable-message-catalog`). Where they are
enabled, `_XmCatgets` and `_MrmCatgets` now pass the text of the catalog through
`_XmMessageOrDefault` together with the built-in text. The catalog text is used
only if it asks for exactly the arguments of the built-in text: the same number
of conversions, each with the same length modifier and conversion letter, no `*`
or positional argument, no `n` conversion and no field wider than three digits.
Otherwise the built-in text is used. This covers the messages that Motif and
the resource manager pass to `snprintf` with their own arguments.

## Validation

`tests/automated/run-message-templates.py PREFIX RESULTS` compiles
`tests/automated/check-message-templates.c` against an installed toolkit built
with `--enable-sanitizers` and runs it on a virtual X display. It checks the
expansion rules directly, and runs hostile templates (`%n`, `%s` beyond the
parameters, huge widths, `*`, positional conversions) through
`_XmWarningMsg`, `XmeWarning`, the representation type check and the
DataField value check. The toolkit before this change writes through the
parameter and fails.
