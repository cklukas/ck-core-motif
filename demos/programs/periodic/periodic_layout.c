/* Application layout only: control fonts, colors and drawing belong to Xm. */
#include "periodic_layout.h"
#include <Xm/Appearance.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/ScrolledW.h>
#include <limits.h>
#include <string.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ComboBox.h>

#define COLUMN_COUNT 8
#define ROW_COUNT 5

typedef struct {
    Widget widget;
    unsigned column, row, columns, rows;
    Dimension width, height;
} OverviewCell;

typedef struct {
    Widget table, clip, scroll, title, subtitle, main_window;
    OverviewCell *cells;
    Cardinal count;
    unsigned columns[COLUMN_COUNT], rows[ROW_COUNT];
    unsigned original_columns[COLUMN_COUNT], original_rows[ROW_COUNT];
    Dimension gap, padding;
    Dimension viewport_width, viewport_height;
    XtIntervalId pending;
} OverviewLayout;

static unsigned
Preferred(Widget widget, Boolean horizontal)
{
    XtWidgetGeometry reply = {0};
    Dimension current;
    XtQueryGeometry(widget, NULL, &reply);
    if (reply.request_mode & (horizontal ? CWWidth : CWHeight))
        return horizontal ? reply.width : reply.height;
    XtVaGetValues(widget, horizontal ? XmNwidth : XmNheight, &current, NULL);
    return current;
}

static void
FitSpan(unsigned *sizes, unsigned start, unsigned count, unsigned required,
        unsigned gap)
{
    unsigned available = gap * (count - 1);
    for (unsigned i = start; i < start + count; ++i) available += sizes[i];
    if (available < required) {
        unsigned missing = required - available;
        for (unsigned i = 0; i < count; ++i)
            sizes[start+i] += missing/count + (i < missing%count);
    }
}

static void
Place(Widget widget, unsigned x, unsigned y, unsigned width, unsigned height)
{
    /* Opposite-form offsets pin both edges without percentage rounding. */
    XtVaSetValues(widget,
        XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, x,
        XmNrightAttachment, XmATTACH_OPPOSITE_FORM, XmNrightOffset, -(int)(x+width),
        XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, y,
        XmNbottomAttachment, XmATTACH_OPPOSITE_FORM, XmNbottomOffset, -(int)(y+height),
        XmNwidth, width, XmNheight, height, NULL);
}

static void
Layout(OverviewLayout *layout)
{
    unsigned columns[COLUMN_COUNT], rows[ROW_COUNT];
    unsigned xs[COLUMN_COUNT+1], ys[ROW_COUNT+1], width, height;
    Dimension viewport_width, viewport_height;
    memcpy(columns, layout->columns, sizeof(columns));
    memcpy(rows, layout->rows, sizeof(rows));
    XtVaGetValues(layout->clip, XmNwidth, &viewport_width,
                  XmNheight, &viewport_height, NULL);
    if(layout->viewport_width==viewport_width && layout->viewport_height==viewport_height)
        return;
    layout->viewport_width=viewport_width;
    layout->viewport_height=viewport_height;
    width = 2*layout->padding + (COLUMN_COUNT-1)*layout->gap;
    height = 2*layout->padding + (ROW_COUNT-1)*layout->gap;
    for (unsigned i=0; i<COLUMN_COUNT; ++i) width += columns[i];
    for (unsigned i=0; i<ROW_COUNT; ++i) height += rows[i];
    if (width < viewport_width) {
        unsigned extra = viewport_width-width;
        for (unsigned i=0; i<COLUMN_COUNT; ++i)
            columns[i] += extra/COLUMN_COUNT + (i < extra%COLUMN_COUNT);
        width = viewport_width;
    }
    if (height < viewport_height) {
        /* Extra space belongs to the lower navigation demonstrations. */
        unsigned extra = viewport_height-height;
        rows[3] += extra/2; rows[4] += extra-extra/2;
        height = viewport_height;
    }
    if (width > USHRT_MAX || height > USHRT_MAX) {
        XtAppWarning(XtWidgetToApplicationContext(layout->table),
                     "Periodic overview exceeds X geometry limits");
        return;
    }
    xs[0]=ys[0]=layout->padding;
    for(unsigned i=0;i<COLUMN_COUNT;++i) xs[i+1]=xs[i]+columns[i]+layout->gap;
    for(unsigned i=0;i<ROW_COUNT;++i) ys[i+1]=ys[i]+rows[i]+layout->gap;
    XtVaSetValues(layout->table,XmNresizePolicy,XmRESIZE_NONE,
                  XmNwidth,width,XmNheight,height,NULL);
    for(Cardinal i=0;i<layout->count;++i) {
        OverviewCell *cell=&layout->cells[i];
        Place(cell->widget,xs[cell->column],ys[cell->row],
              xs[cell->column+cell->columns]-xs[cell->column]-layout->gap,
              ys[cell->row+cell->rows]-ys[cell->row]-layout->gap);
    }
    {
        unsigned title_height=Preferred(layout->title,False);
        unsigned subtitle_height=Preferred(layout->subtitle,False);
        unsigned header_width=xs[7]-xs[1]-layout->gap;
        unsigned header_height=title_height+layout->gap+subtitle_height;
        unsigned top=ys[0]+(rows[0]>header_height?(rows[0]-header_height)/2:0);
        Place(layout->title,xs[1],top,header_width,title_height);
        Place(layout->subtitle,xs[1],top+title_height+layout->gap,
              header_width,subtitle_height);
    }
}

