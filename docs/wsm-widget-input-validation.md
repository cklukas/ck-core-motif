# WSM widget input validation

`WSMSendMessage` and `WSMDefaultOwnSelection` now reject null widget inputs
before calling Xt or querying a display.  `WSMSendMessage` also rejects a null
request.  Existing realized-widget and selection checks remain in place, and
valid callers retain the same protocol behavior.
