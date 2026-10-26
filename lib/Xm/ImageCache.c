/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: ImageCache.c /main/44 1998/10/06 17:26:25 samborn $"
#endif
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "XmI.h"
#include "XmosI.h"              /* for mask name API and stuff */
#include "BitmapsI.h"		/* for built-in images */
#include "ColorI.h"		/* for _XmSearchColorCache() */
#include "HashI.h"		/* for hash table routines. */
#include "ImageCachI.h"		/* for DIRECT_PIXMAP_CACHED */
#include "ReadImageI.h"		/* for read xbm stuff */
#include <Xm/AccColorT.h>       /* for new _XmGetColoredPixmap API */
#include <Xm/ColorObjP.h>       /* for Xme Color Obj access API */
#include <Xm/IconFile.h>        /* XmGetIconFileName */
#ifdef PRINTING_SUPPORTED
#include <Xm/PrintSP.h>         /* for pixmap resolution */
#endif
#include <Xm/XpmP.h>
#include <X11/Xresource.h>
#include <math.h>
#include <stdint.h>
#ifdef JPEG_SUPPORTED
#include "JpegI.h"
#endif
#ifdef PNG_SUPPORTED
#include "PngI.h"
#endif
#ifdef SVG_SUPPORTED
#include "SvgI.h"
#endif

/* additional value for GetImage return, FALSE, TRUE and */
#define NOT_CACHED 2

/* index and max size of the override_colorsarray */
#define NUM_SYMBOLIC_COLORS       7
#define SYMB_BACKGROUND           0
#define SYMB_FOREGROUND           1
#define TRANSPARENT_COLOR         2
#define SYMB_TOP_SHADOW_COLOR     3
#define SYMB_BOTTOM_SHADOW_COLOR  4
#define SYMB_SELECT_COLOR         5
#define SYMB_HIGHLIGHT_COLOR      6

/*  Image set handling defines, structure, and global statics  */

typedef struct _ImageData
{
   int		 hot_x, hot_y;
   XImage	 *image;
   char		 *image_name;
   unsigned char *builtin_data;
} ImageData;

static XmHashTable image_set = NULL;


/*  Pixmap caching structure and global statics  */

typedef struct _PixmapData
{
   Screen * screen;
   char   * image_name;
   XmAccessColorData acc_color ;
   Pixmap   pixmap;
   int	    depth;
   Dimension width ;
   Dimension height ;
   int      reference_count;
   unsigned short print_resolution ;
   Widget   print_shell ;
   double   scaling_ratio ;
   Pixel  * pixels;
   int      npixels;
} PixmapData;

static XmHashTable pixmap_set = NULL;
static XmHashTable pixmap_data_set = NULL;


/*  Color caching structure and global statics  */

typedef struct _CachedColorStruct {
    Display	*display;
    Colormap 	 colormap;
    XrmQuark 	 colorname;
    unsigned short red, green, blue;
    Pixel    	 pixel;
    unsigned int num_cached;	/* reference count */
} CachedColor;

typedef struct _CachedColorListStruct {
    int          numEntries;
    int          maxEntries;
    CachedColor  *cache;
} CachedColorList;

static CachedColorList colorCacheList;


/*  GC caching structure and global statics  */

typedef struct _GCData
{
    GC		gc;
    Screen     *screen;
    Widget	print_shell;
    int		depth;
    int		image_depth;
    Pixel	foreground;
    Pixel	background;
} GCData;

static XmHashTable gc_set = NULL;


typedef struct _CleanKey {
    Screen * screen ;
    Widget shell ;
} CleanKey ;

/********    Static Function Declarations    ********/

static Boolean ComparePixmaps (XmHashKey key_1, XmHashKey key_2);
static Boolean ComparePixmapDatas (XmHashKey key_1, XmHashKey key_2);
static Boolean CompareStrings (XmHashKey key_1, XmHashKey key_2);
static Boolean CompareGCDatas (XmHashKey key_1, XmHashKey key_2);
static XmHashValue HashPixmap (XmHashKey key);
static XmHashValue HashPixmapData (XmHashKey key);
static XmHashValue HashString (XmHashKey key);
static XmHashValue HashGCData (XmHashKey key);
static void InitializeImageSet( void ) ;
static void InitializePixmapSets( void ) ;
static Boolean UninstallImageMapProc (XmHashKey key, XtPointer value, 
			       XtPointer image);  
static Boolean SymbolicColorUsed(String color_name,
				 XpmColor * xpm_colors,
				 unsigned int ncolors);
static void CompleteUnspecColors(
			       Screen *screen,
			       XpmColorSymbol *override_colors);
static int GetOverrideColors(
			    Screen *screen,
			    XmAccessColorData acc_color,
			    XpmColorSymbol *override_colors);
static XtEnum GetXpmImage(Screen *screen, char *image_name, char *file_name,
                          XmAccessColorData acc_color, XImage **image,
                          unsigned short *pixmap_resolution,
                          Pixel **pixels, int *npixels);
static XtEnum GetImage(Screen *screen, char *image_name,
		       XmAccessColorData acc_color, XImage **image,
		       unsigned short * pixmap_resolution,
		       Pixel **pixels,	/* allocated pixels */
		       int *npixels);
static GC GetGCForPutImage(Screen *screen,
			   Widget print_shell,
			   XImage * image, 
			   Pixmap pixmap, 
			   int depth,
			   Pixel foreground,
			   Pixel background,
			   Boolean *caller_owns_gc);
static Boolean XmRoundScaled(double value, unsigned int *rounded);
static Boolean XmScaledDimension(unsigned int source, double ratio,
				 Dimension *dimension);
static Boolean XmImageArrayFits(size_t count, size_t element_size);
static Boolean XmMaskNameSize(String image_name, size_t *size);

#ifdef SVG_SUPPORTED
static Boolean IsSvgFile(const char *file_name);
#endif

static int GetCacheColor(Display *display, Colormap colormap,
			 char *colorname, XColor *color, void *data);
static int FreeCacheColors(Display *display, Colormap colormap,
			   Pixel *pixels, int npixels, void *data);
static void ReleaseImagePixels(Screen *screen, Pixel *pixels, int npixels);

/********    End Static Function Declarations    ********/




/*** IMAGE CACHE PART FIRST ***/




/* Compare two strings. */
static Boolean 
CompareStrings (XmHashKey key_1, 
		XmHashKey key_2)
{
  char *data_1 = (char *) key_1;
  char *data_2 = (char *) key_2;

  return ((data_1 == data_2) ||
          (data_1 != NULL && data_2 != NULL &&
           strcmp(data_1, data_2) == 0));
}


/* Hash a string. */
static XmHashValue 
HashString (XmHashKey key)
{
  char *data = (char *) key;
  size_t len;
  unsigned int folded_len;

  if (data == NULL)
    return 0;
  len = strlen(data);
  folded_len = (unsigned int)((uintmax_t)len ^ ((uintmax_t)len >> 32));
  return (XmHashValue)((((folded_len << 8) |
			      (unsigned int)(unsigned char)data[0]) << 8) |
			    (unsigned int)(unsigned char)data[len]);
}

#ifdef SVG_SUPPORTED
static Boolean
IsSvgFile(const char *file_name)
{
  size_t length;

  if (file_name == NULL)
    return False;
  length = strlen(file_name);
  if (length < 4 || file_name[length - 4] != '.')
    return False;
  return ((file_name[length - 3] == 's' || file_name[length - 3] == 'S') &&
          (file_name[length - 2] == 'v' || file_name[length - 2] == 'V') &&
          (file_name[length - 1] == 'g' || file_name[length - 1] == 'G'));
}
#endif

/************************************************************************
 *
 *  InitializeImageSet
 *	Initialize the image set.
 *
 ************************************************************************/
static void 
InitializeImageSet( void )
{
  register int i;
  XmHashTable new_image_set;
  ImageData *entries[MAX_BUILTIN_IMAGES] = { NULL };

  /* Build the table privately, then publish it only when complete. */
  _XmProcessLock();
  if (image_set != NULL)
    {
      _XmProcessUnlock();
      return;
    }

  new_image_set =
    _XmAllocHashTable (MAX_BUILTIN_IMAGES + 100, CompareStrings, HashString);
  if (new_image_set == NULL)
    {
      _XmProcessUnlock();
      return;
    }

  for (i = 0; i < MAX_BUILTIN_IMAGES ; i++)
    {
      entries[i] = XtNew(ImageData);
      if (entries[i] == NULL)
        goto allocation_failure;

      entries[i]->hot_x = 0;
      entries[i]->hot_y = 0;
      entries[i]->image = NULL;
      entries[i]->image_name = (char*) bitmap_name_set[i];
      entries[i]->builtin_data = (unsigned char *) bitmaps[i];

      if (!_XmAddHashEntry(new_image_set, entries[i]->image_name, entries[i]))
	goto allocation_failure;
    }

  image_set = new_image_set;
  _XmProcessUnlock();
  return;

allocation_failure:
  _XmProcessUnlock();
  for (i = 0; i < MAX_BUILTIN_IMAGES ; i++)
    if (entries[i] != NULL)
      XtFree((char *)entries[i]);
  _XmFreeHashTable(new_image_set);
}


/************************************************************************
 *
 *  _XmInstallImage 
 * 
 *		Allow a hot_spot to be specified
 *
 ************************************************************************/
Boolean 
_XmInstallImage(
        XImage *image,
        char *image_name,
	int hot_x,
	int hot_y)
{
   ImageData *entry;


   /*  Error checking  */

   if (image == NULL || image_name == NULL) return (False);


   /*  Check for the initial allocation of the image set array  */

   if (image_set == NULL) InitializeImageSet();
   if (image_set == NULL)
     return (False);


   /*  Verify that the image_name is not already in the image set.  */
   _XmProcessLock();

   if (_XmGetHashEntry(image_set, image_name) != NULL) {
     _XmProcessUnlock();
     return (False);
   }


   /*  Initialize the image element for the new image and return True.  */

   entry = XtNew(ImageData);
   if (entry == NULL)
     {
       _XmProcessUnlock();
       return (False);
     }
   entry->hot_x = hot_x;
   entry->hot_y = hot_y;
   entry->image = image;
   entry->image_name = XtNewString(image_name);
   if (entry->image_name == NULL)
     {
       XtFree((char *)entry);
       _XmProcessUnlock();
       return (False);
     }
   entry->builtin_data = NULL;

   if (!_XmAddHashEntry(image_set, entry->image_name, entry))
     {
       XtFree(entry->image_name);
       XtFree((char *)entry);
       _XmProcessUnlock();
       return (False);
     }

   _XmProcessUnlock();
   return (True);
}

/************************************************************************
 *
 *  XmInstallImage
 *	Add the provided image for the image set and return an
 *	id to be used for further referencing.  
 *
 ************************************************************************/
Boolean 
XmInstallImage(
        XImage *image,
        char *image_name )
{
    return _XmInstallImage(image, image_name, 0, 0);
}
/* A hash table map procedure to uninstall an image. */

/*ARGSUSED*/
static Boolean 
UninstallImageMapProc (XmHashKey key, /* unused */
		XtPointer value,
		XtPointer image)
{
  ImageData *entry = (ImageData *) value;

  if (entry->image == (XImage*) image)
    {
      /* Can't free built-in data. */
      if (entry->builtin_data == NULL)
	{
	  _XmProcessLock();
	  _XmRemoveHashEntry (image_set, entry->image_name);
	  _XmProcessUnlock();
	  XtFree(entry->image_name);
	  /* uninstall do not free the XImage itself */
	  XtFree((char*) entry);
	}

      return True;
    }

  return False;
}

