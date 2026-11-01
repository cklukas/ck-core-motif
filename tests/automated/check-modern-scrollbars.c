/* Exercise native thumb pixels, input, resource ownership and live restoration. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/ScrollBarP.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static unsigned callbacks;
static void changed(Widget w,XtPointer client,XtPointer call) {
    (void)w;(void)client;CHECK(call);++callbacks;
}
static void drain(XtAppContext app,Display *d) {
    XSync(d,False);while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll);XSync(d,False);
}
static void paint(XmScrollBarWidget s,Boolean disabled) {
    XImage *im;int x=s->scrollBar.slider_x,y=s->scrollBar.slider_y;
    int width=s->scrollBar.slider_width,height=s->scrollBar.slider_height;
    XSync(XtDisplay(s),False);
    im=XGetImage(XtDisplay(s),XtWindow(s),0,0,XtWidth(s),XtHeight(s),AllPlanes,ZPixmap);CHECK(im);
    CHECK(XGetPixel(im,x,y)==s->scrollBar.trough_color);
    CHECK(XGetPixel(im,x+width/2,y+height/2)==(disabled?s->primitive.top_shadow_color:s->primitive.foreground));
    XDestroyImage(im);
}
static void input(Widget w,const char *action,int type,int x,int y) {
    XEvent ev={0};Position rx,ry;
    XtTranslateCoords(w,x,y,&rx,&ry);
    ev.xbutton.type=type;ev.xbutton.display=XtDisplay(w);ev.xbutton.window=XtWindow(w);
    ev.xbutton.button=Button1;ev.xbutton.x=x;ev.xbutton.y=y;
    ev.xbutton.x_root=rx;ev.xbutton.y_root=ry;ev.xbutton.same_screen=True;
    XtCallActionProc(w,action,&ev,NULL,0);
}
static void scrolled_consumers(XtAppContext app,Display *d,double scale) {
 for(unsigned text=0;text<2;++text) {
  Widget shell=XtAppCreateShell("consumer","ScrollAppearance",applicationShellWidgetClass,d,NULL,0);
  Widget form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,
      XmNwidth,320,XmNheight,200,XmNresizePolicy,XmRESIZE_NONE,NULL);
  Widget content;
  Arg args[4];Cardinal n=0;
  if(text) {
   XtSetArg(args[n],XmNeditMode,XmMULTI_LINE_EDIT);n++;
   XtSetArg(args[n],XmNscrollHorizontal,True);n++;
   content=XmCreateScrolledText(form,"text",args,n);
   XmTextSetString(content,"One long line for horizontal scrolling and viewport allocation.\nSecond line\nThird line\nFourth line\nFifth line");
  } else {
   XtSetArg(args[n],XmNvisibleItemCount,3);n++;
   XtSetArg(args[n],XmNlistSizePolicy,XmCONSTANT);n++;
   content=XmCreateScrolledList(form,"list",args,n);
   for(unsigned i=0;i<12;++i) {
    XmString item=XmStringCreateLocalized("A long list item for horizontal scrolling and viewport allocation");
    XmListAddItem(content,item,0);XmStringFree(item);
   }
  }
  Widget sw=XtParent(content),vb,hb;
  XtVaSetValues(sw,XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,
      XmNtopAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_FORM,NULL);
  XtManageChild(content);XtRealizeWidget(shell);drain(app,d);
  XtVaGetValues(sw,XmNverticalScrollBar,&vb,XmNhorizontalScrollBar,&hb,NULL);CHECK(vb && hb);
  CHECK(XtWidth(vb)==(Dimension)(18*scale+0.5));
  CHECK(XtHeight(hb)==(Dimension)(18*scale+0.5));
  Dimension width=XtWidth(sw),height=XtHeight(sw);
  CHECK(XmSetDisplayScaleOverride(d,1.25));drain(app,d);
  if(XtWidth(vb)!=23 || XtHeight(hb)!=23)
   fprintf(stderr,"consumer %s bars=%ux%u expected thickness=23\n",text?"Text":"List",XtWidth(vb),XtHeight(hb));
  CHECK(XtWidth(vb)==23 && XtHeight(hb)==23);
  if(XtWidth(sw)!=width || XtHeight(sw)!=height)
   fprintf(stderr,"consumer %s outer=%ux%u before=%ux%u shell=%ux%u\n",text?"Text":"List",XtWidth(sw),XtHeight(sw),width,height,XtWidth(shell),XtHeight(shell));
  CHECK(XtWidth(sw)==width && XtHeight(sw)==height);
  CHECK(XtX(content)>=0 && XtY(content)>=0);
  CHECK(XtWidth(content)>0 && XtHeight(content)>0);
  CHECK(XtX(content)+XtWidth(content)<=XtX(vb));
  CHECK(XtY(content)+XtHeight(content)<=XtY(hb));
  CHECK(XmSetDisplayScaleOverride(d,scale));drain(app,d);
  CHECK(XtWidth(vb)==(Dimension)(18*scale+0.5));
  CHECK(XtHeight(hb)==(Dimension)(18*scale+0.5));
  const Dimension tiny[][2]={{32,32},{16,16},{1,1},{1,100},{100,1},{320,200}};
  for(unsigned t=0;t<XtNumber(tiny);++t) {
   XtResizeWidget(form,tiny[t][0],tiny[t][1],0);
   XSync(d,False);
   CHECK(XtWidth(sw)==tiny[t][0] && XtHeight(sw)==tiny[t][1]);
   /* Both consumers contain lines wider than any allocation in this matrix.
    * Tiny inner widths must not masquerade as a large unsigned extent. */
   CHECK(XtIsManaged(hb));
   CHECK(XtX(content)>=0 && XtY(content)>=0);
   CHECK(XtWidth(content)>0 && XtHeight(content)>0);
   CHECK(XtX(content)+XtWidth(content)<=XtWidth(sw));
   CHECK(XtY(content)+XtHeight(content)<=XtHeight(sw));
   Widget bars[]={hb,vb};
   for(unsigned a=0;a<2;++a) {
    Widget bar=bars[a];
    if(!XtIsManaged(bar)) continue;
    if(t+1==XtNumber(tiny)) CHECK(XtX(bar)<XtWidth(sw) && XtY(bar)<XtHeight(sw));
    if(XtX(bar)>=XtWidth(sw) || XtY(bar)>=XtHeight(sw)) continue;
    CHECK(XtX(bar)>=0 && XtY(bar)>=0);
    CHECK(XtX(bar)+XtWidth(bar)<=XtWidth(sw));
    CHECK(XtY(bar)+XtHeight(bar)<=XtHeight(sw));
    CHECK(a ? XtHeight(bar)==XtHeight(content) : XtWidth(bar)==XtWidth(content));
    CHECK(XtX(bar)+XtWidth(bar)<=XtX(content) ||
          XtX(content)+XtWidth(content)<=XtX(bar) ||
          XtY(bar)+XtHeight(bar)<=XtY(content) ||
          XtY(content)+XtHeight(content)<=XtY(bar));
   }
  }
  if(!text) {
   XmListDeleteAllItems(content);
   CHECK(!XtIsManaged(hb));
   XmString item=XmStringCreateLocalized("A long restored item for horizontal scrollbar visibility after emptying the list");
   XmListAddItem(content,item,0);XmStringFree(item);
   CHECK(XtIsManaged(hb));
  }
  XtDestroyWidget(shell);drain(app,d);
 }
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;unsigned cases=0;
    const double scales[]={1,1.5,2};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    d=XtOpenDisplay(app,NULL,"scrollAppearance","ScrollAppearance",NULL,0,&argc,argv);CHECK(d);
    for(int scale=0;scale<3;++scale) for(int dark=0;dark<2;++dark)
    for(int horizontal=0;horizontal<2;++horizontal) for(int reversed=0;reversed<2;++reversed) {
        Widget shell,board,w,explicit_bar;XmScrollBarWidget s;unsigned char classic_arrows,classic_visual;
        Pixel classic_trough;int old,value,size,inc,page,cx,cy;
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));
        shell=XtAppCreateShell("scroll","ScrollAppearance",applicationShellWidgetClass,d,NULL,0);
        board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,700,XmNheight,500,NULL);
        w=XtVaCreateManagedWidget("scroll",xmScrollBarWidgetClass,board,
            XmNx,20,XmNy,20,XmNwidth,horizontal?360:32,XmNheight,horizontal?32:360,
            XmNorientation,horizontal?XmHORIZONTAL:XmVERTICAL,
            XmNprocessingDirection,horizontal?(reversed?XmMAX_ON_LEFT:XmMAX_ON_RIGHT):(reversed?XmMAX_ON_TOP:XmMAX_ON_BOTTOM),
            XmNminimum,0,XmNmaximum,100,XmNsliderSize,20,XmNvalue,40,NULL);
        s=(XmScrollBarWidget)w;classic_arrows=s->scrollBar.show_arrows;classic_visual=s->scrollBar.etched_slider;classic_trough=s->scrollBar.trough_color;
        explicit_bar=XtVaCreateManagedWidget("explicit",xmScrollBarWidgetClass,board,
            XmNx,420,XmNy,20,XmNheight,200,XmNwidth,32,
            XmNshowArrows,XmNONE,XmNsliderVisual,XmBACKGROUND_COLOR,
            XmNtroughColor,BlackPixelOfScreen(XtScreen(w)),NULL);
        XtAddCallback(w,XmNvalueChangedCallback,changed,NULL);
        XtRealizeWidget(shell);drain(app,d);
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));drain(app,d);
        CHECK(s->scrollBar.show_arrows==XmEACH_SIDE);CHECK(s->primitive.shadow_thickness==0);
        CHECK(s->scrollBar.etched_slider==XmFOREGROUND_COLOR);paint(s,False);
        CHECK(((XmScrollBarWidget)explicit_bar)->scrollBar.show_arrows==XmNONE);
        CHECK(((XmScrollBarWidget)explicit_bar)->scrollBar.etched_slider==XmBACKGROUND_COLOR);
        CHECK(((XmScrollBarWidget)explicit_bar)->scrollBar.trough_color==BlackPixelOfScreen(XtScreen(w)));
        {
            XEvent key={0};String parameter=horizontal?"left":"up";
            key.xkey.type=KeyPress;key.xkey.display=d;key.xkey.window=XtWindow(w);
            old=s->scrollBar.value;
            XtCallActionProc(w,"IncrementUpOrLeft",&key,&parameter,1);drain(app,d);
            CHECK(s->scrollBar.value!=old);paint(s,False);
        }
        /* Default end arrows must operate, not merely reserve geometry. */
        for (unsigned arrow=0; arrow<2; ++arrow) {
            int ax=(arrow?s->scrollBar.arrow2_x:s->scrollBar.arrow1_x)+s->scrollBar.arrow_width/2;
            int ay=(arrow?s->scrollBar.arrow2_y:s->scrollBar.arrow1_y)+s->scrollBar.arrow_height/2;
            old=s->scrollBar.value;
            input(w,"Select",ButtonPress,ax,ay);
            input(w,"Release",ButtonRelease,ax,ay);drain(app,d);
            CHECK(s->scrollBar.value!=old);
        }
        old=s->scrollBar.value;callbacks=0;
        cx=s->scrollBar.slider_x+s->scrollBar.slider_width/2;cy=s->scrollBar.slider_y+s->scrollBar.slider_height/2;
        input(w,"Select",ButtonPress,cx,cy);
        input(w,"Moved",MotionNotify,cx+(horizontal?30:0),cy+(horizontal?0:30));
        input(w,"Release",ButtonRelease,cx+(horizontal?30:0),cy+(horizontal?0:30));drain(app,d);
        CHECK(s->scrollBar.value!=old);CHECK(callbacks==1);paint(s,False);
        XmScrollBarSetValues(w,80,20,1,10,False);drain(app,d);paint(s,False);
        XmScrollBarGetValues(w,&value,&size,&inc,&page);CHECK(value==80&&size==20&&inc==1&&page==10);
        XmScrollBarSetValues(w,40,1,1,10,False);drain(app,d);paint(s,False);
        CHECK((horizontal?s->scrollBar.slider_width:s->scrollBar.slider_height)>=(int)(24*scales[scale]));
        XtSetSensitive(w,False);drain(app,d);paint(s,True);
        XmScrollBarSetValues(w,10,20,1,10,False);drain(app,d);paint(s,True);
        XtSetSensitive(w,True);drain(app,d);paint(s,False);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);
        CHECK(s->scrollBar.show_arrows==classic_arrows);CHECK(s->scrollBar.etched_slider==classic_visual);CHECK(s->scrollBar.trough_color==classic_trough);
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));drain(app,d);paint(s,False);
        /* Default thickness follows scale/density; application dimensions win. */
        {
            Widget automatic=XtVaCreateManagedWidget("automatic",xmScrollBarWidgetClass,board,
                XmNx,560,XmNy,20,XmNorientation,horizontal?XmHORIZONTAL:XmVERTICAL,NULL);
            Dimension thickness;
            const char *axis=horizontal?XmNheight:XmNwidth;
            XtVaGetValues(automatic,axis,&thickness,NULL);
            CHECK(thickness==(Dimension)(18*scales[scale]+0.5));
            CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMPACT));drain(app,d);
            XtVaGetValues(automatic,axis,&thickness,NULL);
            CHECK(thickness==(Dimension)(16*scales[scale]+0.5));
            CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));drain(app,d);
            XtVaGetValues(automatic,axis,&thickness,NULL);
            CHECK(thickness==(Dimension)(18*scales[scale]+0.5));
            Widget scrolled=XtVaCreateManagedWidget("scrolled",xmScrolledWindowWidgetClass,board,
                XmNx,20,XmNy,410,XmNwidth,300,XmNheight,70,XmNscrollingPolicy,XmAUTOMATIC,NULL);
            XtVaCreateManagedWidget("content",xmDrawingAreaWidgetClass,scrolled,
                XmNwidth,600,XmNheight,600,NULL);
            Widget vb,hb;
            XtVaGetValues(scrolled,XmNverticalScrollBar,&vb,XmNhorizontalScrollBar,&hb,NULL);
            CHECK(vb && hb);drain(app,d);
            XtVaGetValues(vb,XmNwidth,&thickness,NULL);CHECK(thickness==(Dimension)(18*scales[scale]+0.5));
            XtVaGetValues(hb,XmNheight,&thickness,NULL);CHECK(thickness==(Dimension)(18*scales[scale]+0.5));
            Dimension outer_width=XtWidth(scrolled),outer_height=XtHeight(scrolled);
            XtWidgetGeometry query={0},reply={0};
            query.request_mode=CWWidth|XtCWQueryOnly;query.width=XtWidth(vb)+1;
            Dimension before_width=XtWidth(vb);
            CHECK(XtMakeGeometryRequest(vb,&query,&reply)==XtGeometryYes);
            CHECK(XtWidth(vb)==before_width && XtWidth(scrolled)==outer_width && XtHeight(scrolled)==outer_height);
            CHECK(XmSetDisplayScaleOverride(d,1.25));drain(app,d);
            XtVaGetValues(automatic,axis,&thickness,NULL);CHECK(thickness==23);
            XtVaGetValues(vb,XmNwidth,&thickness,NULL);CHECK(thickness==23);
            XtVaGetValues(hb,XmNheight,&thickness,NULL);CHECK(thickness==23);
            CHECK(XtWidth(scrolled)==outer_width && XtHeight(scrolled)==outer_height);
            CHECK(XmSetDisplayScaleOverride(d,scales[scale]));drain(app,d);
            XtVaGetValues(automatic,axis,&thickness,NULL);CHECK(thickness==(Dimension)(18*scales[scale]+0.5));
            XtDestroyWidget(scrolled);drain(app,d);
            XtVaSetValues(automatic,axis,37,NULL);
            CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMPACT));drain(app,d);
            XtVaGetValues(automatic,axis,&thickness,NULL);CHECK(thickness==37);
            CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));drain(app,d);
            XtVaSetValues(automatic,axis,0,NULL);drain(app,d);
            XtVaGetValues(automatic,axis,&thickness,NULL);CHECK(thickness==(Dimension)(18*scales[scale]+0.5));
            XtVaSetValues(automatic,XmNorientation,horizontal?XmVERTICAL:XmHORIZONTAL,NULL);drain(app,d);
            const char *turned_axis=horizontal?XmNwidth:XmNheight;
            XtVaGetValues(automatic,turned_axis,&thickness,NULL);CHECK(thickness==(Dimension)(18*scales[scale]+0.5));
            XtVaSetValues(automatic,turned_axis,39,NULL);
            XtVaSetValues(automatic,XmNorientation,horizontal?XmHORIZONTAL:XmVERTICAL,NULL);
            XtVaSetValues(automatic,XmNorientation,horizontal?XmVERTICAL:XmHORIZONTAL,NULL);drain(app,d);
            XtVaGetValues(automatic,turned_axis,&thickness,NULL);CHECK(thickness==39);
        }
        {
            const unsigned char modes[]={XmNONE,XmEACH_SIDE,XmMIN_SIDE,XmMAX_SIDE};
            const Dimension lengths[]={1,2,3,5,8,16,32,80};
            for(unsigned mode=0;mode<XtNumber(modes);++mode) {
                XtVaSetValues(w,XmNshowArrows,modes[mode],NULL);
                for(unsigned length=0;length<XtNumber(lengths);++length) {
                    XtResizeWidget(w,horizontal?lengths[length]:18,horizontal?18:lengths[length],0);
                    int major=lengths[length];
                    int first=horizontal?s->scrollBar.arrow1_x:s->scrollBar.arrow1_y;
                    int second=horizontal?s->scrollBar.arrow2_x:s->scrollBar.arrow2_y;
                    int arrow=horizontal?s->scrollBar.arrow_width:s->scrollBar.arrow_height;
                    int track=horizontal?s->scrollBar.slider_area_x:s->scrollBar.slider_area_y;
                    int span=horizontal?s->scrollBar.slider_area_width:s->scrollBar.slider_area_height;
                    if(track<0 || track+span>major || (arrow && (first<0 || second<0 || first+arrow>major || second+arrow>major)))
                        fprintf(stderr,"tiny scrollbar mode=%u horizontal=%d major=%d arrows=%d,%d extent=%d track=%d+%d\n",modes[mode],horizontal,major,first,second,arrow,track,span);
                    CHECK(track>=0 && span>0 && track+span<=major);
                    if(arrow) {
                        CHECK(first>=0 && second>=0 && first+arrow<=major && second+arrow<=major);
                        CHECK(first+arrow<=second || second+arrow<=first);
                        CHECK(first+arrow<=track || track+span<=first);
                        CHECK(second+arrow<=track || track+span<=second);
                        for(unsigned which=0;which<2;++which) {
                            int x=(which?s->scrollBar.arrow2_x:s->scrollBar.arrow1_x)+s->scrollBar.arrow_width/2;
                            int y=(which?s->scrollBar.arrow2_y:s->scrollBar.arrow1_y)+s->scrollBar.arrow_height/2;
                            XmScrollBarSetValues(w,40,1,1,10,False);
                            input(w,"Select",ButtonPress,x,y);
                            CHECK(which?s->scrollBar.arrow2_selected:s->scrollBar.arrow1_selected);
                            CHECK(!(which?s->scrollBar.arrow1_selected:s->scrollBar.arrow2_selected));
                            input(w,"Release",ButtonRelease,x,y);
                        }
                    }
                }
            }
            XtVaSetValues(w,XmNshowArrows,XmEACH_SIDE,NULL);
            XtResizeWidget(w,1,1,0);
            CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);
            CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));drain(app,d);
            CHECK(s->scrollBar.slider_area_x>=0 && s->scrollBar.slider_area_y>=0);
            CHECK(s->scrollBar.slider_area_x+s->scrollBar.slider_area_width<=XtWidth(w));
            CHECK(s->scrollBar.slider_area_y+s->scrollBar.slider_area_height<=XtHeight(w));
            XtResizeWidget(w,horizontal?360:32,horizontal?32:360,0);
        }
        scrolled_consumers(app,d,scales[scale]);
        /* Construction under the modern profile must match live conversion. */
        {
            Widget startup=XtVaCreateManagedWidget("startup",xmScrollBarWidgetClass,board,
                XmNx,500,XmNy,20,XmNwidth,32,XmNheight,200,NULL);
            XmScrollBarWidget t=(XmScrollBarWidget)startup;
            drain(app,d);CHECK(t->scrollBar.show_arrows==XmEACH_SIDE);
            CHECK(t->scrollBar.etched_slider==XmFOREGROUND_COLOR);paint(t,False);
        }
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_HIGH_CONTRAST));drain(app,d);
        CHECK(s->scrollBar.show_arrows==classic_arrows);
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    printf("PASS modern scrollbars: %u cases\n",cases);XtCloseDisplay(d);XtDestroyApplicationContext(app);return 0;
}
