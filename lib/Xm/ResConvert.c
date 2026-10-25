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
static char rcsid[] = "$TOG: ResConvert.c /main/29 1999/05/18 19:19:39 mgreess $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#define X_INCLUDE_STRING_H
#define XOS_USE_XT_LOCKING
#ifndef NEED_XOS_R_H
#include <X11/Xos_r.h>
#else
#include <Xm/Xmos_r.h>
#endif


#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#endif
#include <ctype.h>
#include <Xm/SpecRenderT.h>
#include <Xm/TraitP.h>
#include <Xm/XmosP.h>
#include "MessagesI.h"
#include "RepTypeI.h"
#include "ResConverI.h"
#include "ResIndI.h"
#include "XmI.h"
#include "XmRenderTI.h"


#define MSG2	_XmMMsgResConvert_0001
#define MSG3    _XmMMsgResConvert_0002
#define MSG4    _XmMMsgResConvert_0003
#define MSG6    _XmMMsgResConvert_0005
#define MSG7    _XmMMsgResConvert_0006
#define MSG12   _XmMMsgResConvert_0011



/********    Static Function Declarations    ********/

static Boolean StringToEntity( 
                        Display *disp,
                        XrmValue *args,
                        Cardinal *n_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToWidget( 
                        Display *disp,
                        XrmValue *args,
                        Cardinal *n_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToWindow( 
                        Display *disp,
                        XrmValue *args,
                        Cardinal *n_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToChar( 
                        Display *disp,
                        XrmValue *args,
                        Cardinal *n_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToKeySym( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static void CvtStringToXmStringDestroy( 
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer converter_data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToXmString( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static void CvtStringToXmFontListDestroy( 
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer converter_data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToXmFontList( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToButtonFontList( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToLabelFontList( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToTextFontList( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean GetNextFontListEntry( 
                        char **s,
                        char **fontNameRes,
                        char **fontTagRes,
                        XmFontType *fontTypeRes,
                        char *delim) ;
static Boolean GetFontName( 
                        char **s,
                        char **name,
                        char *delim) ;
static Boolean GetFontTag( 
                        char **s,
                        char **tag,
                        char *delim) ;
static Boolean GetNextXmString( 
                        char **s,
                        char **cs,
                        Boolean *after_delimiter) ;
static Boolean CvtStringToXmStringTable( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val,
                        XtPointer *data) ;
static void XmStringCvtDestroy( 
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToStringTable( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val,
                        XtPointer *data) ;
static void StringCvtDestroy( 
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToCardinalList(
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val,
                        XtPointer *data) ;
static void CardinalListCvtDestroy(
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToHorizontalPosition( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToHorizontalDimension( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToVerticalPosition( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToVerticalDimension( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static void ConvertStringToButtonTypeDestroy( 
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer converter_data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean ConvertStringToButtonType( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static void CvtStringToKeySymTableDestroy( 
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer converter_data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToKeySymTable( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static void CvtStringToCharSetTableDestroy( 
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer converter_data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToCharSetTable( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToBooleanDimension( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToAtomList( 
                        Display *dpy,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static void SimpleDestructor( 
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean OneOf( 
#if NeedWidePrototypes
                        int c,
#else
                        char c,
#endif /* NeedWidePrototypes */
                        char *set) ;
static char * GetNextToken( 
                        char *src,
                        char *delim,
			char **context) ;
static Boolean CvtStringToCardinal( 
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToTextPosition(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToTopItemPosition(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean isInteger(
                        String string,
                        int *value) ;
static Boolean CvtStringToRenditionPixel(Display *disp,
					    XrmValuePtr args,
					    Cardinal *num_args,
					    XrmValue *from_val,
					    XrmValue *to_val,
					    XtPointer *converter_data); 
static Boolean CvtPixelToRenditionPixel(Display *disp,
					    XrmValuePtr args,
					    Cardinal *num_args,
					    XrmValue *from_val,
					    XrmValue *to_val,
					    XtPointer *converter_data); 
static Boolean CvtStringToSelectColor(Display *disp,
					 XrmValuePtr args,
					 Cardinal *num_args,
					 XrmValue *from_val,
					 XrmValue *to_val,
					 XtPointer *converter_data); 
static void CvtStringToXmTabListDestroy(XtAppContext app,
					   XrmValue *to,
					   XtPointer converter_data,
					   XrmValue *args,
					   Cardinal *num_args); 
static Boolean GetNextTab(char **s,
			  float *value,
			  char *unitType,
			  XmOffsetModel *offsetModel); 
static Boolean CvtStringToXmTabList(Display *dpy,
				       XrmValue *args,
				       Cardinal *num_args,
				       XrmValue *from,
				       XrmValue *to,
				       XtPointer *converter_data); 
static void CvtStringToXmRenderTableDestroy(XtAppContext app,
					       XrmValue *to,
					       XtPointer converter_data,
					       XrmValue *args,
					       Cardinal *num_args); 
static Boolean CvtStringToRenderTable(Display *dpy,
					 XrmValue *args,
					 Cardinal *num_args,
					 XrmValue *from,
					 XrmValue *to,
					 XtPointer *converter_data); 
static Boolean CvtStringToButtonRenderTable(Display *dpy,
					       XrmValue *args,
					       Cardinal *num_args,
					       XrmValue *from,
					       XrmValue *to,
					       XtPointer *converter_data); 
static Boolean CvtStringToLabelRenderTable(Display *dpy,
					      XrmValue *args,
					      Cardinal *num_args,
					      XrmValue *from,
					      XrmValue *to,
					      XtPointer *converter_data); 
static Boolean CvtStringToTextRenderTable(Display *dpy,
					     XrmValue *args,
					     Cardinal *num_args,
					     XrmValue *from,
					     XrmValue *to,
					     XtPointer *converter_data); 

static void _XmGetDisplayArg(Widget widget,
					Cardinal *size,
					XrmValue *value);


/********    End Static Function Declarations    ********/
  

static XtConvertArgRec selfConvertArgs[] = {
    { XtBaseOffset, (XtPointer) 0, sizeof(int) }
};

static XtConvertArgRec  displayConvertArg[] = {
    {XtProcedureArg, (XtPointer)_XmGetDisplayArg, 0},
};

/* Motif widget set version number.  Accessable by applications via Xm.h. */

externaldef(xmuseversion) int xmUseVersion = XmVersion;


/************************************************************************
 *
 *  _XmGetDisplayArg
 *
 * Function used to allow Fonts to be per display
 *
 ************************************************************************/
static void _XmGetDisplayArg(widget, size, value)
    Widget widget;
    Cardinal *size;
    XrmValue* value;
{
    if (widget == NULL)
            XtErrorMsg("missingWidget", "_XmGetDisplayArg", "XtToolkitError",
                 "_XmGetDisplayArg called without a widget to reference",
                 (String*)NULL, (Cardinal*)NULL );
        /* can't return any useful Display and caller will de-ref NULL,
           so aborting is the only useful option */
 
    value->size = sizeof(Display*);
    value->addr = (XPointer)&DisplayOfScreen(XtScreenOfObject(widget));
}


/************************************************************************
 *
 *  _XmRegisterConverters
 *	Register all of the Xm resource type converters.  Retain a
 *	flag indicating whether the converters have already been
 *	registered.
 *
 ************************************************************************/
void
_XmRegisterConverters( void )
{
    static Boolean registered = False ;

    _XmProcessLock();
    if(    !registered    )
    {
        _XmRepTypeInstallConverters() ;

        XtSetTypeConverter( XmRString, XmRWidget, CvtStringToWidget, 
                            selfConvertArgs, XtNumber(selfConvertArgs),
                            XtCacheNone, (XtDestructor) NULL) ;
        XtSetTypeConverter( XmRString, XmRWindow, CvtStringToWindow, 
                            selfConvertArgs, XtNumber(selfConvertArgs),
                            XtCacheNone, (XtDestructor) NULL) ;
        XtSetTypeConverter( XmRString, XmRChar, CvtStringToChar, NULL, 0,
			   XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRFontList, CvtStringToXmFontList,
                            displayConvertArg,  XtNumber(displayConvertArg),
                            XtCacheByDisplay, CvtStringToXmFontListDestroy);
        XtSetTypeConverter( XmRString, XmRXmString, CvtStringToXmString,
			    NULL, 0, (XtCacheNone | XtCacheRefCount), 
			    CvtStringToXmStringDestroy ) ;
        XtSetTypeConverter( XmRString, XmRKeySym, CvtStringToKeySym,
			   NULL, 0, XtCacheNone, NULL) ;

        XtSetTypeConverter( XmRString, XmRHorizontalPosition,
                           CvtStringToHorizontalPosition, selfConvertArgs,
                             XtNumber( selfConvertArgs), XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRHorizontalDimension,
                          CvtStringToHorizontalDimension, selfConvertArgs,
                             XtNumber( selfConvertArgs), XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRVerticalPosition,
                             CvtStringToVerticalPosition, selfConvertArgs,
                             XtNumber( selfConvertArgs), XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRVerticalDimension,
                            CvtStringToVerticalDimension, selfConvertArgs,
                             XtNumber( selfConvertArgs), XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRBooleanDimension, 
                             CvtStringToBooleanDimension, selfConvertArgs,
                             XtNumber( selfConvertArgs), XtCacheNone, NULL) ;

        XtSetTypeConverter( XmRCompoundText, XmRXmString, XmCvtTextToXmString,
			   NULL, 0, XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRXmString, XmRCompoundText, XmCvtXmStringToText,
                           NULL, 0, XtCacheNone, NULL) ;

        XtSetTypeConverter( XmRString, XmRCharSetTable,
			   CvtStringToCharSetTable, NULL, 0, XtCacheNone,
			   CvtStringToCharSetTableDestroy) ;
        XtSetTypeConverter( XmRString, XmRKeySymTable,
			   CvtStringToKeySymTable, NULL, 0, XtCacheNone,
			   CvtStringToKeySymTableDestroy) ;
        XtSetTypeConverter( XmRString, XmRButtonType, 
			   ConvertStringToButtonType, NULL, 0, XtCacheNone, 
			   ConvertStringToButtonTypeDestroy) ;
        XtSetTypeConverter( XmRString, XmRXmStringTable, 
			   CvtStringToXmStringTable, NULL, 0,
			   (XtCacheNone | XtCacheRefCount), 
			   XmStringCvtDestroy) ;
        XtSetTypeConverter (XmRString, XmRStringTable,
			    CvtStringToStringTable, NULL, 0,
			    (XtCacheNone | XtCacheRefCount), 
			    StringCvtDestroy) ;
	XtSetTypeConverter( XmRString, XmRCardinalList,
                        CvtStringToCardinalList, NULL, 0,
                        XtCacheNone, CardinalListCvtDestroy) ;
        XtSetTypeConverter( XmRString, XmRAtomList, 
                    CvtStringToAtomList, NULL, 0,
                      (XtCacheNone | XtCacheRefCount), SimpleDestructor) ;
        XtSetTypeConverter( XmRString, XmRCardinal,
                            CvtStringToCardinal, NULL, 0,
			    XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRTextPosition,
                            CvtStringToTextPosition, NULL, 0,
                            XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRTopItemPosition,
                             CvtStringToTopItemPosition, NULL, 0,
                             XtCacheNone, NULL) ;
	XtSetTypeConverter(XmRString, XmRRenditionPixel,
			   CvtStringToRenditionPixel,
			   (XmConst XtConvertArgList)colorConvertArgs, 2,
			   XtCacheByDisplay, NULL);

	/* also set a converter from Pixel to RenditionPixel so that
	   the ColorObject setting as Pixel directly in the resource
	   Database be taken for rendition background and foreground */
	XtSetTypeConverter(XmRPixel, XmRRenditionPixel,
			   CvtPixelToRenditionPixel, NULL, 0,
			   XtCacheByDisplay, NULL);

	XtSetTypeConverter(XmRString, XmRSelectColor,
			   CvtStringToSelectColor,
			   (XmConst XtConvertArgList)colorConvertArgs, 2,
			   XtCacheByDisplay, NULL);

	XtSetTypeConverter(XmRString, XmRTabList,
			   CvtStringToXmTabList, NULL, 0,
			   (XtCacheAll | XtCacheRefCount), 
			   CvtStringToXmTabListDestroy); 
        XtSetTypeConverter(XmRString, XmRRenderTable,
			   CvtStringToRenderTable,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmRenderTableDestroy);
        XtSetTypeConverter(XmRString, XmRButtonRenderTable,
			   CvtStringToButtonRenderTable,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmRenderTableDestroy);
        XtSetTypeConverter(XmRString, XmRLabelRenderTable,
			   CvtStringToLabelRenderTable,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmRenderTableDestroy);
        XtSetTypeConverter(XmRString, XmRTextRenderTable,
			   CvtStringToTextRenderTable,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmRenderTableDestroy);

        XtSetTypeConverter(XmRString, XmRButtonFontList,
			   CvtStringToButtonFontList,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmFontListDestroy);
        XtSetTypeConverter(XmRString, XmRLabelFontList,
			   CvtStringToLabelFontList,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmFontListDestroy);
        XtSetTypeConverter(XmRString, XmRTextFontList,
			   CvtStringToTextFontList,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmFontListDestroy);
	
        registered = True;
        }
    _XmProcessUnlock();
    return ;
}



/************************************************************************
 *
 *  XmeNamesAreEqual
 *	Compare two strings and return true if equal.
 *	The comparison is on lower cased strings.  It is the callers
 *	responsibility to ensure that test_str is already lower cased.
 *
 ************************************************************************/
Boolean 
XmeNamesAreEqual(
        register char *in_str,
        register char *test_str )
{
        register char i ;

    if (in_str == NULL || test_str == NULL)
        return False;

    if(    ((in_str[0] == 'X') || (in_str[0] == 'x'))
        && ((in_str[1] == 'M') || (in_str[1] == 'm'))    )
    {   
        in_str +=2;
        } 
    do
    {
 /*
  * Fix for 5330 - For OS compatibility with old operating systems, always
  *                check a character with isupper before using tolower on it.
  */
        if (isupper((unsigned char)*in_str))
            i = (char) tolower((unsigned char) *in_str) ;
        else
            i = *in_str;
        in_str++;

        if(    i != *test_str++    )
        {   
            return( False) ;
            } 
    }while(    i    ) ;

    return( True) ;
    }



/************************************************************************
 *
 *  StringToEntity
 *    Allow widget or window to be specified by name
 *
 ************************************************************************/
/*ARGSUSED*/
static Boolean
StringToEntity(
        Display *disp,
        XrmValue *args,
        Cardinal *n_args,
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    Widget child;
    Widget widget;
    static Widget  itsChild;
    Boolean        success;

    if (args == NULL || args[0].addr == NULL || n_args == NULL ||
	*n_args != 1 || from == NULL || from->addr == NULL || to == NULL)
      return False;

    widget = *(Widget*) args[0].addr ;
    if (widget == NULL)
      return False;

    /* handle the XmSELF case */
    if (XmeNamesAreEqual ((String) from->addr, "self"))
	child  = widget ;
    else {
	child  = XtNameToWidget(XtParent(widget), (String)from->addr);
    }

    success   = !( child == NULL );

    if ( success ) 
    { 
        if (to->addr == NULL) {
           itsChild = child;  
           to->addr = (XPointer) &itsChild;
	}

        else if (to->size < sizeof(Widget))  
            success  = FALSE;

        else
            *(Widget*) to->addr = child;

        to->size = sizeof(Widget);    
    } 
    else
        XtDisplayStringConversionWarning(disp, from->addr, "Widget");     

    return ( success );
}

/************************************************************************
 *
 *  CvtStringToWidget
 *    Allow widget to be specified by name
 *
 ************************************************************************/
static Boolean
CvtStringToWidget(
        Display *disp,
        XrmValue *args,
        Cardinal *n_args,
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data)
{
    return (StringToEntity( disp, args, n_args, from, to, converter_data ) );
}

/************************************************************************
 *
 *  CvtStringToWindow
 *    Allow widget(Window) to be specified by name
 *
 ************************************************************************/
static Boolean
CvtStringToWindow(
        Display *disp,
        XrmValue *args,
        Cardinal *n_args,
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data)
{
    return (StringToEntity( disp, args, n_args, from, to, converter_data ) );
}



/************************************************************************
 *
 *  CvtStringToChar
 *	Convert string to a single character (a mnemonic)
 *
 ************************************************************************/
/*ARGSUSED*/
static Boolean
CvtStringToChar(
        Display *disp,		/* unused */
        XrmValue *args,		/* unused */
        Cardinal *n_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
   unsigned char in_char;

   if (from == NULL || from->addr == NULL || from->size < sizeof(unsigned char) ||
       to == NULL)
      return False;
   in_char = *((unsigned char *) (from->addr)) ;

   _XM_CONVERTER_DONE( to, unsigned char, in_char, ; )
   }


/************************************************************************
 *
 *   CvtStringToKeySym
 *	Convert a string to a KeySym
 *
 ************************************************************************/
/*ARGSUSED*/
static Boolean
CvtStringToKeySym(
        Display *display,	
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    if (from == NULL || from->addr == NULL || to == NULL)
        return False;
    KeySym tmpKS = XStringToKeysym( (char *) (from->addr)) ;

    if(    tmpKS != NoSymbol    )
    {   
        _XM_CONVERTER_DONE( to, KeySym, tmpKS, ; )
        } 
    XtDisplayStringConversionWarning(display, (char *) from->addr, XmRKeySym) ;

    return( FALSE) ;
    }

/*ARGSUSED*/
static void
CvtStringToXmStringDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer converter_data, /* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{   
    if (to == NULL || to->addr == NULL)
        return;
    XmStringFree( *((XmString *) to->addr)) ;
    return ;
    } 

/************************************************************************
 *
 *  CvtStringToXmString
 *	Convert an ASCII string to a XmString.
 *
 ************************************************************************/
/*ARGSUSED*/
static Boolean
CvtStringToXmString(
        Display *display,	
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
        XmString tmpStr ;

    if (from == NULL || from->addr == NULL || to == NULL)
        return False;

    if(    from->addr    )
    {   
        tmpStr = XmStringGenerate((char *)from->addr,
				  XmFONTLIST_DEFAULT_TAG,
				  XmCHARSET_TEXT, NULL);
        if(    tmpStr    )
        {   
            _XM_CONVERTER_DONE( to, XmString, tmpStr, XmStringFree( tmpStr) ; )
            } 
        } 
    XtDisplayStringConversionWarning(display, ((char *) from->addr), 
				     XmRXmString) ;

    return( FALSE) ;
    }

/*ARGSUSED*/
static void
CvtStringToXmFontListDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer converter_data, /* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{   
    if (to == NULL || to->addr == NULL)
        return;
    XmFontListFree( *((XmFontList *) to->addr)) ;

    return ;
    }

/************************************************************************
 *
 *  CvtStringToXmFontList
 *	Convert a string to a fontlist.  This is in the form :
 *  
 *  <XmFontList>	::=	<fontlistentry> { ',' <fontlistentry> }
 *  
 *  <fontlistentry>	::=	<fontset> | <font>
 *  
 *  <fontset>		::=	<fontname> { ';' <fontname> } ':' [ <tag> ]
 *  
 *  <font>		::=	<fontname> [ '=' <tag> ]
 *  
 *  <fontname>		::=	<XLFD String>
 *  
 *  <tag>		::=	<characters from ISO646IRV except newline>
 *  
 *  
 *  Additional syntax is allowed for compatibility with Xm1.1:
 *  
 *  1. The fontlistentries may be separated by whitespace, rather than ','.
 *  2. Empty fontlistentries are ignored.
 *
 ************************************************************************/

/*ARGSUSED*/
static Boolean
CvtStringToXmFontList(
        Display *dpy,		
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
  Boolean got_it = FALSE;
  char *s;
  char *newString;
  char *sPtr;
  char *fontName;
  char *fontTag;
  XmFontType fontType;
  char delim;
  XmFontListEntry fontListEntry;
  XmFontList      fontList = NULL;

  if (from == NULL || from->addr == NULL || to == NULL)
    return FALSE;
  
  if (from->addr)
    {   
      /* Copy the input string. */
      s = (char *) from->addr;
      sPtr = newString = XtNewString(s);
      if (newString == NULL)
	return FALSE;

      /* Get the first fontlist entries. */
      if (!GetNextFontListEntry(&sPtr, &fontName, &fontTag,
				&fontType, &delim))
	{
	  XtFree(newString);
	  s = (char *) XmDEFAULT_FONT;
	  sPtr = newString = XtNewString(s);
	  if (newString == NULL)
	    return FALSE;


	  if (!GetNextFontListEntry(&sPtr, &fontName, &fontTag,
				    &fontType, &delim))
	    {
	      XtFree(newString);
	      XmeWarning(NULL, MSG2);
	      exit(1);
	    }
	}

      /* Parse additional font list entries. */
      do {
	if (*fontName) 
	  {
	    fontListEntry = XmFontListEntryLoad(dpy, fontName,
						fontType, fontTag);
	    if (fontListEntry != NULL)
	      {
		got_it = TRUE;
		fontList = XmFontListAppendEntry(fontList, fontListEntry);
		XmFontListEntryFree(&fontListEntry);
	      } 
	    else
	      XtDisplayStringConversionWarning(dpy, fontName, XmRFontList);
	  }
      } while ((delim == ',') && *++sPtr &&
	       GetNextFontListEntry(&sPtr, &fontName, &fontTag,
				    &fontType, &delim));
      XtFree(newString);
    }

  if (got_it)
    {
      _XM_CONVERTER_DONE(to, XmFontList, fontList, XmFontListFree(fontList);)
    }

  XtDisplayStringConversionWarning(dpy, (char *) from->addr, XmRFontList);
  return FALSE;
}


/*ARGSUSED*/
static Boolean
CvtStringToButtonFontList(Display *dpy,
				XrmValue *args,
				Cardinal *num_args, /* unused */
				XrmValue *from,
				XrmValue *to,
				XtPointer *converter_data) /* unused */
{

  return(CvtStringToXmFontList(dpy, args, num_args, from, to, 
			       converter_data));
}

/*ARGSUSED*/
static Boolean
CvtStringToLabelFontList(Display *dpy,
			       XrmValue *args,
			       Cardinal *num_args, /* unused */
			       XrmValue *from,
			       XrmValue *to,
			       XtPointer *converter_data) /* unused */
{

  return(CvtStringToXmFontList(dpy, args, num_args, from, to, 
			       converter_data));
}

/*ARGSUSED*/
static Boolean
CvtStringToTextFontList(Display *dpy,
			       XrmValue *args,
			       Cardinal *num_args, /* unused */
			       XrmValue *from,
			       XrmValue *to,
			       XtPointer *converter_data) /* unused */
{
  return(CvtStringToXmFontList(dpy, args, num_args, from, to, 
			       converter_data));
}

/************************************************************************
 *
 *  GetNextFontListEntry
 *  
 ************************************************************************/
static Boolean 
GetNextFontListEntry (
    char **s ,
    char **fontNameRes ,
    char **fontTagRes ,
    XmFontType *fontTypeRes ,
    char *delim )
{
    char *fontName;
    char *fontTag = XmFONTLIST_DEFAULT_TAG;
    char *fontPtr;
    String params[2];
    Cardinal num_params;

    *fontTypeRes = XmFONT_IS_FONT;

    /*
     * Parse the fontname or baselist.
     */

    if (!GetFontName(s, &fontName, delim))
    {
	return (FALSE);
    }

    while (*delim == ';')
    {
        *fontTypeRes = XmFONT_IS_FONTSET;

        **s = ',';
        (*s)++;

        if (!GetFontName(s, &fontPtr, delim))
        {
	    return (FALSE);
        }
    }

    /*
     * Parse the fontsettag or fonttag.
     */

    if (*delim == ':')
    {
        *fontTypeRes = XmFONT_IS_FONTSET;

	(*s)++;
        if (!GetFontTag(s, &fontTag, delim))
        {
	    fontTag = XmFONTLIST_DEFAULT_TAG;
        }
    }
    else
    {
	if (*fontTypeRes == XmFONT_IS_FONTSET)
	{
	    /* CR4721 */
            params[0] = fontName;
	    num_params = 1;
	    XtWarningMsg("conversionWarning", "string", "XtToolkitError",
			 MSG3, params, &num_params);

	    return (FALSE);
	}

        if (*delim == '=')
        {
	    (*s)++;
            if (!GetFontTag(s, &fontTag, delim))
            {
	        return (FALSE);
            }
        }
	else if ((*delim == ',') || *delim == '\0')
	{
	    fontTag = XmFONTLIST_DEFAULT_TAG;
	}
	else
        {
	    /* CR4721 */
	    params[0] = fontTag;
	    num_params = 1;
	    XtWarningMsg("conversionWarning", "string", "XtToolkitError",
			 MSG4, params, &num_params);

	    return (FALSE);
        }
    }
    *fontNameRes = fontName;
    *fontTagRes = fontTag;
    return (TRUE);
}

/************************************************************************
 *
 *  GetFontName
 *  
 *
 *  May return null string as fontname (Xm1.1 compatibility).
 ************************************************************************/
static Boolean
GetFontName (
    char **s,
    char **name,
    char *delim )
{
    String params[2];
    Cardinal num_params;

    /*
     * Skip any leading whitespace.
     */

    while (**s != '\0' && isspace((unsigned char)**s))
    {
	(*s)++;
    }
    if (**s == '\0')
    {
        return (FALSE);
    }

    /*
     * Have nonspace.  Find the end of the name.
     */

    *name = *s;
    if (**s == '"')
    {
        (*name)++;
        (*s)++;
        while (**s != '\0' && (**s != '"'))
	{
            (*s)++;
	}
        if (**s == '\0')
        {
	  /* CR4721 */
	  params[0] = --(*name);
	  num_params = 1;
	  XtWarningMsg("conversionWarning", "string", "XtToolkitError",
		       MSG6, params, &num_params);
	  return (FALSE);
        }
        **s = '\0';
        (*s)++;
	*delim = **s;
    }
    else
    {
        while ((**s != '\0') &&
	       (**s != ',') && (**s != ':') && (**s != ';') && (**s != '='))
	{
	      (*s)++;
	}
	*delim = **s;
        **s = '\0';
    }

    return (TRUE);
}

/************************************************************************
 *
 *  GetFontTag
 *  
 ************************************************************************/
static Boolean
GetFontTag (
    char **s,
    char **tag,
    char *delim )
{
    String params[2];
    Cardinal num_params;
    Boolean needs_tag = (*delim == '=');

    /*
     * Skip any leading whitespace.
     */

    while (**s != '\0' && isspace((unsigned char)**s))
    {
	(*s)++;
    }
    if (**s == '\0')
    {
        return (FALSE);
    }

    /*
     * Have nonspace.  Find the end of the tag.
     */

    *tag = *s;
    if (**s == '"')
    {
        (*tag)++;
        (*s)++;
        while (**s != '\0' && (**s != '"'))
	{
            (*s)++;
	}
        if (**s == '\0')
        {
	  /* CR4721 */
	  params[0] = --(*tag);
	  num_params = 1;
	  XtWarningMsg("conversionWarning", "string", "XtToolkitError",
		       MSG6, params, &num_params);
	  return (FALSE);
        }
        **s = '\0';
        (*s)++;
	*delim = **s;
    }
    else
    {
        while (!isspace((unsigned char)**s) && (**s != ',') && (**s != '\0'))
	{
	    (*s)++;
	}
	/* Xm1.1 compatibility */
	*delim = isspace ((unsigned char)**s) ? ',' : **s;	
        **s = '\0';
    }

    /* Null tags are not accepted. */

    if (*s == *tag)
    {
        if (needs_tag) {
	  /* CR4721 */
	  params[0] = XmRFontList;
	  num_params = 1;
	  XtWarningMsg("conversionWarning", "string", "XtToolkitError",
		       MSG7, params, &num_params);
	}
        return (FALSE);
    }

    return (TRUE);
}

/************************************************************************
 *									*
 * GetNextXmString - return a pointer to a null-terminated string.	*
 *                   The pointer is passed in cs. Up to the caller to   *
 *    		     free that puppy. Returns FALSE if end of string.	*
 *									*
 ************************************************************************/
static Boolean 
GetNextXmString(
        char **s,
        char **cs,
        Boolean *after_delimiter )
{
   char *tmp;
   size_t input_length;
   int csize;

   *cs = NULL;

   if (**s == '\0')
   {
      if (!*after_delimiter)
         return(FALSE);
      *cs = XtMalloc(1);
      if (*cs == NULL)
	 return(TRUE);
      **cs = '\0';
      *after_delimiter = False;
      return(TRUE);
   }


   /*  Skip any leading whitespace.  */

   while(isspace((unsigned char)**s) && **s != '\0') (*s)++;

   if (**s == '\0')
   {
      if (!*after_delimiter)
         return(FALSE);
      *cs = XtMalloc(1);
      if (*cs == NULL)
	 return(TRUE);
      **cs = '\0';
      *after_delimiter = False;
      return(TRUE);
   }
  

   /* Found something. Allocate some space (ugh!) and start copying  */
   /* the next string                                                */

   input_length = strlen(*s);
   if (input_length > (size_t)UINT_MAX - 1)
	 return(TRUE);
   *cs = XtMalloc((Cardinal)(input_length + 1));
   if (*cs == NULL)
	 return(TRUE);
   tmp = *cs;

   while((**s) != '\0') 
   {
      if ((**s) == '\\' && *((*s)+1) == ',')	/* Quoted comma */
      {
         (*s)+=2;
         *tmp = ',';
         tmp++;
      }
      else
      {
         if((**s) == ',')			/* End of a string */
         {
            *tmp = '\0';
            (*s)++;
            *after_delimiter = True;
            return(TRUE);
         }
         else
         {
	    if (MB_CUR_MAX > 1) {
#ifndef NO_MULTIBYTE
	      if ((csize = mblen(*s, MB_CUR_MAX)) < 0)
	        break;
#else
	      csize = **s ? 1 : 0;
#endif
	      strncpy(tmp, *s, csize);
	      tmp += csize;
	      (*s) += csize;
	    } else {
	      *tmp = **s;
	      tmp++; 
	      (*s)++;
	    }
         }
       }
    }

    *tmp = '\0';
    *after_delimiter = False;
    return(TRUE);
}

/************************************************************************
 *
 * CvtStringToXmStringTable
 *
 * Convert a string table to an array of XmStrings.This is in the form :
 *
 *       		String [, String2]*
 *
 * The comma delimeter can be  quoted by a \
 *
 ************************************************************************/
/* ARGSUSED */
static Boolean 
CvtStringToXmStringTable(
        Display *dpy,
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val,
        XtPointer *data )
{
  char  *s, *cs;
  XmString *table;
  static  XmString *tblptr;
  int	  str_no, table_size;
  Boolean after_delimiter = False;

  if (from_val == NULL || from_val->addr == NULL || to_val == NULL)
    return FALSE;

  s = (char *) from_val->addr;
  table_size = 100;
  table = (XmString *) XtMalloc(
	(Cardinal)(sizeof(XmString) * (size_t)table_size));
  if (table == NULL)
    return FALSE;
  for (str_no = 0;;)
    {
      cs = NULL;
      if (!GetNextXmString(&s, &cs, &after_delimiter))
	break;
      if (cs == NULL)
	{
	  int cleanup;
	  for (cleanup = 0; cleanup < str_no; ++cleanup)
	    XmStringFree(table[cleanup]);
	  XtFree((char *)table);
	  return False;
	}
      if (str_no >= table_size)
	{
	  int cleanup;
	  XmString *new_table;
	  if (table_size > INT_MAX / 2 ||
	      (unsigned int)table_size > UINT_MAX / (2 * sizeof(XmString)))
	    {
	      XtFree(cs);
	      for (cleanup = 0; cleanup < str_no; ++cleanup)
		XmStringFree(table[cleanup]);
	      XtFree((char *)table);
	      return False;
	    }
	  table_size *= 2;
	  new_table = (XmString *)XtRealloc((char *)table,
					      (Cardinal)(sizeof(XmString) *
							 (size_t)table_size));
	  if (new_table == NULL) {
	      for (cleanup = 0; cleanup < str_no; ++cleanup)
		XmStringFree(table[cleanup]);
	      XtFree((char *)table);
	      XtFree(cs);
	      return False;
	  }
	  table = new_table;
	}
      table[str_no] = XmStringGenerate(cs, XmFONTLIST_DEFAULT_TAG,
				       XmCHARSET_TEXT, NULL);
      XtFree(cs);
      if (table[str_no] == NULL) {
	  int cleanup;
	  for (cleanup = 0; cleanup < str_no; ++cleanup)
	    XmStringFree(table[cleanup]);
	  XtFree((char *)table);
	  return False;
      }
      ++str_no;
    }

  /* NULL terminate the array... */
  if ((unsigned int)str_no >= UINT_MAX / sizeof(XmString))
    {
      int cleanup;
      for (cleanup = 0; cleanup < str_no; ++cleanup)
	XmStringFree(table[cleanup]);
      XtFree((char *)table);
      return False;
    }
  table_size = str_no + 1;
  {
    XmString *new_table = (XmString *)XtRealloc((char *) table,
						 (Cardinal)(sizeof(XmString) *
							    (size_t)table_size));
    if (new_table == NULL) {
      int cleanup;
      for (cleanup = 0; cleanup < str_no; ++cleanup)
	  XmStringFree(table[cleanup]);
      XtFree((char *)table);
      return False;
    }
    table = new_table;
  }
  table[str_no] = (XmString) NULL;

  if (to_val->addr != NULL) 
    {
      if (to_val->size < sizeof(XtPointer)) 
	{
	  int cleanup;
	  to_val->size = sizeof(XtPointer);	
	  for (cleanup = 0; cleanup < str_no; ++cleanup)
	    XmStringFree(table[cleanup]);
	  XtFree((char *)table);
	  return False;
	}		
      *(XmString **)(to_val->addr) = table;
    }
  else 
    {
      tblptr = table;
      to_val->addr = (XPointer)&tblptr;
    }				
  to_val->size = sizeof(XtPointer);
  return TRUE;
}

/****************
 *
 * XmStringCvtDestroy - free up the space allocated by the converter
 *
 ****************/
/*ARGSUSED*/
static void 
XmStringCvtDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer data,		/* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args )	/* unused */
{
   int i;
   if (to == NULL || to->addr == NULL)
       return;
   XmString *table = *(XmString **)(to->addr);
   if (table == NULL)
       return;
   for (i = 0; table[i] != NULL; i++)
       XmStringFree(table[i]);       
   XtFree((char*)table);
}

/*ARGSUSED*/
static Boolean
CvtStringToStringTable(
        Display *dpy,		/* unused */
        XrmValuePtr args,	/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from_val,
        XrmValue *to_val,
        XtPointer *data)	/* unused */
{   
    register char *p ;
            char *top ;
            String *table ;
    static String *tblptr ;
            int size = 50 ;
            int i, len ;
            size_t span;
            int csize;
            Boolean after_comma = False;

    if (from_val == NULL || to_val == NULL ||
        (p = from_val->addr) == NULL)
    {   return( False) ;
        } 
    table = (String *) XtMalloc(
	(Cardinal)(sizeof(String) * (size_t)size));
    if (table == NULL)
	return(False);

    for(    i = 0 ; *p || after_comma ; i++    )
    {   
        after_comma = False;
        while(    isspace((unsigned char) *p) && *p != '\0'    )
        {   p++ ;
            } 
        if(    *p == '\0'    )
        {   
            if(    i == size    )
            {   
                int cleanup;
                String *new_table;
                if (size >= INT_MAX ||
                    (unsigned int)size > UINT_MAX / sizeof(String))
                {
                    for (cleanup = 0; cleanup < i; ++cleanup)
                        XtFree((char *)table[cleanup]);
                    XtFree((char *)table);
                    return(False);
                }
                size++ ;
                new_table = (String *)XtRealloc((char *)table,
							 (Cardinal)(sizeof(String) *
								    (size_t)size));
                if (new_table == NULL) {
                    for (cleanup = 0; cleanup < i; ++cleanup)
				XtFree((char *)table[cleanup]);
                    XtFree((char *)table);
                    return(False);
                }
                table = new_table;
                }
            table[i] = XtMalloc( sizeof( char)) ;
            if (table[i] == NULL) {
                int cleanup;
                for (cleanup = 0; cleanup < i; ++cleanup)
		    XtFree((char *)table[cleanup]);
                XtFree((char *)table);
                return(False);
            }
            *(table[i]) = '\0' ;
            ++i;
            break ;
            }
        for(    top = p ; *p != ',' && *p != '\0' ; p+=csize    )
        {   
            if(    *p == '\\' && *(p + 1) == ','    )
            {   p++ ;
                } 
#ifndef NO_MULTIBYTE
	    if((csize = mblen(p, MB_CUR_MAX)) < 0)
 	      break;
#else
	    csize = *p ? 1 : 0;
#endif
            } 
        if(    i == size    )
        {   
            int cleanup;
            String *new_table;
            if (size > INT_MAX / 2 ||
                (unsigned int)size > UINT_MAX / (2 * sizeof(String)))
            {
                for (cleanup = 0; cleanup < i; ++cleanup)
                    XtFree((char *)table[cleanup]);
                XtFree((char *)table);
                return(False);
            }
            size *= 2 ;
            new_table = (String *)XtRealloc((char *)table,
							 (Cardinal)(sizeof(String) *
								    (size_t)size));
            if (new_table == NULL) {
                for (cleanup = 0; cleanup < i; ++cleanup)
			    XtFree((char *)table[cleanup]);
                XtFree((char *)table);
                return(False);
            }
            table = new_table;
            }
        span = (size_t)(p - top);
        if (span > (size_t)INT_MAX - 1) {
            int cleanup;
            for (cleanup = 0; cleanup < i; ++cleanup)
                XtFree((char *)table[cleanup]);
            XtFree((char *)table);
            return(False);
        }
        len = (int)span ;
        table[i] = XtMalloc((Cardinal)(span + 1));
	if (table[i] == NULL) {
            int cleanup;
            for (cleanup = 0; cleanup < i; ++cleanup)
		XtFree((char *)table[cleanup]);
            XtFree((char *)table);
            return(False);
	}
        strncpy( table[i], top, len) ;
	(table[i])[len] = '\0' ;
        if (*p != '\0')
        {
            p++ ;
            after_comma = True;
        }
        }
    if ((unsigned int)i >= UINT_MAX / sizeof(String))
    {
        int cleanup;
        for (cleanup = 0; cleanup < i; ++cleanup)
            XtFree((char *)table[cleanup]);
        XtFree((char *)table);
        return(False);
    }
    {
        String *new_table = (String *)XtRealloc((char *)table,
						 (Cardinal)(sizeof(String) *
							    (size_t)(i + 1)));
        if (new_table == NULL) {
            int cleanup;
            for (cleanup = 0; cleanup < i; ++cleanup)
		XtFree((char *)table[cleanup]);
            XtFree((char *)table);
            return(False);
        }
        table = new_table;
    }
    table[i] = NULL ;

    if(    to_val->addr != NULL    )
    {   
        if(    to_val->size < sizeof( XPointer)    )
        {   
            to_val->size = sizeof( XPointer) ;
            for (i = 0; table[i] != NULL; ++i)
		XtFree((char *)table[i]);
            XtFree((char *)table);
            return( False) ;
            }
        *(String **)(to_val->addr) = table ;
        }
    else
    {   tblptr = table ;
        to_val->addr = (XPointer)&tblptr ;
        }
    to_val->size = sizeof( XPointer) ;
    return( True) ;
    }
 
/*ARGSUSED*/
static void
StringCvtDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer data,		/* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{   
            int i ;
            if (to == NULL || to->addr == NULL)
                return;
            String *table = * (String **) (to->addr) ;
            if (table == NULL)
                return;

    for(    i = 0 ; table[i] != NULL ; i++    )
    {   XtFree( (char *) table[i]) ;
        } 
    XtFree( (char *) table) ;

    return ;
    }
 
/*ARGSUSED*/
static Boolean
CvtStringToCardinalList(
    Display *dpy,		/* unused */
    XrmValuePtr args,	/* unused */
    Cardinal *num_args,	/* unused */
    XrmValue *from_val,
    XrmValue *to_val,
    XtPointer *data)	/* unused */
{
    register char *	p;
    char *		end;
    Cardinal *		crd_array;
    int             	crd_array_size = 50;
    int			crd_array_count = 0;
    unsigned long	new_element;

    if (from_val == NULL || to_val == NULL ||
        (p = from_val->addr) == NULL)
        return(False);

    crd_array = (Cardinal *)XtCalloc
			(crd_array_size,sizeof(Cardinal));
    if (crd_array == NULL)
	return(False);
    while (*p != '\0')
	{
	while ((isspace((unsigned char)*p) || *p == ',') && *p != '\0')
	    p++;
	if (*p == '\0')				/* end-of data */
	    break;
	if (*p == '-')
	    goto invalid_cardinal;
	errno = 0;
	new_element = strtoul(p, &end, 10);
	if (end == p || errno == ERANGE || new_element > UINT_MAX)
	    goto invalid_cardinal;
	if (*end != '\0' && !isspace((unsigned char)*end) && *end != ',')
	    goto invalid_cardinal;
	if (crd_array_size == crd_array_count)
	    {
	    if (crd_array_size > INT_MAX / 2 ||
		(size_t)(crd_array_size * 2) > SIZE_MAX / sizeof(Cardinal) ||
		(size_t)(crd_array_size * 2) > UINT_MAX / sizeof(Cardinal))
		goto invalid_cardinal;
	    crd_array_size *= 2;
	    {
	    Cardinal *new_array = (Cardinal *)XtRealloc((char *)crd_array,
							 (Cardinal)(sizeof(Cardinal) *
								    (size_t)crd_array_size));
	    if (new_array == NULL)
		goto invalid_cardinal;
	    crd_array = new_array;
	    }
	    }
	crd_array[crd_array_count++] = (Cardinal)new_element;
	p = end;
	}

        _XM_CONVERTER_DONE(to_val,Cardinal *,crd_array,;)

invalid_cardinal:
        XtDisplayStringConversionWarning(dpy, from_val->addr, XmRCardinalList);
        XtFree((char *)crd_array);
        return(False);
}

/*ARGSUSED*/
static void
CardinalListCvtDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer data,		/* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{
        if (to == NULL || to->addr == NULL)
            return;
        XtFree((XPointer)to->addr);
}

/*ARGSUSED*/
static Boolean
CvtStringToHorizontalPosition(
        Display *display,	
        XrmValue *args,
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    Widget widget;
    Screen * screen;
    unsigned char defaultFromType;
    Position tmpPix;
    Boolean parseError;

    if (args == NULL || args[0].addr == NULL || from == NULL ||
        from->addr == NULL || to == NULL)
        return False;
    widget = *(Widget*) args[0].addr ;
    if (widget == NULL)
        return False;
    screen = XtScreen(widget) ;
    defaultFromType = _XmGetUnitType(widget) ;
 
    tmpPix = (Position)
	_XmConvertStringToUnits (screen, from->addr, (int) defaultFromType,
			       XmHORIZONTAL, XmPIXELS, (XtEnum*) &parseError);
    if (parseError) {
        XtDisplayStringConversionWarning(display, (char *)from->addr,
					 XmRHorizontalPosition);
        return False;
    }
    else
        _XM_CONVERTER_DONE( to, Position, tmpPix, ; )
}

/*ARGSUSED*/
static Boolean
CvtStringToHorizontalDimension(
        Display *display,	
        XrmValue *args,
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    Widget widget;
    Screen * screen;
    unsigned char defaultFromType;
    Dimension tmpPix;
    Boolean parseError;

    if (args == NULL || args[0].addr == NULL || from == NULL ||
        from->addr == NULL || to == NULL)
        return False;
    widget = *(Widget*) args[0].addr ;
    if (widget == NULL)
        return False;
    screen = XtScreen(widget) ;
    defaultFromType = _XmGetUnitType(widget) ;
 
    tmpPix = (Dimension)
      _XmConvertStringToUnits (screen, from->addr, (int) defaultFromType,
			       XmHORIZONTAL, XmPIXELS, (XtEnum*) &parseError);
    if (parseError)
        {
        XtDisplayStringConversionWarning(display, (char *)from->addr, 
					 XmRHorizontalDimension);
        return False;
        }
    else
        _XM_CONVERTER_DONE( to, Dimension, tmpPix, ; )
    }

/*ARGSUSED*/
static Boolean
CvtStringToVerticalPosition(
        Display *display,	
        XrmValue *args,
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    Widget widget;
    Screen * screen;
    unsigned char defaultFromType;
    Position tmpPix;
    Boolean parseError;

    if (args == NULL || args[0].addr == NULL || from == NULL ||
        from->addr == NULL || to == NULL)
        return False;
    widget = *(Widget*) args[0].addr ;
    if (widget == NULL)
        return False;
    screen = XtScreen(widget) ;
    defaultFromType = _XmGetUnitType(widget) ;
 
    tmpPix = (Position)
	_XmConvertStringToUnits(screen, from->addr, (int) defaultFromType,
				XmVERTICAL, XmPIXELS, (XtEnum*) &parseError);
    if (parseError)
	{
            XtDisplayStringConversionWarning(display, (char *)from->addr, 
					     XmRVerticalPosition);
            return False;
        }
        else
            _XM_CONVERTER_DONE( to, Position, tmpPix, ; )
}

/*ARGSUSED*/
static Boolean
CvtStringToVerticalDimension(
        Display *display,	
        XrmValue *args,
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
        Widget widget;
        Screen * screen;
        unsigned char defaultFromType;
	Dimension tmpPix;
        Boolean parseError;

        if (args == NULL || args[0].addr == NULL || from == NULL ||
            from->addr == NULL || to == NULL)
            return False;
        widget = *(Widget*) args[0].addr ;
        if (widget == NULL)
            return False;
        screen = XtScreen(widget) ;
        defaultFromType = _XmGetUnitType(widget) ;
 
        tmpPix = (Dimension)
	  _XmConvertStringToUnits(screen, from->addr, (int) defaultFromType,
				  XmVERTICAL, XmPIXELS, (XtEnum*) &parseError);
        if (parseError)
            {
            XtDisplayStringConversionWarning(display, (char *)from->addr,
					     XmRVerticalDimension);
            return False;
            }
        else
            _XM_CONVERTER_DONE( to, Dimension, tmpPix, ; )
    }

	
/************************************************************************
 *
 *  XmeGetDefaultRenderTable
 *       This function is called by a widget to initialize it's rendertable
 *   resource with a default, when it is NULL. This is done by checking to
 *   see if any of the widgets, in the widget's parent hierarchy has the
 *   specifyRenderTable trait.
 *
 *************************************************************************/
typedef struct _system_font_list
{
    Display	*display;
    XmFontList	 fontlist;
} SystemFontList;

static SystemFontList *sFontLists = NULL;
static int nsFontLists = 0;
static int maxnsFontLists = 0;

static XmFontList DefaultSystemFontList(Display *display, XmFontList fontlist)
{
    if (fontlist)
    {
	if (nsFontLists >= maxnsFontLists)
	{
	    Cardinal nbytes;
	    int new_maxns_font_lists;
	    SystemFontList *new_font_lists;

	    if (maxnsFontLists > INT_MAX - 8 ||
		(size_t)(maxnsFontLists + 8) >
		    UINT_MAX / sizeof(SystemFontList))
		return NULL;
	    new_maxns_font_lists = maxnsFontLists + 8;
	    nbytes = (Cardinal) sizeof(SystemFontList)*new_maxns_font_lists;
	    if (NULL == sFontLists)
	    {
	        new_font_lists = (SystemFontList*) XtMalloc(nbytes);
		if (new_font_lists == NULL)
		    return NULL;
		memset((void*) new_font_lists, 0, nbytes);
	    }
	    else
	    {
	        new_font_lists =
		  (SystemFontList*) XtRealloc((char*) sFontLists, nbytes);
		if (new_font_lists == NULL)
		    return NULL;
		memset((void*) &new_font_lists[nsFontLists], 0,
		       sizeof(SystemFontList) *
		       (new_maxns_font_lists - nsFontLists));
	    }
	    sFontLists = new_font_lists;
	    maxnsFontLists = new_maxns_font_lists;
	}
        /* Capacity growth and insertion are independent: another display
         * normally arrives while this array still has spare capacity. */
        sFontLists[nsFontLists].display = display;
        sFontLists[nsFontLists].fontlist = fontlist;
        nsFontLists++;
    }
    else
    {
	int i;
	if (NULL == sFontLists) return NULL;

	for (i=0; i<nsFontLists; i++)
	{
	    if (sFontLists[i].display == display)
	      return sFontLists[i].fontlist;
	}
    }
    return NULL;
}

/* XmeGetDefaultRenderTable returns a borrowed table. The per-display cache
 * retains its original reference until the native display owner retires;
 * widgets retain their own copies independently. Never retain Display keys
 * after teardown, since a later connection may reuse that address. */
void
_XmFreeDefaultRenderTable(Display *display)
{
    XmFontList fontlist = NULL;
    int i;
    _XmProcessLock();
    for (i = 0; i < nsFontLists; ++i)
    {
        if (sFontLists[i].display == display)
        {
            fontlist = sFontLists[i].fontlist;
            --nsFontLists;
            if (i < nsFontLists)
                memmove(&sFontLists[i], &sFontLists[i + 1],
                        sizeof(SystemFontList) * (nsFontLists - i));
            memset(&sFontLists[nsFontLists], 0, sizeof(SystemFontList));
            if (!nsFontLists)
            {
                XtFree((char *)sFontLists);
                sFontLists = NULL;
                maxnsFontLists = 0;
            }
            break;
        }
    }
    _XmProcessUnlock();
    if (fontlist)
        XmFontListFree(fontlist);
}

XmFontList 
XmeGetDefaultRenderTable(
        Widget w,
#if NeedWidePrototypes
        unsigned int fontListType )
#else
        unsigned char fontListType )
#endif /* NeedWidePrototypes */
{
    XmFontList fontlist = NULL;
    Widget origw = w;
    XmFontListEntry fontListEntry;
    char *s;
    char *newString;
    char *sPtr;
    char *fontName;
    char *fontTag;
    XmFontType fontType;
    char delim;
    XmSpecRenderTrait trait ;

    if (w == NULL)
        return NULL;
    _XmWidgetToAppContext(w);

    if (fontListType) {
        _XmAppLock(app);
	/* look for the first ancestor with the trait */
	while ((w = XtParent(w)) != NULL) {
	    if ((trait = (XmSpecRenderTrait) 
		 XmeTraitGet((XtPointer) XtClass(w), 
			     XmQTspecifyRenderTable)) != NULL) {
		fontlist = trait->getRenderTable(w, fontListType) ;
		break ;  
	    }
	}
        _XmAppUnlock(app);
    }

    if (fontlist) {
        return (fontlist);
    }

    _XmProcessLock();
    fontlist = DefaultSystemFontList(XtDisplay(origw), (XmFontList) NULL);
    if (!fontlist) {
	/* Begin fixing OSF 4735 */
	s = (char *) XmDEFAULT_FONT;
	sPtr = newString = XtNewString (s);
	if (newString == NULL) {
	    _XmProcessUnlock();
	    return NULL;
	}

	if (!GetNextFontListEntry (&sPtr, &fontName, &fontTag,
				   &fontType, &delim)) {
	    _XmProcessUnlock();
	    XtFree (newString);
	    XmeWarning(NULL, MSG2);
	    exit( 1) ;
	}

	do {
	    if (*fontName) {
		fontListEntry = XmFontListEntryLoad (XtDisplay(origw), 
						     fontName,
						     fontType, fontTag);
		if (fontListEntry != NULL) {
		    fontlist = XmFontListAppendEntry (fontlist, fontListEntry);
		    XmFontListEntryFree (&fontListEntry);
		}
		else
		    XtDisplayStringConversionWarning(XtDisplay(origw), 
						     fontName, XmRFontList);
	    }
	}
	while ((delim == ',') && *++sPtr && !fontlist &&
	       GetNextFontListEntry (&sPtr, &fontName, &fontTag,
				     &fontType, &delim));
	XtFree (newString);
	DefaultSystemFontList(XtDisplay(origw), fontlist);
	/* End fixing OSF 4735 */
    }
    _XmProcessUnlock();
    return (fontlist);
}

/*ARGSUSED*/
static void
ConvertStringToButtonTypeDestroy( 
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer converter_data, /* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{   
    if (to == NULL || to->addr == NULL)
        return;
    XtFree( *((char **) to->addr)) ;

    return ;
    } 

/*ARGSUSED*/
static Boolean
ConvertStringToButtonType(
        Display *display,	
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    String in_str;
    size_t in_str_size = 0 ;
    XmButtonTypeTable buttonTable ;
    size_t i, comma_count ;
    String work_str, btype_str ;
    _Xstrtokparams strtok_buf;

    if (display == NULL || from == NULL || from->addr == NULL || to == NULL)
        return FALSE;
    in_str = (String) from->addr ;
    if (in_str == NULL)
        return(FALSE);
    
    comma_count = 0 ;
    while(    in_str[in_str_size]    )
    {   if (in_str_size == SIZE_MAX)
            return(FALSE);
        if(    in_str[in_str_size++] == ','    )
        {   if (comma_count == SIZE_MAX)
                return(FALSE);
            ++comma_count ;
            } 
        } 
    if (in_str_size == SIZE_MAX)
        return(FALSE);
    ++in_str_size ;

    if (comma_count > SIZE_MAX / sizeof(XmButtonType) - 2 ||
        comma_count > (size_t)UINT_MAX / sizeof(XmButtonType) - 2 ||
        in_str_size > (size_t)UINT_MAX)
        return(FALSE);

    buttonTable = (XmButtonTypeTable) XtMalloc( 
                                   (Cardinal)(sizeof(XmButtonType) *
						      (comma_count + 2))) ;
    if (buttonTable == NULL)
        return(FALSE);
    buttonTable[comma_count+1] = (XmButtonType)0;
    work_str = (String) XtMalloc((Cardinal)in_str_size) ;
    if (work_str == NULL) {
        XtFree((char *) buttonTable);
        return(FALSE);
    }
    strcpy( work_str, in_str) ;

    for(    i = 0, btype_str = _XStrtok(work_str, ",", strtok_buf) ;
            btype_str ;
            btype_str = _XStrtok(NULL, ",", strtok_buf), ++i)
    {
        while (*btype_str && isspace((unsigned char)*btype_str)) btype_str++;
        if (*btype_str == '\0')
            break;
        if (XmeNamesAreEqual(btype_str, "pushbutton"))
            buttonTable[i] = XmPUSHBUTTON;
        else if (XmeNamesAreEqual(btype_str, "togglebutton"))
            buttonTable[i] = XmTOGGLEBUTTON;
        else if (XmeNamesAreEqual(btype_str, "cascadebutton"))
            buttonTable[i] = XmCASCADEBUTTON;
        else if (XmeNamesAreEqual(btype_str, "separator"))
            buttonTable[i] = XmSEPARATOR;
        else if (XmeNamesAreEqual(btype_str, "double_separator"))
            buttonTable[i] = XmDOUBLE_SEPARATOR;
        else if (XmeNamesAreEqual(btype_str, "title"))
            buttonTable[i] = XmTITLE;
        else
        {
            XtDisplayStringConversionWarning(display, (char *) btype_str,
					     XmRButtonType) ;
            XtFree( (char *) buttonTable) ;
            XtFree( (char *) work_str) ;

            return( FALSE) ;
            }
        }
    if (i != comma_count + 1)
        {
        XtDisplayStringConversionWarning(display, in_str, XmRButtonType);
        XtFree((char *) buttonTable);
        return(FALSE);
        }
    XtFree( work_str) ;

    _XM_CONVERTER_DONE( to, XmButtonTypeTable, buttonTable, XtFree( (char *) buttonTable) ; )
    }

/*ARGSUSED*/
static void
CvtStringToKeySymTableDestroy( 
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer converter_data, /* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{   
    if (to == NULL || to->addr == NULL)
        return;
    XtFree( *((char **) to->addr)) ;

    return ;
    } 

/*ARGSUSED*/
static Boolean
CvtStringToKeySymTable(
        Display *display,	
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
  String in_str;
  size_t in_str_size = 0;
  XmKeySymTable keySymTable;
  size_t i, comma_count;
  String work_str, ks_str, separator;
  KeySym ks;

  if (display == NULL || from == NULL || from->addr == NULL || to == NULL)
    return FALSE;
  in_str = (String) from->addr;
  if (in_str == NULL)
    return(FALSE);

  comma_count = 0;
  while (in_str[in_str_size])
    {   
	if (in_str_size == SIZE_MAX)
	  return(FALSE);
      if (in_str[in_str_size++] == ',')
	++comma_count;
    } 
  if (comma_count > (SIZE_MAX / sizeof(KeySym)) - 2 ||
      comma_count > ((size_t)UINT_MAX / sizeof(KeySym)) - 2)
    return(FALSE);

  keySymTable = (XmKeySymTable) XtMalloc(
	(Cardinal)(sizeof(KeySym) * (comma_count + 2)));
  if (keySymTable == NULL)
    return(FALSE);
  keySymTable[comma_count + 1] = (KeySym)NULL;
  work_str = XtNewString(in_str);
  if (work_str == NULL) {
    XtFree((char *) keySymTable);
    return(FALSE);
  }

  for (ks_str = work_str, i = 0; i < comma_count + 1; ++i)
    {
	separator = strchr(ks_str, ',');
	if (separator)
	    *separator = '\0';
      if (!*ks_str)
	keySymTable[i] = NoSymbol;
      else
	{  
	  if ((ks = XStringToKeysym(ks_str)) == NoSymbol)
	    {   
	      XtDisplayStringConversionWarning(display, ks_str, XmRKeySym);
	      XtFree((char *) work_str);
	      XtFree((char *) keySymTable);

	      return FALSE;
	    } 
	  keySymTable[i] = ks;
	}
	if (!separator)
	    break;
	ks_str = separator + 1;
    }
  XtFree((char *) work_str);

  _XM_CONVERTER_DONE(to, XmKeySymTable, keySymTable, 
		     XtFree((char*)keySymTable);)
}

/*ARGSUSED*/
static void
CvtStringToCharSetTableDestroy( 
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer converter_data, /* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{   
    if (to == NULL || to->addr == NULL)
        return;
    XtFree( *((char **) to->addr)) ;

    return ;
    } 

/*ARGSUSED*/
static Boolean
CvtStringToCharSetTable(
        Display *display,	/* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
  String in_str;
  XmStringCharSetTable charsetTable;
  unsigned int numCharsets = 0;
  size_t strDataSize = 0;
  size_t tableSize;
  char * dataPtr;
  int i;
  String work_str, cs_str, separator;

  if (from == NULL || from->addr == NULL || to == NULL)
    return FALSE;
  in_str = (String) from->addr;
  if (in_str == NULL)
    return(False);

  work_str = XtNewString(in_str);
  if (work_str == NULL)
    return(False);

  for (cs_str = work_str; ; cs_str = separator + 1)
    {
      separator = strchr(cs_str, ',');
      if (separator)
	*separator = '\0';
      if (*cs_str)
	{
	  size_t itemSize = strlen(cs_str);
	  if (itemSize == SIZE_MAX || strDataSize > SIZE_MAX - (itemSize + 1))
	    {
	      XtFree(work_str);
	      return(False);
	    }
	  strDataSize += itemSize + 1;
	}
      if (numCharsets == UINT_MAX)
	{
	  XtFree(work_str);
	  return(False);
	}
      ++numCharsets;
      if (!separator)
	break;
    }
  strcpy(work_str, in_str);

  if ((size_t)numCharsets > (SIZE_MAX / sizeof(XmStringCharSet)) - 1)
    {
      XtFree(work_str);
      return(False);
    }
  tableSize = sizeof(XmStringCharSet) * ((size_t)numCharsets + 1);
  if (strDataSize > SIZE_MAX - tableSize ||
      strDataSize > (size_t)UINT_MAX - tableSize)
    {
      XtFree(work_str);
      return(False);
    }
  charsetTable = (XmStringCharSetTable) XtMalloc(
	(Cardinal)(tableSize + strDataSize));
  if (charsetTable == NULL) {
    XtFree(work_str);
    return(False);
  }
  charsetTable[numCharsets] = (XmStringCharSet)NULL;
  dataPtr = (char *) &charsetTable[numCharsets+1];
  strcpy(work_str, in_str);

  for (i = 0, cs_str = work_str; i < (int)numCharsets; ++i)
	{
	separator = strchr(cs_str, ',');
	if (separator)
	    *separator = '\0';
      if (*cs_str)
	{
	  charsetTable[i] = dataPtr;
	  strcpy(dataPtr, cs_str);
	  dataPtr += strlen(cs_str) + 1;
	}
      else
	{   
	  charsetTable[i] = NULL;
	} 
	if (!separator)
	    break;
	cs_str = separator + 1;
    }
  XtFree((char *) work_str);

  _XM_CONVERTER_DONE(to, XmStringCharSetTable, charsetTable,
		     XtFree((char *) charsetTable);)
}

/************************************************************************
 *
 *  CvtStringToBooleanDimension
 *
 ************************************************************************/
/*ARGSUSED*/
static Boolean
CvtStringToBooleanDimension(
        Display *display,	
        XrmValue *args,
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
        char *in_str;
        Dimension outVal ;
        int intermediate;

        if (from == NULL || from->addr == NULL || to == NULL)
            return FALSE;
        in_str = (char *) from->addr ;

    if (isInteger(from->addr, &intermediate))
    {   
        /* Is numeric argument, so convert to horizontal dimension.  This is
        *   to preserve 1.0 compatibility (the resource actually behaves like
        *   a boolean in version 1.1).
        */
        Widget widget;
        Screen *screen;
        unsigned char unitType;

        if (args == NULL || args[0].addr == NULL)
            return FALSE;
        widget = *(Widget*) args[0].addr ;
        if (widget == NULL)
            return FALSE;
        screen = XtScreen(widget) ;
        unitType = _XmGetUnitType(widget) ;
	
        if(    intermediate < 0    )
        {   XtDisplayStringConversionWarning(display, (char *)from->addr,
					     XmRBooleanDimension) ;
            return( FALSE) ;
            } 
        outVal = (Dimension) _XmConvertUnits( screen, XmHORIZONTAL,
                                      (int) unitType, intermediate, XmPIXELS) ;
        } 
    else
    {   /* Presume Boolean (version 1.1).
        */
        if(    XmeNamesAreEqual( in_str, XtEtrue)    )
        {   outVal = (Dimension) 1 ;
            } 
        else
        {   if(    XmeNamesAreEqual( in_str, XtEfalse)    )
            {   outVal = (Dimension) 0 ;
                } 
            else
            {   XtDisplayStringConversionWarning(display, in_str,
						 XmRBooleanDimension) ;
                return( FALSE) ;
                } 
            } 
        } 
    _XM_CONVERTER_DONE( to, Dimension, outVal, ; )
    }



/************************************************************************
 *
 *  XmCvtStringToAtomList
 *	Convert a string to an array of atoms.  Atoms within the string
 *  are delimited by commas.  If the comma is preceded by a backslash,
 *  it is considered to be part of the atom.
 *
 ************************************************************************/
/*ARGSUSED*/
static Boolean
CvtStringToAtomList(
	Display *dpy,
	XrmValue *args,		/* unused */
	Cardinal *num_args,	/* unused */
	XrmValue *from,
	XrmValue *to,
	XtPointer *converter_data ) /* unused */
{
  char *atom_name;
  char *stack_names[128];
  char **name_list = stack_names;
  int max_atoms = XtNumber(stack_names);
  int atom_count;
  Atom *atom_list;
  char *context_string;

  if (dpy == NULL || from == NULL || from->addr == NULL || to == NULL)
    return(False);
	
  atom_count = 0;
  for (atom_name = GetNextToken((char*) from->addr, ",", &context_string);
       atom_name != NULL;
       atom_name = GetNextToken(NULL, ",", &context_string))
    {
      if (atom_count == max_atoms)
	{
	  if (max_atoms > INT_MAX / 2 ||
	      (size_t)max_atoms > SIZE_MAX / (2 * sizeof(char *)) ||
	      (size_t)max_atoms > UINT_MAX / (2 * sizeof(char *)))
	    {
	      int cleanup;
	      XtFree(atom_name);
	      for (cleanup = 0; cleanup < atom_count; ++cleanup)
		XtFree(name_list[cleanup]);
	      if (name_list != stack_names)
		XtFree((char *)name_list);
	      return(False);
	    }
	  max_atoms *= 2;

	  if (name_list == stack_names)
	    {
	      char **new_names = (char **) XtMalloc(
		(Cardinal)(sizeof(char *) * (size_t)max_atoms));
	      if (new_names == NULL) {
		int cleanup;
		XtFree(atom_name);
		for (cleanup = 0; cleanup < atom_count; ++cleanup)
		  XtFree(name_list[cleanup]);
		return(False);
	      }
	      memcpy((char *)new_names, (char *)name_list,
		     (sizeof(char *) * atom_count));
	      name_list = new_names;
	    }
	  else
	    {
	      char **new_names = (char **) XtRealloc((char *)name_list,
						     (Cardinal)(sizeof(char *) *
								 (size_t)max_atoms));
	      if (new_names == NULL) {
		int cleanup;
		XtFree(atom_name);
		for (cleanup = 0; cleanup < atom_count; ++cleanup)
		  XtFree(name_list[cleanup]);
		XtFree((char *)name_list);
		return(False);
	      }
	      name_list = new_names;
	    }
	}

      name_list[atom_count++] = atom_name;
    }

  if ((size_t)atom_count > SIZE_MAX / sizeof(Atom) ||
      (size_t)atom_count > UINT_MAX / sizeof(Atom))
    {
      int cleanup;
      for (cleanup = 0; cleanup < atom_count; ++cleanup)
	XtFree(name_list[cleanup]);
      if (name_list != stack_names)
	XtFree((char *)name_list);
      return(False);
    }
  atom_list = (Atom *) XtMalloc(
	(Cardinal)(sizeof(Atom) * (size_t)atom_count));
  if (atom_list == NULL) {
    int cleanup;
    for (cleanup = 0; cleanup < atom_count; ++cleanup)
	XtFree(name_list[cleanup]);
    if (name_list != stack_names)
	XtFree((char *)name_list);
    return(False);
  }
  XInternAtoms(dpy, name_list, atom_count, False, atom_list);

  while(--atom_count >= 0)
    XtFree(name_list[atom_count]);

  {
    static Atom *buf;

    if(to->addr)
      {
	if(to->size < sizeof(Atom *))
	  {
	    XtFree((char *) atom_list);
	    to->size = sizeof(Atom *);
	    return(False);
	  }
	else
	  *((Atom **) (to->addr)) = atom_list;
      }
    else
      {
	buf = atom_list;
	to->addr = (XPointer) &buf;
      }

    to->size = sizeof(Atom *);
    return(True);
  }
}

/*ARGSUSED*/
static void 
SimpleDestructor(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer data,		/* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args )	/* unused */
{
   char *simple_struct;
   if (to == NULL || to->addr == NULL)
       return;
   simple_struct = *(char **)(to->addr);

   XtFree(simple_struct);
}

/*
 *
 * GetNextToken
 *
 * This should really be in some sort of utility library.
 * This function is supposed to behave a bit like strtok in that it
 * saves a context which is used if src is NULL.  We'd like to use
 * strok, but strok can't handle backslashes.
 *
 * A token is the contiguous substring of src which begins with either
 * a backslashed space character or a non-space character and
 * terminates with occurance of a non-backslashed delimiter character
 * or the character before the last non-backshashed space character.
 *
 * Caller is responsible to free the returned string.
 *
 * Example A:
 *    The delimiter string is ","   The src is
 *           "   \ token  token\ , next token"
 *    The token is
 *           " token token "
 *
 * Example B:
 *
 *    The delimiter string is
 *        ".:"
 *    The src is 
 *        "   \: the \t token \. \    : next token  "
 *    The token returned is
 *        ": the \t token .  "
 *
 */

static Boolean
OneOf(
#if NeedWidePrototypes
        int c,
#else
        char c,
#endif /* NeedWidePrototypes */
	char *set )
{
	char *p;

	for (p = set; *p != 0; p++)
		if (*p == c)
			return(True);
	
	return(False);
}

static char * 
GetNextToken(
	char *src,
	char *delim,
	char **context)
{
	Boolean terminated = False;
	char *s, *e, *p;
	char *next_context;
	char *buf = NULL;
	size_t token_size;

	if (src != NULL)
		*context = src;

	if (*context == NULL)
		return(NULL);

	s = *context;

	/* find the end of the token */
	for (e = s; (!terminated) && (*s != '\0'); e = s++)
	{
		if ((*s == '\\') && (*(s+1) != '\0'))
			s++;
		else if (OneOf(*s, delim))
			terminated = True;
	}

	/* assert (OneOf(*e,delim) || (*e == '\0')) */
	if (terminated)
	{
		next_context = (e + 1);
		e--;
	}
	else
		next_context = NULL;
	
	/* Strip out non-backslashed leading and trailing whitespace */
	s = *context;
	while ((s != e) && isspace((unsigned char)*s))
		s++;
	while ((e != s) && isspace((unsigned char)*e) && ((*e-1) != '\\'))
		e--;

	if (e == s)
	{
		/*
		 * Only white-space between the delimiters,
		 * if we're at the end of the string anyway, indicate
		 * that we're done, otherwise return an empty string.
		 */
		if (terminated)
		{
			buf = (char *) XtMalloc(1);
			if (buf == NULL)
				return(NULL);
			*buf = '\0';
			return(buf);
		}
		else
			return(NULL);
	}
	
	/*
	 * Copy into buffer.  Swallow any backslashes which precede
	 * delimiter characters or spaces.  It would be great if we had
	 * time to implement full C style backslash processing...
	 */
	token_size = (size_t)(e - s) + 1;
	if (token_size > (size_t)UINT_MAX - 1)
		return(NULL);

	p = buf = XtMalloc((Cardinal)(token_size + 1));
	if (buf == NULL)
		return(NULL);
	while (s != e)
	{
		if ((*s == '\\') && 
		    (OneOf(*(s+1), delim) || isspace((unsigned char)*(s+1))))
			s++;
		
		*(p++) = *(s++);
	}
	*(p++) = *(s++);
	*p = '\0';

	*context = next_context;

	return(buf);
}

/*ARGSUSED*/
static Boolean
CvtStringToCardinal(
        Display *display,
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    Cardinal value;
    int intermediate;
    if (from == NULL || from->addr == NULL || to == NULL)
        return False;
    if (!isInteger(from->addr,&intermediate) || intermediate < 0)
	{
	XtDisplayStringConversionWarning(display, (char *)from->addr,
					 XmRCardinal);
	return False;
	}

    value = (Cardinal) intermediate;
    _XM_CONVERTER_DONE( to, Cardinal, value, ; )
}


/*ARGSUSED*/
static Boolean
CvtStringToTextPosition(
        Display *display,
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    XmTextPosition value;
    int intermediate;
    if (from == NULL || from->addr == NULL || to == NULL)
        return False;
    if (!isInteger(from->addr,&intermediate) || intermediate < 0)
        {
        XtDisplayStringConversionWarning(display, (char *)from->addr, 
					 XmRTextPosition);
        return False;
        }

    value = (XmTextPosition) intermediate;
    _XM_CONVERTER_DONE( to, XmTextPosition, value, ; )
}


/*ARGSUSED*/
static Boolean
CvtStringToTopItemPosition(
        Display *display,
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    int value;
    int intermediate;
    if (from == NULL || from->addr == NULL || to == NULL)
        return False;
    if (!isInteger(from->addr,&intermediate) || intermediate < 0)
        {
        XtDisplayStringConversionWarning(display, (char *)from->addr,
					 XmRTopItemPosition);
        return False;
        }

    value = intermediate - 1;
    _XM_CONVERTER_DONE( to, int, value, ; )
}


static Boolean 
isInteger(
    String string,
    int *value)		/* RETURN */
{
    Boolean foundDigit = False;
    Boolean isNegative = False;
    Boolean isPositive = False;
    unsigned long val = 0;
    unsigned long limit;
    unsigned int digit;
    char ch;

    if (string == NULL || value == NULL)
	return False;
    /* skip leading whitespace */
    while ((ch = *string) == ' ' || ch == '\t') string++;
    while ((ch = *string++) != '\0') {
	if (ch >= '0' && ch <= '9') {
	    digit = (unsigned int)(ch - '0');
	    limit = isNegative ? (unsigned long)INT_MAX + 1UL :
		(unsigned long)INT_MAX;
	    if (val > (limit - digit) / 10UL)
		return False;
	    val = val * 10UL + digit;
	    foundDigit = True;
	    continue;
	}
	if (ch == ' ' || ch == '\t') {
	    if (!foundDigit) return False;
	    /* make sure only trailing whitespace */
	    while ((ch = *string++) != '\0') {
		if (ch != ' ' && ch != '\t')
		    return False;
	    }
	    break;
	}
	if (ch == '-' && !foundDigit && !isNegative && !isPositive) {
	    isNegative = True;
	    continue;
	}
	if (ch == '+' && !foundDigit && !isNegative && !isPositive) {
	    isPositive = True;
	    continue;
	}
	return False;
    }
    if (ch == '\0') {
	if (isNegative)
	    *value = (val == (unsigned long)INT_MAX + 1UL) ? INT_MIN :
		-(int)val;
	else
	    *value = (int)val;
	return True;
    }
    return False;
}


/************************************************************************
 *
 *  CvtStringToRenditionPixel
 *	Convert a string to Pixel, checking for the special value
 *	"unspecified_pixel" which returns XmUNSPECIFIED_PIXEL
 *
 ************************************************************************/
static Boolean
CvtStringToRenditionPixel(Display *disp,
			     XrmValuePtr args,
			     Cardinal *num_args,
			     XrmValue *from_val,
			     XrmValue *to_val,
			     XtPointer *converter_data )
{
  String 	str;
  Boolean	result = False;

  if (disp == NULL || num_args == NULL || from_val == NULL ||
      from_val->addr == NULL || to_val == NULL)
    return False;
  str = (String)from_val->addr;

  if (XmeNamesAreEqual(str, "unspecified_pixel"))
    {
      _XM_CONVERTER_DONE(to_val, Pixel, XmUNSPECIFIED_PIXEL, ;)
      }
  
  result = XtCallConverter(disp, XtCvtStringToPixel, args, *num_args,
			   from_val, to_val, NULL);
  
  if (result == False)
    {
      if (converter_data != NULL)
        *converter_data = False;
      return False;
    }
  else
    {
      if (converter_data != NULL)
        *converter_data = (char *)True;
      return True;
    }
}

/************************************************************************
 *
 *  CvtPixelToRenditionPixel
 *	Convert a Pixel to a RenditionPixel, which really means
 *      doing nothing.
 *
 ************************************************************************/

/*ARGSUSED*/
static Boolean
CvtPixelToRenditionPixel(Display *disp,
			     XrmValuePtr args, /* unused */
			     Cardinal *num_args,
			     XrmValue *from_val,
			     XrmValue *to_val,
			     XtPointer *converter_data ) /* unused */
{
    if (num_args == NULL || from_val == NULL || from_val->addr == NULL ||
	to_val == NULL)
	return False;
    if (*num_args != 0)
	{
	if (disp == NULL)
	    return False;
	XtAppWarningMsg(XtDisplayToApplicationContext(disp),
		"wrongParameters","CvtPixelToRenditionPixel", "ToolkitError",
		"Pixel to RenditionPixel conversion needs no extra arguments",
			(String *) NULL, (Cardinal *)NULL);
	}
    _XM_CONVERTER_DONE(to_val, Pixel, *(int*)from_val->addr, ;)
}

/************************************************************************
 *
 *  CvtStringToSelectColor
 *	Convert a string to Pixel, checking for the special values
 *	"default_select_color" which returns the XmDEFAULT_SELECT_COLOR,
 *	"highlight_color" which returns the XmHIGHLIGHT_COLOR.
 *	"reversed_ground_colors" which returns XmREVERSED_GROUND_COLORS.
 *
 ************************************************************************/
static Boolean
CvtStringToSelectColor(Display *disp,
			  XrmValuePtr args,
			  Cardinal *num_args,
			  XrmValue *from_val,
			  XrmValue *to_val,
			  XtPointer *converter_data )
{
  String 	str;
  Boolean	result = False;

  if (disp == NULL || num_args == NULL || from_val == NULL ||
      from_val->addr == NULL || to_val == NULL ||
      (*num_args != 0 && args == NULL))
    return False;
  str = (String)from_val->addr;

  /* in those 3 cases, just return the constant and the widget
     will have to do teh onversion itself. It's ok, since the widget
     has to handle the constant case anyway for direct use by
     a program at creation and setvalues time */
  if (XmeNamesAreEqual(str, "default_select_color")) {
      _XM_CONVERTER_DONE(to_val, Pixel, XmDEFAULT_SELECT_COLOR, ;)
      }
  else if (XmeNamesAreEqual(str, "reversed_ground_colors")) {
      _XM_CONVERTER_DONE(to_val, Pixel, XmREVERSED_GROUND_COLORS, ;)
      }
  else if (XmeNamesAreEqual(str, "highlight_color")) {
      _XM_CONVERTER_DONE(to_val, Pixel, XmHIGHLIGHT_COLOR, ;)
      }
  
  /* else call the Xt converter, passing it the colorConvertArg */
  result = XtCallConverter(disp, XtCvtStringToPixel, args, *num_args,
			   from_val, to_val, NULL);
  
  if (result == False)
    {
      if (converter_data != NULL)
        *converter_data = False;
      return False;
    }
  else
    {
      if (converter_data != NULL)
        *converter_data = (char *)True;
      return True;
    }
}


/************************************************************************
 *
 *  GetNextTab
 *  
 ************************************************************************/
static Boolean 
GetNextTab(char **s,
	   float *value,
	   char *unitType,
	   XmOffsetModel *offsetModel)
{
  int	ret_val;
  char 	sign[2];
  char	*tmp;

  if (s == NULL || *s == NULL || value == NULL || unitType == NULL ||
      offsetModel == NULL)
    return FALSE;

  bzero(sign, sizeof(sign));
  unitType[0] = '\0';
  
  if (sscanf(*s, " %2[+]", sign) == 1)
    ret_val = sscanf(*s, " %2[+] %f %12[^ \t\r\n\v\f,] ",
		     sign, value, unitType);
  else ret_val = sscanf(*s, " %f %12[^ \t\r\n\v\f,] ",
			value, unitType);

  if (ret_val == EOF) return(FALSE);
  
  if (sign[1] != '\0')
    {
      /* Error message */
      return(FALSE);
    }
  
  switch (sign[0])
    {
    case '\0':
      *offsetModel = XmABSOLUTE;
      break;
      
    case '+':
      *offsetModel = XmRELATIVE;
      break;
    }
  
  tmp = strpbrk(*s, ",");
  
  if (tmp == NULL) *s += strlen(*s);
  else *s = (tmp + 1);

  return(TRUE);
}


/*ARGSUSED*/
static void
CvtStringToXmTabListDestroy(XtAppContext app, /* unused */
			       XrmValue *to,
			       XtPointer converter_data, /* unused */
			       XrmValue *args, /* unused */
			       Cardinal *num_args) /* unused */
{   
  if (to == NULL || to->addr == NULL)
    return;
  XmTabListFree(*((XmTabList *)to->addr));

  return;
}

/************************************************************************
 *
 *  CvtStringToXmTabList
 *	Convert a string to a tab list.  This is in the form :
 *  
 *  <XmTabList>	::=	<tab> { ',' <tab> }*
 *  
 *  <tab>	::=	<float> <units>
 *  
 *  <float>	::=	{ <sign> } { {DIGIT}*.}DIGIT+
 *  
 *  <sign>	::=	+ | -
 *  
 ************************************************************************/

/*ARGSUSED*/
static Boolean
CvtStringToXmTabList(Display *dpy, 
			XrmValue *args,	/* unused */
			Cardinal *num_args, /* unused */
			XrmValue *from,
			XrmValue *to,
			XtPointer *converter_data) /* unused */
{
  Boolean 	got_one = FALSE;
  char 		*s;
  float 	value;
  char 		unitType[12]; /* longest unit name is "millimeters"  */
  XmOffsetModel	offsetModel = XmABSOLUTE;
  int		units;
  XmParseResult	result;
  XmTab		tab;
  XmTabList	tl = NULL;
  XmTabList	new_tl;

  if (dpy == NULL || from == NULL || from->addr == NULL || to == NULL)
    return FALSE;
  
  if (from->addr)
    {   
      s = (char *)from->addr;

      /* Parse the tabs */
      while (GetNextTab(&s, &value, unitType, &offsetModel))
	{
	  got_one = TRUE;
	  
	  result = XmeParseUnits(unitType, &units);

	  if (result == XmPARSE_ERROR)
	    {
	      got_one = FALSE;
	      break;
	    }
	  else if (result == XmPARSE_NO_UNITS)
	    {
	      units = XmPIXELS;
	    }
	  tab = XmTabCreate(value, (unsigned char)units, offsetModel,
			    XmALIGNMENT_BEGINNING, XmS);
	  if (tab == NULL) {
	    XmTabListFree(tl);
	    return FALSE;
	  }
	  
  new_tl = XmTabListInsertTabs(tl, &tab, 1, -1);
  if (new_tl == NULL) {
    XmTabFree(tab);
    XmTabListFree(tl);
    return FALSE;
  }
  tl = new_tl;
	  
	  XmTabFree(tab);
	}
    }
  
  if (got_one) 
    _XM_CONVERTER_DONE(to, XmTabList, tl, XmTabListFree(tl);)

  XtDisplayStringConversionWarning(dpy, (char *)from->addr, XmRTabList);
  return(FALSE);
} 

/*ARGSUSED*/
static Boolean
cvtStringToXmRenderTable(Display *dpy, 
			 Widget widget,
			 String resname,
			 String resclass,
			 XrmValue *from,
			 XrmValue *to)
{
  char 		*s;
  XmRendition	rend[1];
  XmRenderTable	rt;
  char		*tag;
  Boolean	has_default = FALSE, in_db = FALSE;
  _Xstrtokparams strtok_buf;

  if (dpy == NULL || widget == NULL || from == NULL || from->addr == NULL ||
      to == NULL)
    return FALSE;
  
  if (from->addr)
    {   
      s = XtNewString((char *)from->addr);
      if (s == NULL)
	return FALSE;
      rt = NULL;
      has_default = FALSE;

      /* Try for default rendition */
      rend[0] = _XmRenditionCreate(NULL, widget, resname, resclass,
				   NULL, NULL, 0, NULL);
      
      if (rend[0] != NULL)
	{
	  rt = XmRenderTableAddRenditions(NULL, rend, 1, XmMERGE_REPLACE);
	  has_default = TRUE;
	}
      
      /* Try to get first tag. */
      if ((tag = _XStrtok(s, " \t\r\n\v\f,", strtok_buf)) != NULL)
	{
	  XmRenditionFree(rend[0]);
	  rend[0] = _XmRenditionCreate(NULL, widget, resname, resclass, 
				       tag, NULL, 0, &in_db); 

	  if (!has_default && !in_db)
	    {
	      /* Call the fontlist converter */
	      XmRenditionFree(rend[0]);
	      XtFree(s);
	      return CvtStringToXmFontList(dpy, NULL, 0, from, to, NULL);
	    }
	  
	  rt = XmRenderTableAddRenditions(rt, rend, 1, XmMERGE_REPLACE);
	}
      else if (rend[0] == NULL)
	{
	  /* warning */
	  XtFree(s);
	  return FALSE;
	}
      else 
	{
	  /* only a default rendition */
	  XtFree(s);
	  XmRenditionFree(rend[0]);
	  _XM_CONVERTER_DONE(to, XmRenderTable, rt, XmRenderTableFree(rt);)
	}

      while ((tag = _XStrtok(NULL, " \t\r\n\v\f,", strtok_buf)) != NULL)
	{
	  XmRenditionFree(rend[0]);

	  rend[0] = _XmRenditionCreate(NULL, widget, resname, resclass, 
				       tag, NULL, 0, NULL);
	    
	  rt = XmRenderTableAddRenditions(rt, rend, 1, XmMERGE_REPLACE);
	}

      XtFree(s);
      XmRenditionFree(rend[0]);
      _XM_CONVERTER_DONE(to, XmRenderTable, rt, XmRenderTableFree(rt);)
    }

  return FALSE;
}

/*ARGSUSED*/
static Boolean
CvtStringToRenderTable(Display *dpy,
			  XrmValue *args,
			  Cardinal *num_args, /* unused */
			  XrmValue *from,
			  XrmValue *to,
			  XtPointer *converter_data) /* unused */
{
  Widget wid;

  if (dpy == NULL || args == NULL || args[0].addr == NULL ||
      from == NULL || to == NULL)
    return FALSE;
  
  wid = *(Widget *)args[0].addr;
  if (wid == NULL)
    return FALSE;

  return(cvtStringToXmRenderTable(dpy, wid,
				  XmNrenderTable, XmCRenderTable,
				  from, to));
}

/*ARGSUSED*/
static Boolean
CvtStringToButtonRenderTable(Display *dpy,
				XrmValue *args,
				Cardinal *num_args, /* unused */
				XrmValue *from,
				XrmValue *to,
				XtPointer *converter_data) /* unused */
{
  Widget wid;

  if (dpy == NULL || args == NULL || args[0].addr == NULL ||
      from == NULL || to == NULL)
    return FALSE;
  
  wid = *(Widget *)args[0].addr;
  if (wid == NULL)
    return FALSE;

  return(cvtStringToXmRenderTable(dpy, wid,
				  XmNbuttonRenderTable, XmCButtonRenderTable,
				  from, to));
}

/*ARGSUSED*/
static Boolean
CvtStringToLabelRenderTable(Display *dpy,
			       XrmValue *args,
			       Cardinal *num_args, /* unused */
			       XrmValue *from,
			       XrmValue *to,
			       XtPointer *converter_data) /* unused */
{
  Widget wid;

  if (dpy == NULL || args == NULL || args[0].addr == NULL ||
      from == NULL || to == NULL)
    return FALSE;
  
  wid = *(Widget *)args[0].addr;
  if (wid == NULL)
    return FALSE;

  return(cvtStringToXmRenderTable(dpy, wid,
				  XmNlabelRenderTable, XmCLabelRenderTable,
				  from, to));
}

/*ARGSUSED*/
static Boolean
CvtStringToTextRenderTable(Display *dpy,
			      XrmValue *args,
			      Cardinal *num_args, /* unused */
			      XrmValue *from,
			      XrmValue *to,
			      XtPointer *converter_data) /* unused */
{
  Widget wid;

  if (dpy == NULL || args == NULL || args[0].addr == NULL ||
      from == NULL || to == NULL)
    return FALSE;
  
  wid = *(Widget *)args[0].addr;
  if (wid == NULL)
    return FALSE;

  return(cvtStringToXmRenderTable(dpy, wid,
				  XmNtextRenderTable, XmCTextRenderTable,
				  from, to));
}

/*ARGSUSED*/
static void
CvtStringToXmRenderTableDestroy(XtAppContext app, /* unused */
				 XrmValue *to,
				 XtPointer converter_data, /* unused */
				 XrmValue *args, /* unused */
				 Cardinal *num_args) /* unused */
{   
  if (to == NULL || to->addr == NULL)
    return;
  XmRenderTableFree(*((XmRenderTable *)to->addr));
}
