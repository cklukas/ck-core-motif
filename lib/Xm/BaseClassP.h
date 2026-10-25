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
#ifndef _XmBaseClassP_h
#define _XmBaseClassP_h

#ifndef _XmNO_BC_INCL
#define _XmNO_BC_INCL
#endif

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif


#define _XmBCEPTR(wc)	((XmBaseClassExt *)(&(((WidgetClass)(wc))\
					      ->core_class.extension)))
#define _XmBCE(wc)	((XmBaseClassExt)(((WidgetClass)(wc))\
					  ->core_class.extension))

#define _XmGetBaseClassExtPtr(wc, owner) \
    ((_XmBCE(wc) && (((_XmBCE(wc))->record_type) == owner)) ? \
     _XmBCEPTR(wc) :  \
     ((XmBaseClassExt *) _XmGetClassExtensionPtr( \
						 ((XmGenericClassExt *)  \
						  _XmBCEPTR( wc)),  \
						 owner)))

/* defines for 256 bit (at least) bit field
 */
#define _XmGetFlagsBit(field, bit) \
	(field[ (bit >> 3) ]) & (1 << (bit & 0x07))

#define _XmSetFlagsBit(field, bit) \
	    (field[ (bit >> 3) ] |= (1 << (bit & 0x07)))



#define XmBaseClassExtVersion 2L
#define XmBaseClassExtVersion 2L


typedef Cardinal (*XmGetSecResDataFunc)( WidgetClass,
					    XmSecondaryResourceData **);

typedef struct _XmObjectClassExtRec{
    XtPointer 		next_extension;	
    XrmQuark 		record_type;	
    long 		version;	
    Cardinal 		record_size;	
} XmObjectClassExtRec, *XmObjectClassExt;

typedef struct _XmGenericClassExtRec{
    XtPointer 		next_extension;	
    XrmQuark 		record_type;	
    long 		version;	
    Cardinal 		record_size;	
} XmGenericClassExtRec, *XmGenericClassExt;

typedef struct _XmWrapperDataRec{
    struct _XmWrapperDataRec *next;
    WidgetClass		widgetClass;
    XtInitProc		initializeLeaf;
    XtSetValuesFunc	setValuesLeaf;
    XtArgsProc		getValuesLeaf;
    XtRealizeProc	realize;
    XtWidgetClassProc	classPartInitLeaf;
    XtWidgetProc	resize;
    XtGeometryHandler   geometry_manager;

    /* init_depth is obselete now .. */
    Cardinal		init_depth;

    int                 initializeLeafCount;
    int                 setValuesLeafCount;
    int                 getValuesLeafCount;
    XtInitProc          constraintInitializeLeaf;
    XtSetValuesFunc     constraintSetValuesLeaf;
    int                 constraintInitializeLeafCount;
    int 		constraintSetValuesLeafCount;
} XmWrapperDataRec, *XmWrapperData;

typedef struct _XmBaseClassExtRec{
    XtPointer 		next_extension;	
    XrmQuark 		record_type;	
    long 		version;	
    Cardinal 		record_size;	
    XtInitProc		initializePrehook;
    XtSetValuesFunc 	setValuesPrehook;
    XtInitProc		initializePosthook;
    XtSetValuesFunc 	setValuesPosthook;
    WidgetClass		secondaryObjectClass;
    XtInitProc		secondaryObjectCreate;
    XmGetSecResDataFunc	getSecResData;
    unsigned char	flags[32];
    XtArgsProc		getValuesPrehook;
    XtArgsProc		getValuesPosthook;
    XtWidgetClassProc	classPartInitPrehook;
    XtWidgetClassProc	classPartInitPosthook;
    XtResourceList	ext_resources;
    XtResourceList	compiled_ext_resources;
    Cardinal		num_ext_resources;
    Boolean		use_sub_resources;
    XmWidgetNavigableProc widgetNavigable;
    XmFocusChangeProc	focusChange;
    XmWrapperData	wrapperData;
} XmBaseClassExtRec, *XmBaseClassExt;



typedef struct _XmWidgetExtDataRec{
    Widget		widget;
    Widget		reqWidget;
    Widget		oldWidget;
} XmWidgetExtDataRec, *XmWidgetExtData;

externalref XrmQuark	     XmQmotif;
externalref int		     _XmInheritClass;
externalref XmBaseClassExt * _Xm_fastPtr;
  
/********    Private Function Declarations    ********/


extern XmGenericClassExt * _XmGetClassExtensionPtr( 
                        XmGenericClassExt *listHeadPtr,
                        XrmQuark owner) ;
extern Boolean _XmIsSubclassOf(WidgetClass wc, WidgetClass sc);


#ifndef XTHREADS
/* Keep the fast-subclass lookup local to each call.  The historical macros
 * reused the process-global _Xm_fastPtr while also dereferencing it in the
 * same expression, which leaves evaluation order undefined and triggers
 * sequence-point diagnostics on modern compilers. */
static inline void
_XmFastSubclassInitLocal(WidgetClass wc, unsigned int bit_field)
{
    XmBaseClassExt *fastPtr = _XmGetBaseClassExtPtr(wc, XmQmotif);

    if (fastPtr && *fastPtr)
        _XmSetFlagsBit((*fastPtr)->flags, bit_field);
}

static inline Boolean
_XmIsFastSubclassLocal(WidgetClass wc, unsigned int bit)
{
    XmBaseClassExt *fastPtr = _XmGetBaseClassExtPtr(wc, XmQmotif);

    if (!fastPtr || !*fastPtr)
        return FALSE;
    return _XmGetFlagsBit((*fastPtr)->flags, bit) ? TRUE : FALSE;
}

#define _XmFastSubclassInit(wc, bit_field) \
    _XmFastSubclassInitLocal((wc), (bit_field))
#define _XmIsFastSubclass(wc, bit) \
    _XmIsFastSubclassLocal((wc), (bit))

#else
extern void _XmFastSubclassInit(WidgetClass, unsigned int);
extern Boolean _XmIsFastSubclass(WidgetClass, unsigned int);
#endif  /* XTHREADS */

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmBaseClassP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
