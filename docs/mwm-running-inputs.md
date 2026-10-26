# MWM-running query input contract

`XmIsMotifWMRunning` checks the Motif window-manager property for a shell's
screen.  A null shell cannot identify a screen or display; the query now
returns `False` before `XtScreen` or application-context lookup.  Valid shell
queries are unchanged.