/************************************************************************
 *
 *  XmUninstallImage
 *	Remove an image from the image set.
 *	Return a boolean (True) if the uninstall succeeded.  Return
 *	a boolean (False) if an error condition occurs.
 *
 ************************************************************************/
Boolean 
XmUninstallImage(
        XImage *image )
{
   Cardinal old_count;
   Boolean	ret_val;

   /*  Check for invalid conditions  */

   if ((image == NULL) || (image_set == NULL)) return (False);

   _XmProcessLock();
   /*  Since we can't index based on the image, search the hash table */
   /*  until the desired entry is found and then remove it.	      */

   old_count = _XmHashTableCount(image_set);

   _XmMapHashTable(image_set, UninstallImageMapProc, image);
   ret_val = (old_count > _XmHashTableCount(image_set));
   _XmProcessUnlock();

   return ret_val;
}



/************************************************************************
 *
 * SymbolicColorUsed
 *  Used to determine if a given color string is defined as a symbolic
 *   color in an Xpm image. This is used for caching policy where we
 *   don't want to cache based on irrelevant colors.
 *  Note that we don't look if the color is actually used because it
 *   is too expensive.
 *
 ************************************************************************/
static Boolean 
SymbolicColorUsed(String color_name,
		  XpmColor * xpm_colors,
		  unsigned int ncolors)
{
    Cardinal i;

    if (color_name == NULL || xpm_colors == NULL)
	return False;

    /* first look if the color is present as a symbolic in the 
       colorTable */
    for (i = 0; i < ncolors; i++, xpm_colors++) {
	if (xpm_colors->symbolic &&
	    !strcmp(xpm_colors->symbolic, color_name))
	    break ;
    }

    if (i == ncolors) return False ;	

    return True ;
}


/************************************************************************
 *
 *  CompleteUnspecColors
 *     Complete the unspecified_pixel values of an override_colors array 
 *      based on the values coming from the ColorObj or the color cache.
 *     Does not handle highlight.
 *     The logic here comes directly from CDE.
 *
 *  The above description does not fit with the actual logic of CDE where
 *  not only the unspecified colors are set here but the whole set of
 *  symbols! See defect CDExc17529.
 *
 ************************************************************************/
static void
CompleteUnspecColors(
		Screen *screen,
		XpmColorSymbol *override_colors)
{
    XmPixelSet 	pixelSets[XmCO_NUM_COLORS];
    int 	colorUse ;
    Boolean	bgFound = False, fgFound = False, result = False;
    Pixel	*pixelPtr;
    Cardinal i, j ;
    XmAccessColorDataRec loc_acc_color;

    if (screen == NULL || override_colors == NULL)
	return;


    /* Ask the color obj */
    if (!(result = XmeGetColorObjData(screen, 
				      &colorUse, pixelSets, XmCO_NUM_COLORS,
				      NULL, NULL, NULL, NULL, NULL)))
      i = XmCO_NUM_COLORS;
    else
      i = 0;

    /* Look for a pixels set containing the given fg and bg,
     * no matter whether they are actually the fg and bg of the set...
     */
    for ( ; i < XmCO_NUM_COLORS; i++) {
	bgFound = False;
	fgFound = False;
	pixelPtr = (Pixel *)&(pixelSets[i].fg);
	for (j = 0; j < 5; j++, pixelPtr++) {
	    if (*pixelPtr == override_colors[SYMB_BACKGROUND].pixel)
	      bgFound = True;
	    else if (*pixelPtr == override_colors[SYMB_FOREGROUND].pixel)
	      fgFound = True;
	}
	if (bgFound && fgFound)
	  break;
    }

    if (i == XmCO_NUM_COLORS) {
	/*
	 * We didn't find the bg/fg tuple in any of the Dt colorsets
	 * or if the color server is not running, we will now try
	 * the color cache. 
	 */

      XmColorData *old_colors;
      XmColorData new_colors;

      loc_acc_color.background = override_colors[SYMB_BACKGROUND].pixel ;

      new_colors.screen = screen;
      new_colors.color_map = DefaultColormapOfScreen(screen);
      new_colors.background.pixel = loc_acc_color.background ;

      /* Use motif color set only if already allocated.
       */
      if (!result && _XmSearchColorCache(
		 (XmLOOK_AT_SCREEN | XmLOOK_AT_CMAP | XmLOOK_AT_BACKGROUND),
					 &new_colors, &old_colors)) {
	  XmGetColors(screen, 
		      DefaultColormapOfScreen(screen),
		      loc_acc_color.background,
		      &loc_acc_color.foreground,
		      &loc_acc_color.top_shadow_color,
		      &loc_acc_color.bottom_shadow_color,
		      &loc_acc_color.select_color);
      } else {
	  /* Cannot generate missing colors */
	  return ;
       }
    }
    else {
	loc_acc_color.background = pixelSets[i].bg;
	loc_acc_color.foreground = pixelSets[i].fg;
	loc_acc_color.top_shadow_color = pixelSets[i].ts;
	loc_acc_color.bottom_shadow_color = pixelSets[i].bs;
	loc_acc_color.select_color = pixelSets[i].sc;
    }

    /* Now process the setting for unspecified pixel values.
       If we're here, all the colors but highlight are available */
    
    override_colors[SYMB_BACKGROUND].pixel = loc_acc_color.background ;

    override_colors[SYMB_FOREGROUND].pixel = loc_acc_color.foreground ;

    override_colors[SYMB_TOP_SHADOW_COLOR].pixel = 
	loc_acc_color.top_shadow_color ;

    override_colors[SYMB_BOTTOM_SHADOW_COLOR].pixel = 
	loc_acc_color.bottom_shadow_color ;

    override_colors[SYMB_SELECT_COLOR].pixel = loc_acc_color.select_color ;
}


/************************************************************************
 *
 *  GetOverrideColors
 *   Given a set of colors in acc_color with values specified or not
 *   this function fills up an XpmColorSymbol override_colors array to
 *   be used by the Xpm reader.
 *   It then calls the ColorObject aware filling proc to find values
 *   for the still unspecified ones.
 *   It returns the number of entries to consider in override_colors,
 *   which can be 2 (background and foreground are always valid), 
 *   NUM_SYMBOLIC_COLORS - 1 (if everything but the highlight could
 *   be generated), or NUM_SYMBOLIC_COLOR.
 *
 ************************************************************************/

static int
GetOverrideColors(
		Screen *screen,
		XmAccessColorData acc_color,
		XpmColorSymbol *override_colors)
{
    Cardinal i, n ;

    if (screen == NULL || acc_color == NULL || override_colors == NULL)
	return 0;

    /* init the value fields */
    for (i=0; i<NUM_SYMBOLIC_COLORS; i++) override_colors[i].value = NULL ;
	    
    /* proceed each color in turn: acc_color might point to
       XmUNSPECIFIED_PIXEL, but background/foreground should be valid */

    override_colors[SYMB_BACKGROUND].name = XmNbackground ;
    override_colors[SYMB_BACKGROUND].pixel = acc_color->background ;

    override_colors[SYMB_FOREGROUND].name = XmNforeground ;
    override_colors[SYMB_FOREGROUND].pixel = acc_color->foreground ;

    /* Set "none" color to background in case the mask is not used. */
    override_colors[TRANSPARENT_COLOR].name = NULL ;
    override_colors[TRANSPARENT_COLOR].value = "None" ;
    override_colors[TRANSPARENT_COLOR].pixel = acc_color->background ;

    override_colors[SYMB_TOP_SHADOW_COLOR].name = XmNtopShadowColor ;
    override_colors[SYMB_TOP_SHADOW_COLOR].pixel = 
	acc_color->top_shadow_color ;

    override_colors[SYMB_BOTTOM_SHADOW_COLOR].name = XmNbottomShadowColor ;
    override_colors[SYMB_BOTTOM_SHADOW_COLOR].pixel = 
	acc_color->bottom_shadow_color ;

    override_colors[SYMB_SELECT_COLOR].name = XmNselectColor ;
    override_colors[SYMB_SELECT_COLOR].pixel = acc_color->select_color ;
    /* need to hack around selectColor which might be still
       unspecified while highlight is not (and that woudl break our 
       ordering rule - see below). Use top_shadow color so that select 
       follows its set/unset semantics regarding overriding */
    if (override_colors[SYMB_SELECT_COLOR].pixel == XmUNSPECIFIED_PIXEL)
	override_colors[SYMB_SELECT_COLOR].pixel = 
	    override_colors[SYMB_TOP_SHADOW_COLOR].pixel ;

    override_colors[SYMB_HIGHLIGHT_COLOR].name = XmNhighlightColor ;
    override_colors[SYMB_HIGHLIGHT_COLOR].pixel = acc_color->highlight_color ;



    /* now call ColorObject aware routine to give it a chance to
       fill the remaining unspecified pixel field in override_colors */

    CompleteUnspecColors (screen, override_colors) ;

    /* Just count the number of valid pixel. 
       This makes a strong assumption on the ordering: if we find 
       5 valid pixels, the 5 first one will be used.
       This is ok since _XmGetColoredPixmap is only used internally
       in such a way that this happens to be true: background/foreground
       are always here, and highlight color is the only one
       that might be missing after CompleteUnspecColors call.
       The day XmGetColoredPixmap is promoted public, this simple 
       logic will have to be reviewed */

    n = 0 ;
    for (i=0; i<NUM_SYMBOLIC_COLORS; i++) 
	if (override_colors[i].pixel != XmUNSPECIFIED_PIXEL) n++;
	    
    return n ;
}



#ifdef _ORIG_GET_ICON_FILE

/******************************************************************
 * 
 *   GetIconFileName. get a file name using XBMLANGPATH
 *
 *****************************************************************/

static String
GetIconFileName(
    Screen *screen,
    char *icon_name)
{
    char *bmPath;
    SubstitutionRec	subs[1] ;
    Boolean user_path ;
    Display		*display = DisplayOfScreen(screen);
    char *file_name;
 
    subs[0].substitution = icon_name;
	  
    bmPath = _XmOSInitPath(icon_name, "XBMLANGPATH", &user_path);
	  
    if (user_path) subs[0].match = 'B';
    else           subs[0].match = MATCH_CHAR ;
	  
    file_name = XtResolvePathname(display, "bitmaps", NULL,
				  NULL, bmPath, subs, XtNumber(subs), NULL);
    XtFree (bmPath);

    return file_name ;
      
}

#endif

