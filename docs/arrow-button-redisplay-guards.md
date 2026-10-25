# ArrowButton redisplay guards

`XmArrowButton` can receive an expose event while its resources are being
replaced or after a resource allocation has failed.  Redisplay now checks the
shadow and arrow GCs before passing them to the X drawing helpers.  The arrow
geometry helper also returns for zero-sized bounds, which avoids issuing
invalid drawing requests when highlight and shadow margins consume the whole
widget.

The normal expose path remains unchanged when the required resources and
geometry are available.  The superclass expose handler is still invoked so
standard Xt event processing continues for partially initialized widgets.
