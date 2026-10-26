# MWM icon-title painting

Icon title exposure and redisplay callbacks may run after a client has started
teardown. Both title-drawing entry points now require valid client/screen state
and a realized icon frame before reading appearance data or painting text.
Normal title repainting is unchanged.