static XtEnum
GetXpmImage(
        Screen *screen,
        char *image_name, /* original image file name */
        char *file_name,
        XmAccessColorData acc_color,
        XImage **image,
        unsigned short *pixmap_resolution,
        Pixel **pixels,
        int *npixels)
{
    XpmAttributes attrib;
    int xpmStatus;
    Boolean useIconFileCache;
    Boolean useMask;
    Boolean useColor;
    XpmColorSymbol override_colors[NUM_SYMBOLIC_COLORS]; /* max */
    int num_override_colors;
    XImage * mask_image = NULL ;
    int hot_x = 0 , hot_y = 0 ;
    register Display *display = DisplayOfScreen(screen);

    /* init so that we can call safely XpmFreeAttributes. */
    attrib.valuemask = 0;

    /* Init the Xpm attributes to be passed to the reader */
    attrib.closeness = 40000;
    attrib.bitmap_format = XYBitmap;
    attrib.alloc_color = GetCacheColor;
    attrib.free_colors = FreeCacheColors;
    attrib.valuemask = XpmCloseness | XpmBitmapFormat | XpmReturnColorTable
	| XpmAllocColor | XpmFreeColors | XpmReturnAllocPixels;

    /* if any symbolic color are defined, used them */
    if (acc_color && 
	(num_override_colors = GetOverrideColors(screen, 
						 acc_color,
						 override_colors)) != 0) {
	attrib.colorsymbols = override_colors;
	attrib.numsymbols = num_override_colors;
	attrib.valuemask |= XpmColorSymbols;
    }

    /* ask the color object for information about the
       use of mask or color */
    (void)XmeGetIconControlInfo(screen, 
				&useMask,
				&useColor,
				&useIconFileCache); /* unused */

    if (!useColor) {
	attrib.depth = 1;
	attrib.valuemask |= XpmDepth;
    }

    *image = NULL ;

    xpmStatus = XmeXpmReadFileToImage(display,
			      file_name,
			      image,
			      &mask_image,
			      &attrib);
    if (xpmStatus < 0)
	*image = NULL ;
    else {
	/* store allocated colors */
	if (pixels) *pixels = attrib.alloc_pixels;
	if (npixels) *npixels = attrib.nalloc_pixels;
	/* and make sure they won't be free'd */
	attrib.alloc_pixels = NULL;
	attrib.nalloc_pixels = 0;
    }
    

    if (!(*image))
	*image = (XImage *) _XmReadImageAndHotSpotFromFile (display, 
							    file_name,
							    &hot_x, &hot_y);

    /* get the image "design" resolution */
    /* in the future: look in the file */
    if (pixmap_resolution) *pixmap_resolution = 0 ;


    if (*image) {

	/* if the XPM file contained a embedded mask,
	   install it using our mask name scheme if the color object
	   tell us to do so */
	if (mask_image && useMask) {
	    size_t mask_name_size;
	    String mask_name = NULL;

	    if (XmMaskNameSize(image_name, &mask_name_size))
		mask_name = XtMalloc((Cardinal)mask_name_size);

	    if (mask_name != NULL) {
		Boolean installed;

		_XmOSGenerateMaskName(image_name, mask_name);
		/* if an image already exists under that name, release this copy */
		installed = _XmInstallImage(mask_image, mask_name, hot_x, hot_y);
		XtFree(mask_name);
		if (!installed)
		    XDestroyImage(mask_image);
	    } else {
		XDestroyImage(mask_image);
	    }
	} else if (mask_image) {
	    XDestroyImage(mask_image);
	}

	/* now we have to adjust the passed acc_color */
	if (acc_color) {
	    if ((*image)->depth == 1) {
		/* we've loaded an xbm file, just forget about
		   the 'other' symbolic colors, for they
		   are not going to be used during pixmap 
		   generation for depth 1 => pixmap putimage */
		acc_color->top_shadow_color = XmUNSPECIFIED_PIXEL;
		acc_color->bottom_shadow_color= XmUNSPECIFIED_PIXEL;
		acc_color->select_color = XmUNSPECIFIED_PIXEL;
		acc_color->highlight_color = XmUNSPECIFIED_PIXEL;
		if (acc_color->foreground == XmUNSPECIFIED_PIXEL
		    && acc_color->background == XmUNSPECIFIED_PIXEL) {
		    acc_color->foreground = 1;
		    acc_color->background = 0;
		}
	    } else {

		/* we've loaded a xpm, check which symbolics
		   colors were actually used during the read,
		   we don't want to remember the unused one
		   for the pixmap caching */
	
		if (!SymbolicColorUsed(XmNbackground,
				       attrib.colorTable,
				       attrib.ncolors))
		    acc_color->background = XmUNSPECIFIED_PIXEL;
		
		if (!SymbolicColorUsed(XmNforeground,
				       attrib.colorTable,
				       attrib.ncolors))
		    acc_color->foreground = XmUNSPECIFIED_PIXEL;
		
		if (!SymbolicColorUsed(XmNtopShadowColor,
				       attrib.colorTable,
				       attrib.ncolors))
		    acc_color->top_shadow_color = XmUNSPECIFIED_PIXEL;	
		
		if (!SymbolicColorUsed(XmNbottomShadowColor,
				       attrib.colorTable,
				       attrib.ncolors))
		    acc_color->bottom_shadow_color = XmUNSPECIFIED_PIXEL;
		
		if (!SymbolicColorUsed(XmNselectColor,
				       attrib.colorTable,
				       attrib.ncolors))
		    acc_color->select_color = XmUNSPECIFIED_PIXEL;
		
		if (!SymbolicColorUsed(XmNhighlightColor,
				       attrib.colorTable,
				       attrib.ncolors))
		    acc_color->highlight_color = XmUNSPECIFIED_PIXEL;
		
	    }	
	}	
	

	/* install the XImage with that name.
	   We do not cache the non-depth 1 ones
	   for we don't want to keep them in the image
	   cache, since they need color lookup,
	   which is done one level up in the pixmap cache */
	if (((*image)->depth == 1)
            && acc_color
	    && (acc_color->foreground == 1)
	    && (acc_color->background == 0)
	    ) {
	    _XmInstallImage (*image, image_name, hot_x, hot_y);
	    return TRUE ;
	} else {
	    if (xpmStatus >= 0)
		XmeXpmFreeAttributes(&attrib);
	    return NOT_CACHED ; /* mean the image can be destroyed
				   after it is used */
	}
    }
    
    if (xpmStatus >= 0)
	XmeXpmFreeAttributes(&attrib);

    return FALSE;
}

/************************************************************************
 *
 *  GetImage
 *	Main routine of the image cache part.
 *
 ************************************************************************/
static XtEnum
GetImage(
	 Screen *screen,
	 char *image_name,
         XmAccessColorData acc_color,
	 XImage **image,
	 unsigned short * pixmap_resolution,
	 Pixel **pixels,	/* allocated pixels */
	 int *npixels)
{
    static XImage  * built_in_image = NULL;

    if (screen == NULL || image == NULL)
        return FALSE;
    *image = NULL;
    register Display *display = DisplayOfScreen(screen);
    ImageData *entry;
    char *file_name;
    XtEnum return_value;
#if defined (PNG_SUPPORTED) || defined (JPEG_SUPPORTED)
    FILE *infile;
    int rc;
#endif

    /* init for when we go thru the image cache first */
    if (pixmap_resolution) *pixmap_resolution = 0 ;

    if (pixels) *pixels = NULL;
    if (npixels) *npixels = 0;

    /***  Check for the initial allocation of the image set array  */
    
    if (image_set == NULL) InitializeImageSet();
    if (image_set == NULL)
        return FALSE;

    if (!image_name) return FALSE ;

    /*** look in the XImage cache first */
    _XmProcessLock();
    entry = (ImageData*) _XmGetHashEntry(image_set, image_name);
    _XmProcessUnlock();
    

    if (entry) {

	/*  If the image is a builtin image then get it.  */

	if (entry->builtin_data) {
	    /* there is one builtin XImage shared by all builtins,
	       only the data part change every time a query is made */
	    _XmProcessLock();
	    if (!built_in_image) {
		/* update that with new R6 init image stuff when out */
		_XmCreateImage(built_in_image, display, NULL,
			       16, 16, MSBFirst);
		if (!built_in_image) {
		    _XmProcessUnlock();
		    return FALSE;
		}
	    }
	
	    built_in_image->data = (char *) entry->builtin_data;
	    _XmProcessUnlock();
	    *image = built_in_image;
	    
	} else {   

	    /* other entry found are just fine, set the image and return  */
    
	    *image = entry->image ;
	}	

	return TRUE;
    }

#ifdef SVG_SUPPORTED
    /* Embedded artwork uses the ordinary color/depth/scale-aware pixmap
     * cache. An explicitly installed image above keeps precedence. */
    if (_XmSvgGetBuiltinImage(screen, image_name,
            acc_color && acc_color->background != XmUNSPECIFIED_PIXEL
                ? acc_color->background : WhitePixelOfScreen(screen), image) == 0)
        return NOT_CACHED;
#endif

    /*** if no entry, try to read a new file and cache it
         only if it is a bitmap */

#ifdef _ORIG_GET_ICON_FILE
    file_name = GetIconFileName(screen, image_name);
#else
    file_name = XmGetIconFileName(screen, NULL, image_name, 
				  NULL, XmUNSPECIFIED_ICON_SIZE);
#endif

    if (!file_name) {	
        return FALSE;
    }

#ifdef SVG_SUPPORTED
    if (IsSvgFile(file_name)) {
        Pixel background = 0;

        if (acc_color && acc_color->background != XmUNSPECIFIED_PIXEL)
            background = acc_color->background;
        return_value = _XmSvgGetImage(screen, file_name, background, image) == 0
                           ? NOT_CACHED : FALSE;
        XtFree(file_name);
        return return_value;
    }
#endif

#if defined (JPEG_SUPPORTED) || defined (PNG_SUPPORTED)
    if (!(infile = fopen(file_name, "rb"))) {
        XtFree(file_name);
        return FALSE;
    }

#ifdef JPEG_SUPPORTED
    rc = _XmJpegGetImage(screen, infile, image);
#endif
#if defined (JPEG_SUPPORTED) && defined (PNG_SUPPORTED)
    if (rc == 1) { /* not a jpeg file */
#endif
#ifdef PNG_SUPPORTED
        Pixel background;

        if (acc_color)
            background = acc_color->background;
        else
            background = 0;

        if (background == XmUNSPECIFIED_PIXEL)
            background = 0; /* XXX is if OK? */
#endif
#if defined (JPEG_SUPPORTED) && defined (PNG_SUPPORTED)
        rewind(infile);
#endif
#ifdef PNG_SUPPORTED
        rc = _XmPngGetImage(screen, infile, background, image);
#endif
#if defined (JPEG_SUPPORTED) && defined (PNG_SUPPORTED)
    }
#endif

    fclose(infile);

    if (rc > 1) {
        return_value = FALSE;
    } else if (rc == 0) {
        return_value = NOT_CACHED;
    } else {
#endif
        return_value =
            GetXpmImage(screen, image_name, file_name, acc_color,
                    image, pixmap_resolution, pixels, npixels);
#if defined (JPEG_SUPPORTED) || defined (PNG_SUPPORTED)
    }
#endif

    XtFree(file_name);

    return return_value;
} 

/*** Keep this one in here, this is the only entry point to
     the Image cache. It can be used to duplicate them, etc */
Boolean
_XmGetImage(
	Screen *screen,
	char *image_name,
	XImage **image )
{
    return GetImage(screen, image_name, NULL, image, NULL, NULL, NULL) ;
}


/************************************************************************
*
*  _XmInImageCache   
*       Used by IconFile.c
*
************************************************************************/

Boolean
_XmInImageCache(
	String image_name)
{
    XtPointer	ret_val;

    if (image_name == NULL || !image_set) return False ;
    _XmProcessLock();
    ret_val = _XmGetHashEntry(image_set, image_name);
    _XmProcessUnlock();

    return (ret_val != NULL) ;
}









/*** PIXMAP CACHE NOW ***/

/** static dependencies with Image part: 
  * GetImage(), ImageData&image_set, HashString 
  **/
/** static dependencies with Color part: 
  * FreeCacheColors()
  **/




static Boolean
ComparePixmaps (XmHashKey key_1, 
		XmHashKey key_2)
{
  PixmapData *data_1 = (PixmapData *) key_1;
  PixmapData *data_2 = (PixmapData *) key_2;

    /* Check for a matching pixmap using screen and pixmap */

    return 
	(data_1->screen == data_2->screen &&
	 data_1->pixmap == data_2->pixmap) ;
}

