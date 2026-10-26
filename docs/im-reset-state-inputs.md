# XIC reset-state query input contract

`XmImGetXICResetState` now returns the default `XIMInitialState` when passed
a null widget, before looking up input-method registration state.  Queries for
valid widgets retain their existing XIC reset-state behavior.
