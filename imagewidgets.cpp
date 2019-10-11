#include "imagewidgets.h"

/* store the Gdk::Window so we can create accelerated surfaces for whatever app is rendering on */
CImageWidget::CImageWidget(Glib::RefPtr<Gdk::Window> wnd) : Glib::ObjectBase("CImageWidget"), Gtk::DrawingArea()
{
	target_window=wnd;
	w=h=1;
	
	signal_draw().connect(sigc::mem_fun(this,&CImageWidget::on_draw));
	
	//set_size_request(1,1);
}

void CImageWidget::on_unrealize()
{
	printf("unrealize event on CImageWidget %08X\n", this);
}

void CImageWidget::destroy_notify_()
{
	printf("destroy event on CImageWidget %08X\n", this);
}

void CImageWidget::SetSize(int x, int y)
{
	w=x; h=y;
	
	image = target_window->create_similar_surface(Cairo::CONTENT_COLOR_ALPHA,w,h);
	image_ctx = Cairo::Context::create(image);
	
	set_size_request(w,h);
}

void CImageWidget::Redraw()
{
	
}

bool CImageWidget::on_draw(const Cairo::RefPtr<Cairo::Context> &ctx)
{
	if(!image) Redraw();
	/* blit our internal buffer */
	ctx->set_source(image,0,0);
	ctx->rectangle(0,0,w,h);
	ctx->fill();
	
	return true;
}

CImageWidget::~CImageWidget()
{
}

#ifdef HAVE_LASEM

#include <lsm.h>
#include <lsmmathml.h>

CLatexWidget::CLatexWidget(Glib::RefPtr<Gdk::Window> wnd, Glib::ustring text) : CImageWidget(wnd)
{
	source=text;
	
	LsmDomDocument *doc;
	LsmDomView *view;
	GError *err = NULL;
	doc = LSM_DOM_DOCUMENT (lsm_mathml_document_new_from_itex(source.c_str(),source.length(),&err));
	
	if(!err) {
		view = lsm_dom_document_create_view (doc);
		lsm_dom_view_set_resolution (view, target_window->get_screen()->get_resolution());
		
		lsm_dom_view_get_size_pixels (view, (unsigned int*)&w, (unsigned int*)&h, NULL);
		SetSize(w,h);
		lsm_dom_view_render(view, image_ctx->cobj(), 0, 0);
		
		g_object_unref(view);
		g_object_unref(doc);
	} else {
		SetSize(128,16);
		image_ctx->set_source_rgb(1,0,0);
		image_ctx->set_font_size(10);
		image_ctx->move_to(0,14);
		image_ctx->show_text(err->message);
		image_ctx->fill(); //paint();
		g_error_free(err);
	}
}

CLatexWidget::~CLatexWidget()
{
}

void CLatexWidget::Redraw()
{
	
}

#endif