static Boolean
ComparePixmapDatas (XmHashKey key_1, 
		    XmHashKey key_2)
{
  PixmapData *data_1 = (PixmapData *) key_1;
  PixmapData *data_2 = (PixmapData *) key_2;

    /* Check for a matching pixmap using depth, screen, name and
       colors info + print_resolution/scaling_ratio and print_shell */
    /* if a negative depth is given, we must look for a matching
       pixmap of -depth or 1 */

    return
        (data_1 != NULL && data_2 != NULL &&
         data_1->acc_color != NULL && data_2->acc_color != NULL &&
	 (data_2->pixmap == XmUNSPECIFIED_PIXMAP ||
	  data_1->pixmap == data_2->pixmap) &&
	 data_1->image_name != NULL &&
	 data_2->image_name != NULL &&
	 strcmp (data_1->image_name, data_2->image_name) == 0  &&

	 (data_1->screen == data_2->screen) &&

	 ( /* ratio == 0 means print resolution is used */
	  (data_1->scaling_ratio == data_2->scaling_ratio &&
	   data_1->scaling_ratio) ||
	  (data_1->print_resolution == data_2->print_resolution &&
	   !data_1->scaling_ratio)) &&

	 (data_1->print_shell == data_2->print_shell) &&

	 ((data_1->depth == data_2->depth)  ||
	  (data_2->depth < 0 && data_1->depth == -data_2->depth) ||
	  (data_2->depth < 0 && data_1->depth == 1)) &&

	 ((data_1->acc_color->foreground == data_2->acc_color->foreground) ||
	  (data_2->depth < 0 && data_1->acc_color->foreground == 1) ||
	  (data_1->acc_color->foreground == XmUNSPECIFIED_PIXEL) ||
	  (data_2->acc_color->foreground == XmUNSPECIFIED_PIXEL))          &&
	 ((data_1->acc_color->background == data_2->acc_color->background) ||
	  (data_2->depth < 0 && data_1->acc_color->background == 0) ||
	  (data_1->acc_color->background == XmUNSPECIFIED_PIXEL)||
	  (data_2->acc_color->background == XmUNSPECIFIED_PIXEL))          &&

	 ((data_1->acc_color->top_shadow_color == 
	   data_2->acc_color->top_shadow_color) ||
	  (data_1->acc_color->top_shadow_color == XmUNSPECIFIED_PIXEL)||
	  (data_2->acc_color->top_shadow_color == XmUNSPECIFIED_PIXEL))     &&
	 ((data_1->acc_color->bottom_shadow_color 
	   == data_2->acc_color->bottom_shadow_color)||
	  (data_1->acc_color->bottom_shadow_color == XmUNSPECIFIED_PIXEL)||
	  (data_2->acc_color->bottom_shadow_color == XmUNSPECIFIED_PIXEL))  &&
	 ((data_1->acc_color->select_color == 
	   data_2->acc_color->select_color) ||
	  (data_1->acc_color->select_color == XmUNSPECIFIED_PIXEL)||
	  (data_2->acc_color->select_color == XmUNSPECIFIED_PIXEL))       &&
	 ((data_1->acc_color->highlight_color == 
	   data_2->acc_color->highlight_color) ||
	  (data_1->acc_color->highlight_color == XmUNSPECIFIED_PIXEL) ||
	  (data_2->acc_color->highlight_color == XmUNSPECIFIED_PIXEL))) ;
}

/* Hash a Pixmap entry. */
static XmHashValue 
HashPixmap (XmHashKey key)
{
  PixmapData *data = (PixmapData *) key;

  return ((long)data->screen + data->pixmap);
}

/* Hash a PixmapData entry. Reuse the string hash function */
static XmHashValue 
HashPixmapData (XmHashKey key)
{
  PixmapData *data = (PixmapData *) key;

  return ((long)data->screen + HashString ((XmHashKey)data->image_name));
}

/************************************************************************
 *
 *  InitializePixmapSets
 *	Initialize the pixmap sets.
 *
 ************************************************************************/
static void 
InitializePixmapSets( void )
{
    XmHashTable new_pixmap_data_set;
    XmHashTable new_pixmap_set;

    /* Allocate both tables before publishing either global. */
    _XmProcessLock();
    if (pixmap_data_set != NULL || pixmap_set != NULL)
      {
        _XmProcessUnlock();
        return;
      }

    new_pixmap_data_set = _XmAllocHashTable (100, ComparePixmapDatas,
                                             HashPixmapData);
    new_pixmap_set = _XmAllocHashTable (100, ComparePixmaps, HashPixmap);
    if (new_pixmap_data_set != NULL && new_pixmap_set != NULL) {
        pixmap_data_set = new_pixmap_data_set;
        pixmap_set = new_pixmap_set;
    }
    _XmProcessUnlock();

    if (new_pixmap_data_set == NULL && new_pixmap_set != NULL)
        _XmFreeHashTable(new_pixmap_set);
    else if (new_pixmap_data_set != NULL && new_pixmap_set == NULL)
        _XmFreeHashTable(new_pixmap_data_set);
}


/************************************************************************
 *
 *  _XmCachePixmap
 *	Install a pixmap into the pixmap cache.  This is used to add
 *	cached pixmaps which have no XImage associated with them.
 *      This one you can pass depth and size info, if you don't,
 *      the X server will be queried: very expensive.
 *
 ************************************************************************/
Boolean 
_XmCachePixmap(
        Pixmap pixmap,
        Screen *screen,
        char *image_name,
        Pixel foreground,
        Pixel background,
        int depth,
        Dimension width,
        Dimension height)
{
   PixmapData * pix_entry;
   Boolean cache_inserted;

   Window root;
   int x,y;
   unsigned int loc_width, loc_height, border_width, loc_depth;
   
   /*  Error checking  */
   if (screen == NULL || pixmap == XmUNSPECIFIED_PIXMAP ||
       image_name == NULL)
       return (False);

   if (!pixmap_data_set) InitializePixmapSets() ;
   if (pixmap_data_set == NULL || pixmap_set == NULL)
       return (False);

   /* if no information was given, get it from the server */
   if (!(width && height && depth)) {
       if (!XGetGeometry(DisplayOfScreen(screen), pixmap, &root, &x, &y,
			 &loc_width, &loc_height, &border_width, &loc_depth))
		   return (False);
   } else {
	   loc_width = width ;
	   loc_height = height ;
	   loc_depth = depth ;
       }
   
   /*  Allocate the cache structure and put it into the list  */

   pix_entry = XtNew (PixmapData);
   if (pix_entry == NULL)
       return (False);

   pix_entry->screen = screen;
   pix_entry->acc_color = XtNew(XmAccessColorDataRec);
   if (pix_entry->acc_color == NULL) {
       XtFree((char *)pix_entry);
       return (False);
   }
   pix_entry->acc_color->foreground = foreground;
   pix_entry->acc_color->background = background;
   pix_entry->acc_color->top_shadow_color = XmUNSPECIFIED_PIXEL;
   pix_entry->acc_color->bottom_shadow_color = XmUNSPECIFIED_PIXEL;
   pix_entry->acc_color->select_color = XmUNSPECIFIED_PIXEL;
   pix_entry->acc_color->highlight_color = XmUNSPECIFIED_PIXEL;
   pix_entry->depth = loc_depth;
   pix_entry->width = loc_width;
   pix_entry->height = loc_height;
   pix_entry->image_name = XtNewString(image_name);
   if (pix_entry->image_name == NULL) {
       XtFree((char *)pix_entry->acc_color);
       XtFree((char *)pix_entry);
       return (False);
   }
   pix_entry->pixmap = pixmap;
   pix_entry->reference_count = 1;
   pix_entry->print_resolution = 100 ;
   pix_entry->print_shell = NULL ;
   pix_entry->scaling_ratio = 1 ;
   pix_entry->pixels = NULL;
   pix_entry->npixels = 0;

   _XmProcessLock();
   cache_inserted = _XmAddHashEntry(pixmap_set, (XmHashKey)pix_entry,
					(XtPointer)pix_entry);

   /* Only add pixmaps not issued from XmeGetPixmapData to the "real" table */
   if (cache_inserted && 0 != strcmp(image_name, DIRECT_PIXMAP_CACHED) &&
	   !_XmAddHashEntry(pixmap_data_set, (XmHashKey)pix_entry,
				(XtPointer)pix_entry)) {
	_XmRemoveHashEntry(pixmap_set, pix_entry);
	cache_inserted = False;
   }
   _XmProcessUnlock();

   if (!cache_inserted) {
	XtFree((char *)pix_entry->image_name);
	XtFree((char *)pix_entry->acc_color);
	XtFree((char *)pix_entry);
	return (False);
   }

   return (True);
}



/********************************************************************/

Boolean 
_XmGetPixmapData(
		   Screen *screen,
		   Pixmap pixmap,
		   char **image_name,
		   int *depth,
		   Pixel *foreground,
		   Pixel *background,
		   int *hot_x,
		   int *hot_y,
		   unsigned int *width,
		   unsigned int *height)
/* no pixmap_resolution returned... */
{
    PixmapData pix_data, *pix_entry;
    ImageData * entry;

    if (screen == NULL || pixmap == XmUNSPECIFIED_PIXMAP)
        return False;

    if (!pixmap_data_set) InitializePixmapSets() ;
    if (pixmap_data_set == NULL || pixmap_set == NULL)
        return False;

    if (image_name) *image_name = NULL;
    if (depth) *depth = 0;
    if (foreground) *foreground = 0;
    if (background) *background = 0;
    if (hot_x) *hot_x = 0;
    if (hot_y) *hot_y = 0;
    if (width) *width = 0;
    if (height) *height = 0;

    /* Check for a matching screen and pixmap. */
    pix_data.screen = screen;
    pix_data.pixmap = pixmap;

    _XmProcessLock();
    pix_entry = (PixmapData *)
        _XmGetHashEntry(pixmap_set, (XmHashKey)&pix_data);
    if (pix_entry != NULL) {
        if (foreground) *foreground = pix_entry->acc_color->foreground;
        if (background) *background = pix_entry->acc_color->background;
        if (depth) *depth = pix_entry->depth;
        if (image_name) *image_name = pix_entry->image_name;
        if (width) *width = pix_entry->width;
        if (height) *height = pix_entry->height;
        /* Hotspots are optional image metadata; default them when absent. */
        if (image_set) {
            entry = (ImageData *)
                _XmGetHashEntry(image_set, pix_entry->image_name);
            if (entry) {
                if (hot_x) *hot_x = entry->hot_x;
                if (hot_y) *hot_y = entry->hot_y;
            }
        }
        _XmProcessUnlock();
        return True;
    }
    _XmProcessUnlock();
    return False;
}

/*******************************************************************/
/* Release temporary state when scaled-pixmap construction cannot
 * allocate its cache record. */
static void
DiscardScaledPixmapBuild(
    XtEnum ret,
    XImage *image,
    char *old_image_data,
    int old_image_format,
    Boolean image_lock_held,
    Screen *screen,
    Pixel *pixels,
    int npixels,
    PixmapData *pix_entry)
{
    if (image_lock_held)
      {
        if (old_image_data != NULL)
          {
            image->format = old_image_format;
            if (image->data != old_image_data)
              XtFree(image->data);
            image->data = old_image_data;
          }
        _XmProcessUnlock();
      }

    if (ret == NOT_CACHED)
      {
        XDestroyImage(image);
        if (old_image_data != NULL)
          XtFree(old_image_data);
      }

    if (pix_entry != NULL)
      {
        XtFree(pix_entry->image_name);
        XtFree((char *)pix_entry->acc_color);
        XtFree((char *)pix_entry);
      }
    ReleaseImagePixels(screen, pixels, npixels);
}

