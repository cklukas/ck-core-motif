# MWM appearance resource widget

Appearance initialization creates a temporary `client` widget so Motif can
resolve `client.title` resources.  If that widget cannot be allocated, MWM
now reports the startup failure and exits before querying or destroying a null
widget.  Successful resource resolution is unchanged.
