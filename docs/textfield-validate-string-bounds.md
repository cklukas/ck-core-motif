# TextField validation bounds

`XmTextField` validation accepts both locale byte strings and wide-character
strings.  The validator now checks the byte-string length before narrowing
`strlen()` to its legacy signed length type, and checks the wide-character
count before its counter can wrap.

Overly large values are rejected without allocation or mutation.  Ordinary
strings retain the existing validation and callback behavior.