/*******************************************************************/
/* the real one, used by PixConv and locally too */
Pixmap 
_XmGetScaledPixmap(
    Screen *screen,
    Widget widget,
    char *image_name,
    XmAccessColorData acc_color,
    int depth,
#if NeedWidePrototypes
    int only_if_exists,
#else
    Boolean only_if_exists,
#endif /* NeedWidePrototypes */
    double scaling_ratio)
{    
    Display * display;
    XImage * image;
    Pixmap   pixmap;
    GC gc ;
    XtEnum ret ;
    PixmapData pix_data, *pix_entry ;
    unsigned short pixmap_resolution ;
    Pixel *pixels;		/* allocated colors */
    int npixels;
    int old_image_format;
    char *old_image_data = NULL;
    Boolean image_lock_held = False;
    Boolean caller_owns_gc = False;
    Boolean cache_inserted;
    Dimension scaled_width, scaled_height;

    /*  Error checking  */    
    if (image_name == NULL || acc_color == NULL)
	return (XmUNSPECIFIED_PIXMAP);

    if (!pixmap_data_set) InitializePixmapSets() ;
    if (pixmap_data_set == NULL || pixmap_set == NULL)
        return (XmUNSPECIFIED_PIXMAP);

    /* if screen not provided, widget has to be! */
    if (!screen && widget) screen = XtScreen(widget);
    if (!screen)
	return (XmUNSPECIFIED_PIXMAP);
    display = DisplayOfScreen(screen);

    /* since more than one printers can attach to one screen,
       we need to use the print shell in the caching */
    /* since resolution can be dynamic for a printer, we need
       to use the resolution too, when scaling is 0 */
    pix_data.screen = screen ;
    pix_data.image_name = image_name ;
    pix_data.depth = depth ;
    pix_data.acc_color = acc_color ;
    pix_data.print_resolution = 100 ; /* default */
    pix_data.scaling_ratio = scaling_ratio ;

    pix_data.pixmap = XmUNSPECIFIED_PIXMAP;

    /* find out the print shell ancestor */
    pix_data.print_shell = widget ;
    while(pix_data.print_shell && !XmIsPrintShell(pix_data.print_shell)) 
	pix_data.print_shell = XtParent(pix_data.print_shell);
    /* pix_data.print_shell might be NULL here */

#ifdef PRINTING_SUPPORTED
    /* scaling_ratio == 0 means use print_resolution and pixmap_resolution 
       in scaling - so first find out the print_resolution, since it
       is used in caching */
    if (!scaling_ratio && pix_data.print_shell) 
	pix_data.print_resolution = 
	  ((XmPrintShellWidget)pix_data.print_shell)->print.print_resolution ;
#endif

    /* if scaling_ratio a real number, like 1 or 1.2 
       print_resolution still 100 and will not be used */

    /* look if we have a match and return it */
    _XmProcessLock();
    if ((pix_entry = (PixmapData*) 
	 _XmGetHashEntry(pixmap_data_set, (XmHashKey)&pix_data)) != NULL) {
	pix_entry->reference_count++;
	_XmProcessUnlock();
	return pix_entry->pixmap ;
    }
    _XmProcessUnlock();
	
    /* The desired pixmap is not already cached.  Quit now or create it. */
    if (only_if_exists)
	return (XmUNSPECIFIED_PIXMAP);

    /* no pixmap found, look for an XImage. 
       generate a new one if needed.
       GetImage can return a bitmap or a pixmap XImage */
    /* GetImage modify the acc_color struct to update the
       colors that are not used during reading:
         - for xbm, everything but foreground and background.
	 - for xpm, on a case by case basis, depending
	 if the corresponding symbolic color was used. */
       
    if (!(ret = GetImage(screen, image_name, acc_color, &image, 
			 &pixmap_resolution, &pixels, &npixels)))
	return (XmUNSPECIFIED_PIXMAP);

    /* now find how much we need to scale this baby */

    /* compute the real ratio to be used in PutImage */
    if (!pix_data.scaling_ratio) {
	/* if scaling ratio is real, like 1.2, the pixmap resolution is
	   not used, neither is the print_resolution */

	if (!pixmap_resolution) {
	    /* GetImage returns the resolution for which the image 
	       was designed. if none was provided in the file, it's 0, 
	       in which case, the print shell default resolution 
	       for pixmap is used */

#ifdef PRINTING_SUPPORTED
	    if (pix_data.print_shell) 
		pixmap_resolution = 
		    ((XmPrintShellWidget)pix_data.print_shell)
			->print.default_pixmap_resolution ;
	    else
#endif /* PRINTING_SUPPORTED */
		pixmap_resolution = 100 ;
	}

	pix_data.scaling_ratio = (double)pix_data.print_resolution / 
	    (double)pixmap_resolution ;
    } 


    if (image->width <= 0 || image->height <= 0 ||
	isfinite(pix_data.scaling_ratio) == 0 || pix_data.scaling_ratio <= 0.0 ||
	!XmScaledDimension((unsigned int)image->width,
			   pix_data.scaling_ratio, &scaled_width) ||
	!XmScaledDimension((unsigned int)image->height,
			   pix_data.scaling_ratio, &scaled_height)) {
	if (ret == NOT_CACHED)
	    XDestroyImage(image);
	ReleaseImagePixels(screen, pixels, npixels);
	return (XmUNSPECIFIED_PIXMAP);
    }
    

    /* now we treat the -depth case: if a negative depth was given
       then xbm file should map into bitmap and xpm into pixmap */
   
    if (depth < 0) {
	if (image->depth == 1) 
	  depth = 1 ;
	else 
	  depth = -depth ;
    }

    /* now check the validity of the image depth.  XPutImage can
       only handle unequal depths for bitmaps. */
    if ((image->depth != depth) && (image->depth != 1))
	{
	if (ret == NOT_CACHED)
	    XDestroyImage(image);
	ReleaseImagePixels(screen, pixels, npixels);
	return (XmUNSPECIFIED_PIXMAP);
	}

    /* force the foreground/background if a Bitmap
       is returned. These values are going to go in the cache
       too, so we will have to remember this forcing for the
       case -depth: you ask for -depth and some 
       foreground/background, that should match a depth 1
       with 1/0. Look at ComparePixmapDatas above for details */

    if (depth == 1) {
	acc_color->foreground = 1;
	acc_color->background = 0;
    }

    /* XPutImage will only deepen bitmaps -- fake it with pixmaps. */
    old_image_format = image->format;
    if (image->depth == 1) { 
      switch(image->format)
	{
	case XYBitmap:
	  /* XPutImage will do the right thing. */
	  break;

	case XYPixmap:
	  /* We're going to muck with the shared image... */
	  if (ret != NOT_CACHED)
	    {
	      _XmProcessLock();
	      image_lock_held = True;
	    }

	  /* Assume black == fg in the image. */
	  if ((BlackPixelOfScreen(screen) == 0) ||
	      (WhitePixelOfScreen(screen) == 1))
	    {
	      /* Flip the bits so fg == 1 in the image. */
	      size_t nbytes;
	      char *new_image_data;
	      size_t byte;

	      if (image->height <= 0 || image->bytes_per_line <= 0 ||
		  (size_t)image->height >
		  SIZE_MAX / (size_t)image->bytes_per_line) {
		if (ret != NOT_CACHED)
		    _XmProcessUnlock();
		if (ret == NOT_CACHED)
		    XDestroyImage(image);
		return (XmUNSPECIFIED_PIXMAP);
	      }
	      nbytes = (size_t)image->height *
		       (size_t)image->bytes_per_line;

	      /* Image data may be constant, so we must copy it. */
	      old_image_data = image->data;
	      new_image_data = XtMalloc(nbytes);
	      if (new_image_data == NULL) {
		if (ret != NOT_CACHED)
		    _XmProcessUnlock();
		if (ret == NOT_CACHED)
		    XDestroyImage(image);
		return (XmUNSPECIFIED_PIXMAP);
	      }
	      image->data = new_image_data;

	      for (byte = 0; byte < nbytes; byte++)
		image->data[byte] = ~old_image_data[byte];
	    }

	  /* In this depth-1 case the image formats are equivalent. */
	  image->format = XYBitmap;
	  break;

	case ZPixmap:
	  /* Assume the application really wants BadMatch if (depth != 1). */
	  break;

	default:
	  DiscardScaledPixmapBuild(ret, image, old_image_data,
				   old_image_format, image_lock_held,
				   screen, pixels, npixels, NULL);
	  return (XmUNSPECIFIED_PIXMAP);
	}
    }

    
    /*
     * Create a pixmap to hold the image, allocate a new pixmap
     * cache entry, put the cache entry in the tables.
     */
    
    pix_entry = XtNew (PixmapData);
    if (pix_entry == NULL)
      {
        DiscardScaledPixmapBuild(ret, image, old_image_data,
                                 old_image_format, image_lock_held,
                                 screen, pixels, npixels, NULL);
        return (XmUNSPECIFIED_PIXMAP);
      }
    pix_entry->acc_color = NULL;
    pix_entry->image_name = NULL;

    pix_entry->screen = screen;
    pix_entry->acc_color = XtNew(XmAccessColorDataRec);
    if (pix_entry->acc_color == NULL)
      {
        DiscardScaledPixmapBuild(ret, image, old_image_data,
                                 old_image_format, image_lock_held,
                                 screen, pixels, npixels, pix_entry);
        return (XmUNSPECIFIED_PIXMAP);
      }
    memcpy((void*)pix_entry->acc_color, (void*)acc_color,
	   sizeof(XmAccessColorDataRec));
    pix_entry->depth = depth;
    pix_entry->image_name = XtNewString(image_name);
    if (pix_entry->image_name == NULL)
      {
        DiscardScaledPixmapBuild(ret, image, old_image_data,
                                 old_image_format, image_lock_held,
                                 screen, pixels, npixels, pix_entry);
        return (XmUNSPECIFIED_PIXMAP);
      }
    pix_entry->print_shell = pix_data.print_shell;
    pix_entry->print_resolution = pix_data.print_resolution ;
    pix_entry->pixels = pixels;
    pix_entry->npixels = npixels;

    /* use scaling_ratio for caching: as 0, in which case the
       print resolution will be used, or 1, no scaling, or a real ratio. 
       That's because when we enter this function, we don't have 
       pixmap_resolution to cache against, and also because 
       print_resolution might change for the same print shell */
    pix_entry->scaling_ratio = scaling_ratio ;

    /* use the pixmap scaling ratio for printing  */
    pix_entry->width = scaled_width;
    pix_entry->height = scaled_height;

    pixmap = XCreatePixmap (display, RootWindowOfScreen(screen), 
			    pix_entry->width,
			    pix_entry->height,
			    depth);

    if (pixmap == XmUNSPECIFIED_PIXMAP) {
        DiscardScaledPixmapBuild(ret, image, old_image_data,
                                 old_image_format, image_lock_held,
                                 screen, pixels, npixels, pix_entry);
        return (XmUNSPECIFIED_PIXMAP);
    }
    
    pix_entry->pixmap = pixmap;
    pix_entry->reference_count = 1;
    
    /* put the new entry in both tables: the table used for
       this routine and the table used in XmeGetPixmapData */
    _XmProcessLock();
    cache_inserted = _XmAddHashEntry(pixmap_set, (XmHashKey)pix_entry,
				     (XtPointer)pix_entry);
    if (cache_inserted &&
	!_XmAddHashEntry(pixmap_data_set, (XmHashKey)pix_entry,
				 (XtPointer)pix_entry)) {
	_XmRemoveHashEntry(pixmap_set, pix_entry);
	cache_inserted = False;
    }
    _XmProcessUnlock();

    if (!cache_inserted)
      {
	XFreePixmap(display, pixmap);
	DiscardScaledPixmapBuild(ret, image, old_image_data,
				 old_image_format, image_lock_held,
				 screen, pixels, npixels, pix_entry);
	return (XmUNSPECIFIED_PIXMAP);
      }

    /*  Set up a gc for the image to pixmap copy, store the image  */
    /*  into the pixmap and return the pixmap.                     */
    
    gc = GetGCForPutImage(screen, pix_entry->print_shell,
			  image, pixmap, depth,
			  acc_color->foreground,
			  acc_color->background,
			  &caller_owns_gc);
    if (gc == NULL)
      {
        _XmProcessLock();
        _XmRemoveHashEntry(pixmap_data_set, pix_entry);
        _XmRemoveHashEntry(pixmap_set, pix_entry);
        _XmProcessUnlock();
        XFreePixmap(display, pixmap);
        DiscardScaledPixmapBuild(ret, image, old_image_data,
                                 old_image_format, image_lock_held,
                                 screen, pixels, npixels, pix_entry);
        return (XmUNSPECIFIED_PIXMAP);
      }

    /* transfer and scale the image */
    _XmPutScaledImage (display, pixmap, gc, image, 
		       0, 0, 0, 0, image->width, image->height,
		       pix_entry->width, pix_entry->height);
    if (caller_owns_gc)
	XFreeGC(display, gc);

    /* Destroy non-cached XImage now that we've cached the pixmap. */
    if (ret == NOT_CACHED)
      {
	XDestroyImage(image) ;
	if (old_image_data)
	  XtFree(old_image_data);
      }
    else if (image->format != old_image_format)
      {
	/* Restore the original image format after temporary conversion. */
	image->format = old_image_format;

	if (old_image_data)
	  {
	    XtFree(image->data);
	    image->data = old_image_data;
	  }

	/* We're done mangling the image. */
	_XmProcessUnlock();
      }

    return (pixmap);
}


