# XmScrollBar API null-safety

`XmScrollBarGetValues` and `XmScrollBarSetValues` now return immediately for a
null widget before obtaining an Xt application context. `GetValues` continues to
allow optional output pointers, and valid get/set operations retain their prior
behavior.
