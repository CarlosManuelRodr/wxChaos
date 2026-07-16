#include "DisplayUtilities.h"

#include <wx/display.h>
#include <wx/window.h>

namespace DisplayUtilities
{
    bool MoveToRightOf(const wxWindow& anchor, wxWindow& window)
    {
        const int displayIndex = wxDisplay::GetFromWindow(&anchor);
        const wxRect workArea = displayIndex == wxNOT_FOUND
                                    ? wxGetClientDisplayRect()
                                    : wxDisplay(static_cast<unsigned int>(displayIndex)).GetClientArea();
        const wxPoint anchorPosition = anchor.GetScreenPosition();
        const int targetX = anchorPosition.x + anchor.GetSize().GetWidth() + 5;

        if (targetX + window.GetSize().GetWidth() > workArea.GetRight() ||
            anchorPosition.y < workArea.GetTop() || anchorPosition.y > workArea.GetBottom())
            return false;

        window.Move(targetX, anchorPosition.y);
        return true;
    }
}