/*******************************************************************
 *
 * _XmGetColoredPixmap
 *   a improved version of XmGetPixmapByDepth that lets the caller
 *   specify more color information usable during the loading
 *   of the pixmap file to override symbolic colors.
 *
 *   It also supports the negative depth convention where if a -depth
 *   is given, xbm file give Bitmap and xpm file give Pixmap, while
 *   with positive depth, all files give Pixmap result.
 *
 *   Now a wrapper, but still used by IconG, MessageB, and Text directly.
 *
 *******************************************************************/
Pixmap 
_XmGetColoredPixmap(Screen *screen,
		    char *image_name,
		    XmAccessColorData acc_color,
		    int depth,
#if NeedWidePrototypes
		   int only_if_exists)
#else
                   Boolean only_if_exists)
#endif /* NeedWidePrototypes */
{  
    return _XmGetScaledPixmap (screen, NULL,
			       image_name, acc_color, depth,
			       only_if_exists, 1); /* no scaling */
}

Pixmap 
XmGetScaledPixmap(
    Widget widget,
    char *image_name,
    Pixel foreground,
    Pixel background,
    int depth,
    double scaling_ratio)
{  
    XmAccessColorDataRec acc_color_rec;
    Pixmap ret_val;

    if (widget == NULL || image_name == NULL)
	return XmUNSPECIFIED_PIXMAP;
    XtAppContext app = XtWidgetToApplicationContext(widget);

    _XmAppLock(app);
    _XmProcessLock();
    acc_color_rec.foreground = foreground ;
    acc_color_rec.background = background ;
    acc_color_rec.top_shadow_color = XmUNSPECIFIED_PIXEL ;
    acc_color_rec.bottom_shadow_color = XmUNSPECIFIED_PIXEL ;
    acc_color_rec.select_color = XmUNSPECIFIED_PIXEL ;
    acc_color_rec.highlight_color = XmUNSPECIFIED_PIXEL ;
    ret_val = _XmGetScaledPixmap(XtScreen(widget), widget, image_name, 
				 &acc_color_rec, depth, False,
				 scaling_ratio);
    _XmProcessUnlock();
    _XmAppUnlock(app);

    return  ret_val;

}


/*******************************************************************
 *
 * XmGetPixmapByDepth.
 *  create a pixmap from the image_name.  foreground and background
 *  must be valid values. For depth 1 they should be 1 and 0
 *  respectively. 
 *
 *******************************************************************/
Pixmap 
XmGetPixmapByDepth(
    Screen *screen,
    char *image_name,
    Pixel foreground,
    Pixel background,
    int depth)
{    
    XmAccessColorDataRec acc_color_rec;
    Pixmap ret_val;

    if (screen == NULL || image_name == NULL)
	return XmUNSPECIFIED_PIXMAP;
    XtAppContext app = XtDisplayToApplicationContext(
				DisplayOfScreen(screen));

    _XmAppLock(app);
    _XmProcessLock();
    acc_color_rec.foreground = foreground ;
    acc_color_rec.background = background ;
    acc_color_rec.top_shadow_color = XmUNSPECIFIED_PIXEL ;
    acc_color_rec.bottom_shadow_color = XmUNSPECIFIED_PIXEL ;
    acc_color_rec.select_color = XmUNSPECIFIED_PIXEL ;
    acc_color_rec.highlight_color = XmUNSPECIFIED_PIXEL ;
    ret_val = _XmGetScaledPixmap(screen, NULL, image_name, 
				 &acc_color_rec, depth, False,
				 1);
    _XmProcessUnlock();
    _XmAppUnlock(app);

    return  ret_val;
}


/************************************************************************
 *
 *  This one is deprecated, but keep it in here, it was public.
 *
 ************************************************************************/
			 
Pixmap 
XmGetPixmap(
        Screen *screen,
        char *image_name,
        Pixel foreground,
        Pixel background )
{
    XmAccessColorDataRec acc_color_rec;
    Pixmap	ret_val;

    if (screen == NULL || image_name == NULL)
	return XmUNSPECIFIED_PIXMAP;

    XtAppContext app = XtDisplayToApplicationContext(
				DisplayOfScreen(screen));
    _XmAppLock(app);
    _XmProcessLock();
    acc_color_rec.foreground = foreground;
    acc_color_rec.background = background;
    acc_color_rec.top_shadow_color = XmUNSPECIFIED_PIXEL;
    acc_color_rec.bottom_shadow_color = XmUNSPECIFIED_PIXEL;
    acc_color_rec.select_color = XmUNSPECIFIED_PIXEL;
    acc_color_rec.highlight_color = XmUNSPECIFIED_PIXEL;
    ret_val = _XmGetScaledPixmap(screen, NULL, image_name, &acc_color_rec,
				 DefaultDepthOfScreen(screen), False, 1);
    _XmProcessUnlock();
    _XmAppUnlock(app);

    return (ret_val);
}



/************************************************************************
*
*  XmeGetMask    
*
************************************************************************/
Pixmap 
XmeGetMask(
        Screen *screen,
        char *image_name)
{
    String 		mask_name;
    Pixmap		ret_val;
    XtAppContext app;
    size_t mask_name_size;

    if (screen == NULL || image_name == NULL)
	return XmUNSPECIFIED_PIXMAP;
    if (!XmMaskNameSize(image_name, &mask_name_size))
	return XmUNSPECIFIED_PIXMAP;

    app = XtDisplayToApplicationContext(DisplayOfScreen(screen));

    mask_name = XtMalloc((Cardinal)mask_name_size);
    if (mask_name == NULL)
	return XmUNSPECIFIED_PIXMAP;

    _XmAppLock(app);
    
    _XmProcessLock();
    _XmOSGenerateMaskName(image_name, mask_name);
    _XmProcessUnlock();

    ret_val = XmGetPixmapByDepth(screen, mask_name, 1, 0, 1);
    XtFree(mask_name);
    _XmAppUnlock(app);
    return ret_val;
}


/************************************************************************
 *
 *  XmDestroyPixmap
 *	Use the pixmap table (pixmap/screen) to find the pixmap,
 *      then remove it from both pixmap and pixmap_data tables.
 *
 ************************************************************************/
Boolean 
XmDestroyPixmap(
        Screen *screen,
        Pixmap pixmap )
{
   PixmapData pix_data, *pix_entry ;
   XtAppContext app;

   /*  Check for invalid conditions  */

   if (screen == NULL || pixmap == 0 || pixmap_set == NULL)
     return (False);

   app = XtDisplayToApplicationContext(
				DisplayOfScreen(screen));

   _XmAppLock(app);
   _XmProcessLock();

   pix_data.screen = screen ;
   pix_data.pixmap = pixmap ;

   if ((pix_entry = (PixmapData*) 
	_XmGetHashEntry(pixmap_set, (XmHashKey)&pix_data)) != NULL) {
      if (pix_entry->reference_count <= 0) {
	  _XmProcessUnlock();
	  _XmAppUnlock(app);
	  return False;
      }
      pix_entry->reference_count--;
      if (pix_entry->reference_count == 0) {
	  _XmRemoveHashEntry (pixmap_data_set, pix_entry);
	  _XmRemoveHashEntry (pixmap_set, pix_entry);
   	  if (0 != strcmp(pix_entry->image_name, DIRECT_PIXMAP_CACHED))
	    XFreePixmap(DisplayOfScreen(pix_entry->screen), pix_entry->pixmap);
	  XtFree(pix_entry->image_name);
	  XtFree((char*)pix_entry->acc_color);
	  if (pix_entry->pixels) {
	      ReleaseImagePixels(pix_entry->screen, pix_entry->pixels,
				 pix_entry->npixels);
	  }
	  XtFree((char*)pix_entry);
      }

      _XmProcessUnlock();
      _XmAppUnlock(app);
      return True;
  }

  _XmProcessUnlock(); 
  _XmAppUnlock(app);
  return False;
}


/* Compare two gc entry. Only care about the colors for depth 1 */
static Boolean 
CompareGCDatas (XmHashKey key_1, 
		XmHashKey key_2)
{
  GCData *data_1 = (GCData *) key_1;
  GCData *data_2 = (GCData *) key_2;

  return ((data_1->screen == data_2->screen) &&
	  (data_1->print_shell == data_2->print_shell) &&
	  (data_1->depth == data_2->depth) &&
	  ((data_1->image_depth != 1) ||
	   ((data_1->image_depth == 1) &&
	   (data_1->foreground == data_2->foreground) &&
	   (data_1->background == data_2->background)))) ;
}

/* Hash a GC entry. Only care about the colors for depth 1 */
static XmHashValue 
HashGCData (XmHashKey key)
{
  GCData *data = (GCData *) key;
  XmHashValue hv = 0 ;
 
  if (data->image_depth == 1)
    hv = data->foreground + data->background ;

  return (hv + (long)data->screen + (long)data->print_shell +
	  data->depth + data->image_depth);
}

/***
 * GetGCForPutImage
 * Maintain a cache of GC to use for transfering the XImage to
 * a Pixmap depending on the screen, depth, colors.
 ***/
