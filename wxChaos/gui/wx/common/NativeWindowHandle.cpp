#include "NativeWindowHandle.h"

#include <wx/window.h>

#ifdef __WXGTK__
struct _GdkWindow;
using GdkWindow = _GdkWindow;

extern "C" GdkWindow* gtk_widget_get_window(GtkWidget* widget);
extern "C" unsigned long gdk_x11_window_get_xid(GdkWindow* window);
#endif

namespace Platform
{
    sf::WindowHandle GetNativeWindowHandle(const wxWindow& window)
    {
#ifdef __WXGTK__
        GdkWindow* gdkWindow = gtk_widget_get_window(window.GetHandle());
        return gdkWindow == nullptr ? 0 : static_cast<sf::WindowHandle>(gdk_x11_window_get_xid(gdkWindow));
#else
        return reinterpret_cast<sf::WindowHandle>(window.GetHandle());
#endif
    }
}
