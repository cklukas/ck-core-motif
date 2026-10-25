---
nav_exclude: true
---

# WSM configuration-string failure

Configuration-format replies contain one allocated string per attribute.
When decoding those strings, a failed allocation now aborts the current
configuration update and clears the partially built screen information before
returning.  This pairs with `UnpackString`'s null result and prevents
`XrmStringToQuark` from receiving an invalid pointer or leaving a partial
format visible to later requests.
