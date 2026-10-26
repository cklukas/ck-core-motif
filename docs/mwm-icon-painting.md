# MWM icon painting preconditions

Active and inactive icon painting can be triggered while a client is being
withdrawn. Both entry points now require valid client/screen state and a
realized icon frame before reading appearance data or issuing X drawing
requests. Normal icon repainting is unchanged.
