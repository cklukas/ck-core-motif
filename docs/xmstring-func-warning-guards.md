# XmStringFunc warning-backed guards

`XmParseMappingGetValues` receives an Xt `ArgList`, whose `value` field is an
integer `XtArgVal`, not a pointer. Comparing it with `NULL` produced a
portable compiler warning; zero is the correct empty-value check.

`MakeStr` also had an unbraced null-result check that made the next component
fetch appear guarded when it was not. The check is now braced and the fetch is
explicitly sequenced after successful concatenation. Public parse and string
APIs are unchanged.
