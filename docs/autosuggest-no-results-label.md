---
nav_exclude: true
---
# AutoSuggestBox no-results label replacement

`XmNautoSuggestNoResultsLabel` is an owned AutoSuggestBox string.  A resource
update now copies its requested text and, when the no-results popup is visible,
constructs the replacement compound string before retiring the current label.
If either allocation fails, the original resource and list row stay intact.

An initial string-copy failure leaves the owned field empty but retains the
documented `"No suggestions"` fallback for later no-results presentation.
This preserves normal resource semantics and avoids borrowing application
storage after `XtSetValues` returns.