static GC 
GetGCForPutImage(Screen *screen,
		 Widget print_shell,
		 XImage * image, 
		 Pixmap pixmap, 
		 int depth,
		 Pixel foreground,
		 Pixel background,
		 Boolean *caller_owns_gc)
{
    XGCValues gcValues;
    GCData gc_data, *gc_entry ;

    if (caller_owns_gc != NULL)
	*caller_owns_gc = False;

    /* There is a problem here: we cannot simply use the xmScreen object
     * to store the GCs, for the pixmap conversion can be called
     * at a time the xmScreen is not yet available: a conversion
     * for the first VendorShell on this screen, or a conversion for
     * the xmScreen or xmDisplay.
     * We can either arrange for delaying the conversion for those cases,
     * or not use the xmScreen here. Since we need to cache per depth
     * in addition to per screen, and since the Xt routine cannot be used
     * since there is no widget available, let's forget about using the
     * xmScreen object, do our own specialized caching.
     */

    /* There shouldn't be a lot of different GCs, one for
     * depth 1 to depth 1, foreground 1, back 0, one for 
     * depth N to N, fore/back don't matter and some for depth 1 to
     * depth N, where colors matter, for backgroundPixmap resources 
     * and regular use
     * of XmGetPixmapByDepth (with no -depth) issued from XBM file.
     */

    gcValues.foreground = foreground;
    gcValues.background = background;

    _XmProcessLock();
    if (gc_set == NULL)
	gc_set = _XmAllocHashTable (20, CompareGCDatas, HashGCData);
    if (gc_set == NULL)
      {
        _XmProcessUnlock();
        if (caller_owns_gc != NULL)
	    *caller_owns_gc = True;
        return XCreateGC(DisplayOfScreen(screen), pixmap,
                         GCForeground | GCBackground, &gcValues);
      }
    _XmProcessUnlock();
    
    /* set up an entry for search. We only care about the colors
       for image_depth 1 to pixmap depth (N or 1), since otherwise, N to N,
       the transfer is a plain copy */
    gc_data.screen = screen ;    
    gc_data.print_shell = print_shell ;
    gc_data.depth = depth ;    
    gc_data.image_depth = image->depth ;    
    gc_data.foreground = foreground ;
    gc_data.background = background ;

    /* look if we have a match and return it */
    _XmProcessLock();
    if ((gc_entry = (GCData*) 
	 _XmGetHashEntry(gc_set, (XmHashKey)&gc_data)) != NULL) {
	_XmProcessUnlock();
	return gc_entry->gc ;
    }
    _XmProcessUnlock();
	
    /* create a new GC, cache it and return it */
    gc_entry = XtNew(GCData);
    if (gc_entry == NULL)
      {
        if (caller_owns_gc != NULL)
	    *caller_owns_gc = True;
        return XCreateGC(DisplayOfScreen(screen), pixmap,
                         GCForeground | GCBackground, &gcValues);
      }
    gc_entry->screen = screen ;    
    gc_entry->print_shell = print_shell ;
    gc_entry->depth = depth ;    
    gc_entry->image_depth = image->depth ;    
    gc_entry->foreground = foreground ;
    gc_entry->background = background ;
    gc_entry->gc = XCreateGC (DisplayOfScreen(screen), pixmap,
			     GCForeground | GCBackground, &gcValues);
    if (gc_entry->gc == NULL)
      {
        XtFree((char *)gc_entry);
        return (GC) NULL;
      }

    _XmProcessLock();
    if (!_XmAddHashEntry(gc_set, (XmHashKey)gc_entry, (XtPointer)gc_entry))
      {
        _XmProcessUnlock();
        XFreeGC(DisplayOfScreen(screen), gc_entry->gc);
        XtFree((char *)gc_entry);
        return (GC) NULL;
      }
    _XmProcessUnlock();

    return gc_entry->gc ;
}
 

/************
 When a Screen is closed, or a PrintShell, we need to clean up the
 respective cache .
*****/


static Boolean 
CleanGCMapProc (XmHashKey key, /* unused */
		 XtPointer value,
		 XtPointer data)
{
  GCData *entry = (GCData *) value;
  CleanKey * ck = (CleanKey*) data ;

  /* shell should be NULL for non printing screen */
  if (entry->print_shell == ck->shell &&
      entry->screen == ck->screen)
    {
	  _XmProcessLock();
	  _XmRemoveHashEntry (gc_set, entry);
	  _XmProcessUnlock();
	  XFreeGC(DisplayOfScreen(entry->screen), entry->gc);
	  XtFree((char*) entry);
    }

  /* never return True: remove all gc keyed on this print shell */
  return False;
}

static Boolean 
CleanPixmapMapProc (XmHashKey key, /* unused */
		     XtPointer value,
		     XtPointer data)
{
  PixmapData *entry = (PixmapData *) value;
  CleanKey * ck = (CleanKey*) data ;

  /* shell should be NULL for non printing screen */
  if (entry->print_shell == ck->shell &&
      entry->screen == ck->screen)
    {
	/* this should take care of everything */
	XmDestroyPixmap(entry->screen, entry->pixmap);
    }

  /* never return True: remove all pixmap keyed on this print shell */
  return False;
}

#ifdef XP_DEBUG
static Boolean 
PrintPixmapMapProc (XmHashKey key, /* unused */
		    XtPointer value,
		    XtPointer data)
{
  PixmapData *entry = (PixmapData *) value;
  CleanKey * ck = (CleanKey*) data ;

  printf("entry->screen %d\n", entry->screen);
  printf("entry->print_shell %d\n", entry->print_shell);
  printf("entry->image_name %s\n", entry->image_name);
  printf("entry->pixmap %d\n\n", entry->pixmap);

  return False;
}

static Boolean 
PrintGCMapProc (XmHashKey key, /* unused */
		XtPointer value,
		XtPointer data)
{
  GCData *entry = (GCData *) value;
  CleanKey * ck = (CleanKey*) data ;

  printf("entry->screen %d\n", entry->screen);
  printf("entry->print_shell %d\n", entry->print_shell);
  printf("entry->gc %d\n\n", entry->gc);

  return False;
}
#endif
/******
 This is called from PrintShell.Destroy to invalidate the
 GCs and Pixmap used for a given print shell and from Screen.destroy,
 with a NULL shell, to remove the GCs and Pixmap used for a 
 given screen. So in the case of PrintShell, this is call twice
 in a row, where the second time (from Screen) takes care of
 the Screen (non print shell) specific pixmaps abd GCs.
*******/
void 
_XmCleanPixmapCache(Screen * screen, Widget shell)
{
    CleanKey ck ;

    ck.screen = screen;
    ck.shell = shell ;

    if (gc_set != NULL)
        _XmMapHashTable(gc_set, CleanGCMapProc, &ck);
    if (pixmap_set != NULL)
        _XmMapHashTable(pixmap_set, CleanPixmapMapProc, &ck);
}




/************************************************************************
*******************************************************************------
 
  Design notes:

  There is really two different caches here, an XImage cache and
  a Pixmap cache (+ a GC cache).

  The XImage cache is name based (hash key is image_name). No colors are
  supported, and therefore, it is only useful with depth 1 XImage.
  
  Program can directly install non-depth 1 XImage in this cache using 
  the public API (XmInstallImage), but that's not advised, for the
  non-depth 1 XImage resulting from XPM file have color variations
  for the same image_name. If you ask for image_name Foo, foreground 
  red and background blue, and a Foo has been installed in the image
  cache using different foreground & background, you don't want
  to get back this Foo, but a new instance of Foo, one that uses
  the red and blue colors.

  Also, the only access to the XImage reader (GetImage) is via the pixmap 
  API, so if a XPM with no colors sensitivity (no pertinent Motif
  symbolic colors) is read once, it will be found by the
  pixmap cache first, no need to cache it in the image cache.

  The Pixmap cache is name/depth/colors or pixmap based, depending
  which API is looking up.
  The old XmGetPixmapByDepth() is equivalent to asking for
  valid foreground/background and unspecified others, which
  means these additional colors (shadows, select) will not be
  used during Xpm reading for overriding symbolic colors.

  Motifnext adds printing support and clean cache support.

***/


static Boolean
XmRoundScaled(double value, unsigned int *rounded)
{
    double result;

    if (!isfinite(value) || value <= 0.0)
	return False;
    result = value + 0.5;
    if (!isfinite(result) || result > (double)UINT_MAX)
	return False;
    *rounded = result < 1.0 ? 1U : (unsigned int)result;
    return True;
}

static Boolean
XmScaledDimension(unsigned int source, double ratio, Dimension *dimension)
{
    unsigned int scaled;

    if (!XmRoundScaled((double)source * ratio, &scaled) ||
	scaled > (unsigned int)((Dimension)~0))
	return False;
    *dimension = (Dimension)scaled;
    return True;
}

static Boolean
XmImageArrayFits(size_t count, size_t element_size)
{
    return count <= SIZE_MAX / element_size;
}

static Boolean
XmMaskNameSize(String image_name, size_t *size)
{
    size_t length;

    if (image_name == NULL || size == NULL)
	return False;
    length = strlen(image_name);
    if (length > SIZE_MAX - 3)
	return False;
    *size = length + 3;
	if (*size > UINT_MAX)
	return False;
    return True;
}

typedef struct {
  unsigned int *x, *y;
  unsigned int *width, *height;
} Table;