static void
DeferredLayout(XtPointer closure, XtIntervalId *id)
{
    OverviewLayout *layout=closure;
    (void)id; layout->pending=0;
    Layout(layout);
}

static void
ViewportChanged(Widget widget, XtPointer closure, XEvent *event, Boolean *dispatch)
{
    OverviewLayout *layout=closure;
    (void)dispatch;
    if(event->type==ConfigureNotify && !layout->pending &&
       ((unsigned)event->xconfigure.width!=layout->viewport_width ||
        (unsigned)event->xconfigure.height!=layout->viewport_height))
        layout->pending=XtAppAddTimeOut(XtWidgetToApplicationContext(widget),0,
                                       DeferredLayout,layout);
}

static void
Destroyed(Widget widget, XtPointer closure, XtPointer call)
{
    OverviewLayout *layout=closure;
    (void)widget;(void)call;
    if(layout->pending)XtRemoveTimeOut(layout->pending);
    XtRemoveEventHandler(layout->clip,StructureNotifyMask,False,ViewportChanged,layout);
    XtFree((char *)layout->cells);
    XtFree((char *)layout);
}

static void
ConfigureContentViewports(Widget widget)
{
    WidgetList children;
    Cardinal count;
    if(!widget)return;
    if(XmIsText(widget)) {
        XtVaSetValues(widget,XmNcolumns,10,XmNrows,4,NULL);
        return;
    }
    if(XmIsComboBox(widget)) {
        Widget text=NULL;
        XtVaGetValues(widget,XmNtextField,&text,NULL);
        if(text)XtVaSetValues(text,XmNcolumns,10,NULL);
        return;
    }
    if(!XtIsComposite(widget))return;
    XtVaGetValues(widget,XmNchildren,&children,XmNnumChildren,&count,NULL);
    for(Cardinal i=0;i<count;++i)ConfigureContentViewports(children[i]);
}

/* The property editor is a vertically scrolling form. Its native Column owns
 * row/label measurement; the application assigns the available viewport width. */
typedef struct {
    Widget editor, clip;
    XtIntervalId pending;
} EditorLayout;

static void
FitEditor(XtPointer closure, XtIntervalId *id)
{
    EditorLayout *layout=closure;
    Dimension available,current;
    (void)id;layout->pending=0;
    XtVaGetValues(layout->clip,XmNwidth,&available,NULL);
    XtVaGetValues(layout->editor,XmNwidth,&current,NULL);
    if(available && current!=available)
        XtVaSetValues(layout->editor,XmNwidth,available,NULL);
}

