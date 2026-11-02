#include <X11/IntrinsicP.h>
/* Internal acceptance mode for the production gallery. No desktop input is sent. */
static void audit_json_string(FILE *file, const char *text)
{
    fputc('"',file);
    for (const unsigned char *p=(const unsigned char *)(text?text:"");*p;++p) {
        if (*p=='"'||*p=='\\') {fputc('\\',file);fputc(*p,file);}
        else if (*p<32) fprintf(file,"\\u%04x",*p);
        else fputc(*p,file);
    }
    fputc('"',file);
}
static Boolean audit_has_resource(Widget widget,const char *name)
{
    XtResourceList resources=NULL;Cardinal count=0;Boolean found=False;
    XtGetResourceList(XtClass(widget),&resources,&count);
    for(Cardinal i=0;i<count;i++)if(!strcmp(resources[i].resource_name,name))found=True;
    XtFree((char*)resources);
    if(!found && XmIsGadget(widget)) {
        XmSecondaryResourceData *blocks=NULL;Cardinal n=XmGetSecondaryResourceData(XtClass(widget),&blocks);
        for(Cardinal i=0;i<n;i++) {
            for(Cardinal j=0;j<blocks[i]->num_resources;j++)if(!strcmp(blocks[i]->resources[j].resource_name,name))found=True;
            XtFree((char*)blocks[i]->resources);XtFree((char*)blocks[i]);
        }
        XtFree((char*)blocks);
    }
    return found;
}
static void audit_issue(FILE *file,const char *kind,Widget widget)
{
    fprintf(file,"{\"kind\":");audit_json_string(file,kind);
    fprintf(file,",\"name\":");audit_json_string(file,XtName(widget));
    fprintf(file,",\"class\":");audit_json_string(file,XtClass(widget)->core_class.class_name);
    fprintf(file,"}\n");
}
static void audit_widget_tree(Widget widget,FILE *issues,unsigned *widgets,unsigned *fonts,unsigned *core_fonts,unsigned *clips)
{
    if(!widget||!XtIsRectObj(widget))return;
    ++*widgets;
    if(audit_has_resource(widget,XmNrenderTable)) {
        XmRenderTable table=NULL;XmStringTag *tags=NULL;XtVaGetValues(widget,XmNrenderTable,&table,NULL);
        int count=XmRenderTableGetTags(table,&tags);
        for(int i=0;i<count;i++) {
            XmRendition rendition=XmRenderTableGetRendition(table,tags[i]);
            if(rendition){XmFontType type;Arg arg;XtSetArg(arg,XmNfontType,&type);XmRenditionRetrieve(rendition,&arg,1);++*fonts;
                if(type!=XmFONT_IS_XFT){++*core_fonts;audit_issue(issues,"core-font",widget);}XmRenditionFree(rendition);}
            XtFree(tags[i]);
        }
        XtFree((char*)tags);
        Widget view=XtIsWidget(widget)?widget:XtParent(widget);XWindowAttributes visible;
        Boolean mapped_when_managed=True;
        if(XtIsWidget(widget))XtVaGetValues(widget,XtNmappedWhenManaged,&mapped_when_managed,NULL);
        Boolean mapped=mapped_when_managed&&XtIsRealized(view)&&XGetWindowAttributes(XtDisplayOfObject(view),XtWindow(view),&visible)&&visible.map_state==IsViewable;
        if(mapped&&XtIsManaged(widget)&&(XmIsLabel(widget)||XmIsLabelGadget(widget))) {
            XmString label=NULL;unsigned char type=XmSTRING;Dimension width=0,height=0,lw=0,lh=0;
            XtVaGetValues(widget,XmNlabelType,&type,XmNlabelString,&label,XmNwidth,&width,XmNheight,&height,NULL);
            if(type==XmSTRING&&label&&!XmStringEmpty(label)&&table){XmStringExtent(table,label,&lw,&lh);
                if(lw>width||lh>height){fprintf(stderr,"clipped label %s: text=%ux%u allocation=%ux%u\n",XtName(widget),lw,lh,width,height);++*clips;audit_issue(issues,"clipped-label",widget);}}
            XmStringFree(label);
        }
    }
    if(XtIsComposite(widget)) {
        WidgetList children=NULL;Cardinal count=0;XtVaGetValues(widget,XmNchildren,&children,XmNnumChildren,&count,NULL);
        for(Cardinal i=0;i<count;i++)audit_widget_tree(children[i],issues,widgets,fonts,core_fonts,clips);
    }
}
static unsigned char audit_channel(unsigned long pixel,unsigned long mask)
{
    if(!mask)return 0;
    while(!(mask&1)){mask>>=1;pixel>>=1;}
    return (unsigned char)(((pixel&mask)*255)/mask);
}
static Boolean audit_capture(Widget widget,const char *path)
{
    XWindowAttributes a;
    if(!widget||!XtIsRealized(widget)||!XGetWindowAttributes(XtDisplay(widget),XtWindow(widget),&a)||a.map_state!=IsViewable||a.width<=0||a.height<=0)return False;
    XImage *image=XGetImage(XtDisplay(widget),XtWindow(widget),0,0,(unsigned int)a.width,(unsigned int)a.height,AllPlanes,ZPixmap);
    if(!image)return False;
    FILE *file=fopen(path,"wb");if(!file){XDestroyImage(image);return False;}
    fprintf(file,"P6\n%d %d\n255\n",a.width,a.height);
    if((size_t)a.width>SIZE_MAX/3){fclose(file);XDestroyImage(image);return False;}
    unsigned char *row=malloc((size_t)a.width*3);Boolean okay=row!=NULL;
    for(int y=0;okay&&y<a.height;y++) {
        for(int x=0;x<a.width;x++){unsigned long pixel=XGetPixel(image,x,y);row[3*x]=audit_channel(pixel,image->red_mask);row[3*x+1]=audit_channel(pixel,image->green_mask);row[3*x+2]=audit_channel(pixel,image->blue_mask);}
        okay=fwrite(row,3,(size_t)a.width,file)==(size_t)a.width;
    }
    free(row);if(fclose(file))okay=False;XDestroyImage(image);return okay;
}
static int audit_modern_gallery(GalleryState *state,const char *directory)
{
    const XmAppearanceProfile profiles[]={XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK};
    const char *names[]={"modern-light","modern-dark"};
    char path[4096];FILE *report,*issues;unsigned entries=0,total_issues=0;
    if(snprintf(path,sizeof(path),"%s/report.jsonl",directory)>=(int)sizeof(path))return 2;
    report=fopen(path,"w");if(!report)return 2;
    snprintf(path,sizeof(path),"%s/issues.jsonl",directory);issues=fopen(path,"w");if(!issues){fclose(report);return 2;}
    for(unsigned p=0;p<2;p++) {
        if(!XmApplyAppearanceProfile(XtDisplay(state->shell),profiles[p])||!XmSetDisplayScaleOverride(XtDisplay(state->shell),1.0))goto failed;
        sync_appearance_controls(state);
        for(Cardinal i=0;i<GALLERY_MANIFEST_COUNT;i++) {
            const GalleryManifestEntry *entry=&gallery_manifest[i];
            show_entry(state,entry);select_page(state,2);if(!settle_gallery(state))goto failed;
            fprintf(issues,"{\"entry\":");audit_json_string(issues,entry->id);fprintf(issues,",\"profile\":\"%s\"}\n",names[p]);
            unsigned widgets=0,fonts=0,core_fonts=0,clips=0;
            audit_widget_tree(state->shell,issues,&widgets,&fonts,&core_fonts,&clips);
            if(state->component&&XmIsAutoSuggestBox(state->component)){XmAutoSuggestBoxDismiss(state->component,NULL);if(!settle_gallery(state))goto failed;}
            snprintf(path,sizeof(path),"%s/%s-%s-normal.ppm",directory,names[p],entry->id);if(!audit_capture(state->shell,path))goto failed;
            Boolean opened=False;Widget popup=NULL;
            if(state->component_is_popup){component_activate(state->action,state,NULL);opened=True;popup=state->component;}
            else if(state->component&&(XmIsFlyout(state->component)||XmIsTeachingTip(state->component))){Widget trigger=XtNameToWidget(state->component_host,"*showPopup");if(trigger){XtCallCallbacks(trigger,XmNactivateCallback,NULL);opened=True;popup=XtNameToWidget(state->shell,XmIsFlyout(state->component)?"*flyoutShell":"*teachingTipShell");}}
            else if(state->component&&XmIsAutoSuggestBox(state->component)){XmAutoSuggestBoxShow(state->component);opened=True;popup=XtNameToWidget(state->shell,"*suggestionsShell");}
            else if(state->component&&XmIsDatePicker(state->component)){XmDatePickerOpen(state->component);opened=True;popup=XtNameToWidget(state->shell,"*datePickerShell");}
            if(opened){if(!settle_gallery(state))goto failed;if(popup){audit_widget_tree(popup,issues,&widgets,&fonts,&core_fonts,&clips);snprintf(path,sizeof(path),"%s/%s-%s-popup.ppm",directory,names[p],entry->id);if(!audit_capture(popup,path))goto failed;}
                if(state->component_is_popup)XtPopdown(state->component);else if(XmIsFlyout(state->component))XmFlyoutHide(state->component);else if(XmIsTeachingTip(state->component))XmTeachingTipHide(state->component);else if(XmIsDatePicker(state->component))XmDatePickerClose(state->component);else if(XmIsAutoSuggestBox(state->component))XmAutoSuggestBoxDismiss(state->component,NULL);}
            Boolean focused=False;
            if(state->component&&!state->component_is_popup&&XtIsManaged(state->component)) {
                XSetInputFocus(XtDisplay(state->shell),XtWindow(state->shell),RevertToParent,CurrentTime);
                focused=XmProcessTraversal(state->component,XmTRAVERSE_CURRENT);
                if(focused){if(!settle_gallery(state))goto failed;snprintf(path,sizeof(path),"%s/%s-%s-focus.ppm",directory,names[p],entry->id);if(!audit_capture(state->shell,path))goto failed;}
            }
            if(state->component_host){Boolean sensitive=XtIsSensitive(state->component_host);XtSetSensitive(state->component_host,False);if(!settle_gallery(state))goto failed;snprintf(path,sizeof(path),"%s/%s-%s-disabled.ppm",directory,names[p],entry->id);if(!audit_capture(state->shell,path))goto failed;XtSetSensitive(state->component_host,sensitive);}
            fprintf(report,"{\"id\":");audit_json_string(report,entry->id);fprintf(report,",\"profile\":\"%s\",\"constructible\":%s,\"available\":%s,\"widgets\":%u,\"font_renditions\":%u,\"core_fonts\":%u,\"clipped_labels\":%u,\"popup_exercised\":%s}\n",names[p],entry->constructible?"true":"false",entry->class?"true":"false",widgets,fonts,core_fonts,clips,opened?"true":"false");fflush(report);fflush(issues);++entries;total_issues+=core_fonts+clips;
            printf("audit %s %s fonts=%u core=%u clipped=%u\n",names[p],entry->id,fonts,core_fonts,clips);fflush(stdout);
        }
    }
    fclose(report);fclose(issues);XmApplyAppearanceProfile(XtDisplay(state->shell),XmAPPEARANCE_MODERN_LIGHT);select_page(state,1);printf("%s gallery modern audit: %u entry/profile checks, %u font/clipping issues\n",total_issues?"FAIL":"PASS",entries,total_issues);return total_issues?1:0;
failed:
    fclose(report);fclose(issues);fprintf(stderr,"gallery modern audit failed after %u entries\n",entries);return 1;
}