void _XmPutScaledImage (    
    Display*		 display,
    Drawable		 d,
    GC			 gc,
    XImage*		 src_image,
    int			 src_x,
    int			 src_y,
    int			 dest_x,
    int			 dest_y,
    unsigned int	 src_width,
    unsigned int	 src_height, 
    unsigned int	 dest_width,  
    unsigned int	 dest_height) 
{
    XImage *dest_image;
    unsigned int x, y, min_y, max_y, src_max_x;
    unsigned int w, h, strip_height;
    unsigned int image_width, image_height;
    Table table = { NULL, NULL, NULL, NULL };
    Pixel pixel;
    double ratio_x, ratio_y;
    unsigned int scaled_row_bytes;
    size_t image_bytes;
    size_t table_x_bytes, table_y_bytes;
    size_t table_width_bytes, table_height_bytes;
    Bool fast8;
    int output_y;

    if (display == NULL || d == None || gc == NULL ||
	src_image == NULL || src_image->data == NULL ||
	src_image->width <= 0 || src_image->height <= 0 ||
	src_image->bytes_per_line <= 0 ||
	(size_t)src_image->height >
	 SIZE_MAX / (size_t)src_image->bytes_per_line ||
	src_width == 0 || src_height == 0 ||
	dest_width == 0 || dest_height == 0 || dest_width > INT_MAX ||
	dest_height > INT_MAX || src_x < 0 || src_y < 0 ||
	src_x > src_image->width || src_y > src_image->height ||
	src_width > (unsigned int)(src_image->width - src_x) ||
	src_height > (unsigned int)(src_image->height - src_y) ||
	!XmImageArrayFits((size_t)src_image->width + 1, sizeof(*table.x)) ||
	!XmImageArrayFits((size_t)src_image->height + 1, sizeof(*table.y)) ||
	!XmImageArrayFits((size_t)src_image->width, sizeof(*table.width)) ||
	!XmImageArrayFits((size_t)src_image->height, sizeof(*table.height)))
	return;

    image_width = (unsigned int)src_image->width;
    image_height = (unsigned int)src_image->height;

    /* The strip destination is formed as dest_y + min_y below.  Keep that
     * addition within the signed X11 coordinate type before doing any work. */
    if (dest_y > INT_MAX - (int)dest_height)
	return;

    if (dest_width == src_width && dest_height == src_height) {
	/* same for x and y, just send it out */
	XPutImage(display, d, gc, src_image, src_x, src_y, 
		  dest_x, dest_y, dest_width, dest_height); 
	return;
    }

    ratio_x = (double)dest_width / (double)src_width;
    ratio_y = (double)dest_height / (double)src_height;
 
#ifdef PRINTING_SUPPORTED
    unsigned int print_height;
    int xp_event, xp_error;
    /*
     * Check that we have uniform scaling, and that the print extension
     * exists.  We can't call XpGetContext first, because if the print
     * extension doesn't exist we'll get a warning to stderr.
     * It would be better if the print context was passed in directly, so
     * that we didn't spend a round trip when scaling for the video screen,
     * but it's just one round trip on just the first call per display, so
     * it's not that bad.
     */
	if (XmRoundScaled((double)src_height * ratio_x, &print_height) &&
	((print_height > dest_height) ?
	 (print_height - dest_height) : (dest_height - print_height)) <= 1 &&
	XpQueryExtension(display, &xp_event, &xp_error)) {
	/*
	 * We could be clever and try to get the print context first from
	 * the _XmPrintScreenToShellContext and then get the resolution from
	 * the print shell, but we don't have a Screen* to do the lookup.
	 */
	XPContext pcontext = XpGetContext(display);
	if (pcontext) {
	    int media_res;
	    media_res = atoi(XpGetOneAttribute(display, pcontext, XPDocAttr,
					       "default-printer-resolution"));
	    if (media_res) {
		int image_res = (double)media_res / ratio_x + 0.5;
		int prev_res;
		if (XpSetImageResolution(display, pcontext,
					 image_res, &prev_res)) {
		    XPutImage(display, d, gc, src_image, src_x, src_y,
			      dest_x, dest_y, src_width, src_height);
		    XpSetImageResolution(display, pcontext, prev_res, NULL);
		    return;
		}
	    }
	}
    }
#endif /* PRINTING_SUPPORTED */

    if (!XmRoundScaled(ratio_x * src_image->bytes_per_line,
		       &scaled_row_bytes) || scaled_row_bytes == 0)
	return;
    src_max_x = (unsigned int)src_x + src_width;

    strip_height = 65536U / scaled_row_bytes;
    if (strip_height == 0)
	strip_height = 1;
    if (strip_height > dest_height)
	strip_height = dest_height;

    if (!XmRoundScaled(ratio_y, &h) ||
	h > UINT_MAX - strip_height)
	return;
    h += strip_height;
    dest_image = XCreateImage(display,
			      DefaultVisualOfScreen(
					     DefaultScreenOfDisplay(display)),
			      src_image->depth, src_image->format, 
			      0, NULL,
			      dest_width, h,
			      src_image->bitmap_pad, 0);
    if (dest_image == NULL || dest_image->bytes_per_line <= 0 ||
	    (size_t)h > SIZE_MAX / (size_t)dest_image->bytes_per_line) {
	if (dest_image != NULL)
	    XDestroyImage(dest_image);
	return;
    }
    image_bytes = (size_t)dest_image->bytes_per_line * h;
    if (image_bytes > UINT_MAX) {
	XDestroyImage(dest_image);
	return;
    }
    dest_image->data = XtMalloc((Cardinal)image_bytes);
    if (dest_image->data == NULL) {
	XDestroyImage(dest_image);
	return;
    }
    fast8 = (src_image->depth == 8 && src_image->bits_per_pixel == 8 &&
	     dest_image->bits_per_pixel == 8 && src_image->format == ZPixmap);

	if ((size_t)src_image->width >
	    SIZE_MAX / sizeof(*table.x) - 1 ||
	    (size_t)src_image->width >
	    UINT_MAX / sizeof(*table.x) - 1 ||
	    (size_t)src_image->height >
	    SIZE_MAX / sizeof(*table.y) - 1 ||
	    (size_t)src_image->height >
	    UINT_MAX / sizeof(*table.y) - 1 ||
	    (size_t)src_image->width > SIZE_MAX / sizeof(*table.width) ||
	    (size_t)src_image->width > UINT_MAX / sizeof(*table.width) ||
	    (size_t)src_image->height > SIZE_MAX / sizeof(*table.height) ||
	    (size_t)src_image->height > UINT_MAX / sizeof(*table.height))
	    goto cleanup;
	table_x_bytes = sizeof(*table.x) * ((size_t)src_image->width + 1);
	table_y_bytes = sizeof(*table.y) * ((size_t)src_image->height + 1);
	table_width_bytes = sizeof(*table.width) * (size_t)src_image->width;
	table_height_bytes = sizeof(*table.height) * (size_t)src_image->height;
	table.x = (unsigned int *) XtMalloc((Cardinal)table_x_bytes);
	table.y = (unsigned int *) XtMalloc((Cardinal)table_y_bytes);
	table.width = (unsigned int *) XtMalloc((Cardinal)table_width_bytes);
	table.height = (unsigned int *) XtMalloc((Cardinal)table_height_bytes);
	if (table.x == NULL || table.y == NULL ||
	    table.width == NULL || table.height == NULL)
	    goto cleanup;
    
    table.x[0] = 0;
    for (x = 1; x <= image_width; x++) {
	if (!XmRoundScaled(ratio_x * x, &table.x[x]))
	    goto cleanup;
	table.width[x - 1] = table.x[x] - table.x[x - 1];
    }

    table.y[0] = 0;
    for (y = 1; y <= image_height; y++) {
	if (!XmRoundScaled(ratio_y * y, &table.y[y]))
	    goto cleanup;
	table.height[y - 1] = table.y[y] - table.y[y - 1];
    }

    for (min_y = table.y[src_y]; min_y < dest_height; min_y = table.y[y]) {
	max_y = min_y + strip_height;
	if (max_y > dest_height) {
	    strip_height = dest_height - min_y;
	    max_y = dest_height;
	}
	for (y = (unsigned int)src_y; table.y[y] < max_y; y++) {
	    if (table.y[y] < min_y)
		continue;
	    if (fast8) {
		for (x = (unsigned int)src_x; x < src_max_x; x++) {
		    pixel = ((unsigned char *)src_image->data)
			[(size_t)y * (size_t)src_image->bytes_per_line +
			 (size_t)x];
		    for (h = 0; h < table.height[y]; h++) {
			memset(dest_image->data +
		       (size_t)(table.y[y] - min_y + h) *
		       (size_t)dest_image->bytes_per_line + (size_t)table.x[x],
			       (int)(unsigned char)pixel, table.width[x]);
		    }
		}
	    } else {
		for (x = (unsigned int)src_x; x < src_max_x; x++) {
		    pixel = (Pixel)XGetPixel(src_image, (int)x, (int)y);
		    for (h = 0; h < table.height[y]; h++) {
			for (w = 0; w < table.width[x]; w++)
			    XPutPixel(dest_image,
				      (int)(table.x[x] + w),
				      (int)(table.y[y] + h - min_y),
				      pixel);
		    }
		}
	    }
	}
	if ((long long)dest_y + (long long)min_y > INT_MAX ||
	    (long long)dest_y + (long long)min_y < INT_MIN)
	    goto cleanup;
	output_y = (int)((long long)dest_y + (long long)min_y);
	XPutImage(display, d, gc, dest_image, dest_x, 0,
		  dest_x, output_y, dest_width, table.y[y] - min_y);
	if (y >= image_height)
	    break;
    }
    
cleanup:
    XtFree((char *)table.x);
    XtFree((char *)table.y);
    XtFree((char *)table.width);
    XtFree((char *)table.height);

    XDestroyImage(dest_image);
}



/*** COLOR CACHE NOW ***/


static Boolean
GetCacheColorByName( Display *display, Colormap colormap,
		     char *colorname, XColor *xcolor)
{
    static Boolean firstTime = True;
    XrmQuark colorname_q;
    int i;
    CachedColor *color;

    if (!colorname)
	return False;

    if ( firstTime )
    {
       colorCacheList.numEntries = colorCacheList.maxEntries = 0;
       colorCacheList.cache = NULL;
       firstTime = False; 
       return False;
    }

    colorname_q = XrmStringToQuark(colorname);
	
    for (i = 0, color = colorCacheList.cache; i < colorCacheList.numEntries;
	 i++, color++)
    {
	if (color->colorname == colorname_q &&
            color->colormap == colormap &&
            color->display == display)
        {
	   xcolor->pixel = color->pixel;
	   color->num_cached++;
           return True;
        }
    }
    return False;
}

static Boolean
GetCacheColorByRGB( Display *display, Colormap colormap, XColor *xcolor)
{
    int i;
    CachedColor *color;

    for (i = 0, color = colorCacheList.cache; i < colorCacheList.numEntries;
	 i++, color++)
    {
	if (color->colormap == colormap && color->display == display &&
	    color->red == xcolor->red &&
	    color->green == xcolor->green &&
	    color->blue == xcolor->blue)
        {
	   xcolor->pixel = color->pixel;
	   color->num_cached++;
           return True;
        }
    }
    return False;
}

static Boolean
FreeCacheColor( Display *display, Colormap colormap, Pixel pixel)
{
    int i;
    CachedColor *color;

    for (i = 0, color = colorCacheList.cache; i < colorCacheList.numEntries;
	 i++, color++)
    {
	if (color->colormap == colormap && color->display == display &&
	    color->pixel == pixel)
        {
	    if (color->num_cached == 0)
		return False;
	    color->num_cached--;
	    if (color->num_cached == 0) {
		int j;
		for (j = i + 1; j < colorCacheList.numEntries; i++, j++)
		    colorCacheList.cache[i] = colorCacheList.cache[j];
		colorCacheList.numEntries--;
		XFreeColors(display, colormap, &pixel, 1, 0);
		return True;
	    }
	}
    }
    return False;
}

static Boolean
CacheColorPixel( Display *display, Colormap colormap,
                 char *colorname, XColor *xcolor)
{
    int numEntries = colorCacheList.numEntries;

    if (numEntries == colorCacheList.maxEntries)
    {
        CachedColor *new_cache;
        size_t cache_bytes;
        if (colorCacheList.maxEntries > INT_MAX - 25)
	    return False;
        colorCacheList.maxEntries += 25;
	if ((size_t)colorCacheList.maxEntries >
	    SIZE_MAX / sizeof(*colorCacheList.cache) ||
	    (size_t)colorCacheList.maxEntries >
	    UINT_MAX / sizeof(*colorCacheList.cache))
	    return False;
	cache_bytes = (size_t)colorCacheList.maxEntries *
	              sizeof(*colorCacheList.cache);
	new_cache = (CachedColor *)
		    XtRealloc((char *)colorCacheList.cache,
			       (Cardinal)cache_bytes);
	if (new_cache == NULL)
	    return False;
	colorCacheList.cache = new_cache;
    }

    colorCacheList.cache[numEntries].display = display;
    colorCacheList.cache[numEntries].colormap = colormap;
    colorCacheList.cache[numEntries].colorname =
	colorname ? XrmStringToQuark(colorname) : NULLQUARK;
    
    colorCacheList.cache[numEntries].red = xcolor->red;
    colorCacheList.cache[numEntries].green = xcolor->green;
    colorCacheList.cache[numEntries].blue = xcolor->blue;
    colorCacheList.cache[numEntries].pixel = xcolor->pixel;
    colorCacheList.cache[numEntries].num_cached = 1;
                                                      
    colorCacheList.numEntries++;
    return True;
}


static int
GetCacheColor(Display *display, Colormap colormap,
	      char *colorname, XColor *xcolor, void *closure)
{
    int status;
    if (colorname) {
	/* look for the colorname in the cache */
	if (!GetCacheColorByName(display, colormap, colorname, xcolor)) {
	    /* when not found parse and alloc color */
	    if (!XParseColor(display, colormap, colorname, xcolor))
		return -1;
	    if (!GetCacheColorByRGB(display, colormap, xcolor)) {
		status = XAllocColor(display, colormap, xcolor);
		if (status &&
		    !CacheColorPixel(display, colormap, colorname, xcolor)) {
		    XFreeColors(display, colormap, &xcolor->pixel, 1, 0);
		    status = 0;
		}
	    } else
		status = 1;
	} else
	    status = 1;
    } else {
	if (!GetCacheColorByRGB(display, colormap, xcolor)) {
	    status = XAllocColor(display, colormap, xcolor);
	    if (status &&
		!CacheColorPixel(display, colormap, colorname, xcolor)) {
		XFreeColors(display, colormap, &xcolor->pixel, 1, 0);
		status = 0;
	    }
	} else
	    status = 1;
    }

    return status != 0 ? 1 : 0;
}

static int
FreeCacheColors(Display *display, Colormap colormap,
		Pixel *pixels, int n, void *closure)
{
    int i, status;
    for (i = 0; i < n; i++, pixels++)
	status = FreeCacheColor(display, colormap, *pixels);
    return 0;
}

static void
ReleaseImagePixels(Screen *screen, Pixel *pixels, int npixels)
{
    if (pixels == NULL)
	return;
    FreeCacheColors(DisplayOfScreen(screen),
			DefaultColormapOfScreen(screen), pixels, npixels, NULL);
    XmeXpmFree(pixels);
}
