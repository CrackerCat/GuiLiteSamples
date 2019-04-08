#include "core_include/api.h"
#include "core_include/rect.h"
#include "core_include/cmd_target.h"
#include "core_include/wnd.h"
#include "core_include/surface.h"
#include "core_include/resource.h"
#include "core_include/bitmap.h"
#include "core_include/msg.h"
#include "core_include/display.h"
#include "core_include/theme.h"
#include "widgets_include/button.h"
#include "widgets_include/dialog.h"
#include "widgets_include/gesture.h"
#include <stdlib.h>

const int UI_WIDTH = 1280;
const int UI_HEIGHT = 720;

//////////////////////// define widgets & map message ////////////////////////
enum WND_ID
{
	ID_DESKTOP = 1,
	ID_START_BUTTON,
	ID_START_MENU
};

class c_desktop : public c_wnd
{
	virtual c_wnd* clone() { return new c_desktop(); }
	virtual void on_paint(void);
	void on_clicked(unsigned int ctrl_id);
	GL_DECLARE_MESSAGE_MAP()//delcare message
};

//map message
GL_BEGIN_MESSAGE_MAP(c_desktop)
ON_GL_BN_CLICKED(ID_START_BUTTON, c_desktop::on_clicked)
GL_END_MESSAGE_MAP()

void c_desktop::on_clicked(unsigned int ctrl_id)
{
	static bool is_open = false;
	(is_open) ? c_dialog::close_dialog(m_surface): c_dialog::open_dialog((c_dialog*)get_wnd_ptr(ID_START_MENU));
	is_open = !is_open;
}

void c_desktop::on_paint()
{
	c_rect rect;
	get_screen_rect(rect);
	c_bitmap::draw_bitmap(m_surface, m_z_order, c_theme::get_bmp(BITMAP_CUSTOM1), rect.m_left, rect.m_top);

	extern const BITMAP_INFO start_icon_bmp;
	extern const BITMAP_INFO start_icon_click_bmp;
	c_button* start_button = (c_button*)get_wnd_ptr(ID_START_BUTTON);
	start_button->set_bitmap(&start_icon_bmp);
	start_button->set_focus_bitmap(&start_icon_bmp);
	start_button->set_pushed_bitmap(&start_icon_click_bmp);
}

class c_start_menu : public c_dialog
{
	virtual c_wnd* clone() { return new c_start_menu(); }
	virtual void on_paint(void);
};

void c_start_menu::on_paint(void)
{
	c_rect rect;
	get_screen_rect(rect);
	c_bitmap::draw_bitmap(m_surface, m_z_order, c_theme::get_bmp(BITMAP_CUSTOM2), rect.m_left, rect.m_top);
}

//////////////////////// layout UI ////////////////////////
static c_desktop s_desktop;
static c_button s_start_button;
static c_start_menu s_start_menu;

static WND_TREE s_desktop_children[] =
{
	{(c_wnd*)&s_start_menu, ID_START_MENU, 0, 0, 100, 650, 580, NULL},
	{(c_wnd*)&s_start_button, ID_START_BUTTON, 0, 0, 682, 47, 38, NULL},
	{ NULL,0,0,0,0,0,0 }
};

//////////////////////// start UI ////////////////////////
extern const BITMAP_INFO desktop_bmp;
extern const BITMAP_INFO start_menu_bmp;
static c_fifo s_hid_fifo;
static c_display* s_display;
void load_resource()
{
	c_theme::add_bitmap(BITMAP_CUSTOM1, &desktop_bmp);
	c_theme::add_bitmap(BITMAP_CUSTOM2, &start_menu_bmp);
}

void create_ui(void* phy_fb, int screen_width, int screen_height, int color_bytes)
{
	load_resource();
	s_display = new c_display(phy_fb, screen_width, screen_height, UI_WIDTH, UI_HEIGHT, color_bytes, 1);
	c_surface* surface = s_display->alloc_surface(&s_desktop, Z_ORDER_LEVEL_1);
	surface->set_active(true);

	s_desktop.set_surface(surface);
	s_desktop.connect(NULL, ID_DESKTOP, 0, 0, 0, UI_WIDTH, UI_HEIGHT, s_desktop_children);
	s_desktop.show_window();

	new c_gesture(&s_desktop, NULL, &s_hid_fifo);
	while(1)
	{
		thread_sleep(1000000);
	}
}

//////////////////////// interface for all platform ////////////////////////
void start_helloGL(void* phy_fb, int width, int height, int color_bytes)
{
	create_ui(phy_fb, width, height, color_bytes);
}

int sendTouch2helloGL(void* buf, int len)
{
	ASSERT(len == sizeof(MSG_INFO));
	return s_hid_fifo.write(buf, len);
}

void* getUiOfhelloGL(int* width, int* height, bool force_update)
{
	return s_display->get_updated_fb(width, height, force_update);
}

int captureUiOfhelloGL()
{
	return s_display->snap_shot("snap_short.bmp");
}