static void
EditorViewportChanged(Widget widget, XtPointer closure, XEvent *event, Boolean *dispatch)
{
    EditorLayout *layout=closure;
    (void)dispatch;
    if(event->type==ConfigureNotify && !layout->pending)
        layout->pending=XtAppAddTimeOut(XtWidgetToApplicationContext(widget),0,FitEditor,layout);
}

static void
EditorDestroyed(Widget widget, XtPointer closure, XtPointer call)
{
    EditorLayout *layout=closure;
    (void)widget;(void)call;
    if(layout->pending)XtRemoveTimeOut(layout->pending);
    XtRemoveEventHandler(layout->clip,StructureNotifyMask,False,EditorViewportChanged,layout);
    XtFree((char *)layout);
}

static void
InstallEditorLayout(Widget root)
{
    Widget scroll=XtNameToWidget(root,"*toggleButtonControlWin");
    Widget editor=XtNameToWidget(root,"*toggleButtonControls");
    EditorLayout *layout;
    if(!scroll || !editor)return;
    layout=(EditorLayout *)XtCalloc(1,sizeof(*layout));
    layout->editor=editor;
    XtVaGetValues(scroll,XmNclipWindow,&layout->clip,NULL);
    XtAddEventHandler(layout->clip,StructureNotifyMask,False,EditorViewportChanged,layout);
    XtAddCallback(editor,XmNdestroyCallback,EditorDestroyed,layout);
    FitEditor(layout,NULL);
}

