# MWM application context

MWM now checks the Xt application-context allocation before registering
converters or opening the display.  If the context cannot be created, startup
stops through the existing fatal-error path instead of passing a null context
to Xt initialization APIs.  Normal startup is unchanged.
