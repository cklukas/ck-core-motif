# MWM display-open failure

After `XtOpenDisplay` fails, MWM now returns immediately after its existing
fatal-error call.  This keeps later Xlib and Xt initialization from receiving a
null display if an embedding or test environment allows the error routine to
return.  Successful display initialization is unchanged.