void
PeriodicInstallOverviewLayout(Widget main_window)
{
    OverviewLayout *layout;
    WidgetList children;
    Cardinal count;
    Widget scroll=XtNameToWidget(main_window,"*overviewScroll");
    if(!scroll)return;
    InstallEditorLayout(main_window);
    layout=(OverviewLayout *)XtCalloc(1,sizeof(*layout));
    layout->main_window=main_window;
    XtVaSetValues(main_window,XmNuserData,layout,NULL);
    layout->scroll=scroll;
    layout->table=XtNameToWidget(scroll,"*workArea");
    layout->title=XtNameToWidget(layout->table,"*titleLabel");
    layout->subtitle=XtNameToWidget(layout->table,"*subtitleLabel");
    XtVaGetValues(scroll,XmNclipWindow,&layout->clip,NULL);
    XmGetAppearanceSpacing(XtDisplay(scroll),XmAPPEARANCE_SPACING_SMALL,&layout->gap);
    XmGetAppearanceSpacing(XtDisplay(scroll),XmAPPEARANCE_SPACING_LARGE,&layout->padding);
    XtVaGetValues(layout->table,XmNchildren,&children,XmNnumChildren,&count,NULL);
    layout->cells=(OverviewCell *)XtCalloc(count,sizeof(*layout->cells));
    /* Content budgets describe these examples, not toolkit styling. Keep
     * notebook pages/dialog examples independent of the overview's fields. */
    ConfigureContentViewports(XtNameToWidget(layout->table,"*textFrame"));
    ConfigureContentViewports(XtNameToWidget(layout->table,"*comboBoxFrame"));
    /* Measure once after realization, before assigning app layout dimensions.
     * Queries during each resize would feed allocated sizes back into minima. */
    for(Cardinal i=0;i<count;++i) if(XmIsFrame(children[i])) {
        int left,right,top,bottom;
        OverviewCell *cell;
        XtVaGetValues(children[i],XmNleftPosition,&left,XmNrightPosition,&right,
                      XmNtopPosition,&top,XmNbottomPosition,&bottom,NULL);
        if(left<0 || right>40 || top<0 || bottom>40 || right<=left || bottom<=top ||
           left%5 || right%5 || top%8 || bottom%8)
            continue;
        cell=&layout->cells[layout->count++];cell->widget=children[i];
        cell->column=left/5;cell->columns=(right-left)/5;
        cell->row=top/8;cell->rows=(bottom-top)/8;
        cell->width=Preferred(cell->widget,True);
        cell->height=Preferred(cell->widget,False);
        /* A canvas or separator has no intrinsic content size. Give those
         * demonstrations a deliberate sample area instead of treating their
         * initial percentage allocation as a preferred size. */
        if (!strcmp(XtName(cell->widget),"separatorFrame") ||
            !strcmp(XtName(cell->widget),"drawBtnFrame") ||
            !strcmp(XtName(cell->widget),"drawFrame") ||
            !strcmp(XtName(cell->widget),"arrowFrame")) {
            WidgetList parts; Cardinal part_count;
            unsigned title_width=0,title_height=0;
            Dimension unit;
            XmGetAppearanceSpacing(XtDisplay(scroll),XmAPPEARANCE_SPACING_XL,&unit);
            XtVaGetValues(cell->widget,XmNchildren,&parts,XmNnumChildren,&part_count,NULL);
            for(Cardinal j=0;j<part_count;++j) {
                unsigned char type;
                XtVaGetValues(parts[j],XmNframeChildType,&type,NULL);
                if(type==XmFRAME_TITLE_CHILD) {
                    title_width=Preferred(parts[j],True);
                    title_height=Preferred(parts[j],False);
                }
            }
            cell->width=title_width+2*layout->gap;
            if(cell->width<6*unit)cell->width=6*unit;
            cell->height=title_height+2*layout->gap+
                (!strcmp(XtName(cell->widget),"separatorFrame")?unit:6*unit);
        }
    }
    /* Single cells establish tracks before spanning notebook/tab demonstrations. */
    for(unsigned pass=0;pass<2;++pass)
        for(Cardinal i=0;i<layout->count;++i) {
            OverviewCell *cell=&layout->cells[i];
            if((cell->columns>1 || cell->rows>1)!=(pass!=0))continue;
            FitSpan(layout->columns,cell->column,cell->columns,cell->width,layout->gap);
            FitSpan(layout->rows,cell->row,cell->rows,cell->height,layout->gap);
        }
    FitSpan(layout->columns,1,6,Preferred(layout->title,True),layout->gap);
    FitSpan(layout->columns,1,6,Preferred(layout->subtitle,True),layout->gap);
    FitSpan(layout->rows,0,1,Preferred(layout->title,False)+layout->gap+
            Preferred(layout->subtitle,False),layout->gap);
    memcpy(layout->original_columns,layout->columns,sizeof(layout->columns));
    memcpy(layout->original_rows,layout->rows,sizeof(layout->rows));
    XtAddEventHandler(layout->clip,StructureNotifyMask,False,ViewportChanged,layout);
    XtAddCallback(layout->table,XmNdestroyCallback,Destroyed,layout);
    Layout(layout);
    /* Realization may choose a text field using the old UIL fractions. Start
     * keyboard navigation at the first interactive example in table order. */
    {
        Widget first=XtNameToWidget(layout->table,"*arrowB1");
        if(first)XmProcessTraversal(first,XmTRAVERSE_CURRENT);
        XmScrollVisible(scroll,layout->table,0,0);
    }
}

/* Font changes reflow from the original measured budgets, so returning to
 * Medium restores the original minima instead of retaining a grown allocation. */
void PeriodicSetOverviewFontFactor(XtPointer closure, double factor)
{
    Widget main_window=(Widget)closure;
    OverviewLayout *layout=NULL;
    XtVaGetValues(main_window,XmNuserData,&layout,NULL);
    if(!layout)return; /* Historical layout keeps its original fractions. */
    for(unsigned i=0;i<COLUMN_COUNT;i++)layout->columns[i]=(unsigned)(layout->original_columns[i]*factor+0.5);
    for(unsigned i=0;i<ROW_COUNT;i++)layout->rows[i]=(unsigned)(layout->original_rows[i]*factor+0.5);
    layout->viewport_width=layout->viewport_height=0;
    Layout(layout);
}
