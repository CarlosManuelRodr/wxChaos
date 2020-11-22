////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2009 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#define _WIN32_WINDOWS 0x0501
#define _WIN32_WINNT   0x0501
#include <SFML/Window/Joystick.hpp>
#include <windows.h>
#include <mmsystem.h>


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
/// Initialize the instance and bind it to a physical joystick
////////////////////////////////////////////////////////////
void Joystick::Initialize(unsigned int Index)
{
    // Reset state
    myIndex            = JOYSTICKID1;
    myNbButtons        = 0;
    myIsConnected      = false;
    myHasContinuousPOV = false;
    for (int i = 0; i < Joy::AxisCount; ++i)
        myAxes[i] = false;

    // Get the Index-th connected joystick
    MMRESULT Error;
    JOYINFOEX JoyInfo;
    JoyInfo.dwSize = sizeof(JoyInfo);
    JoyInfo.dwFlags = JOY_RETURNALL;
    for (unsigned int NbFound = 0; (Error = joyGetPosEx(myIndex, &JoyInfo)) != JOYERR_PARMS; myIndex++)
    {
        // Check if the current joystick is connected
        if (Error == JOYERR_NOERROR)
        {
            // Check if it's the required index
            if (NbFound == Index)
            {
                // Ok : store its parameters and return
                myIsConnected = true;
                JOYCAPS Caps;
                joyGetDevCaps(myIndex, &Caps, sizeof(Caps));
                myNbButtons = Caps.wNumButtons;
                if (myNbButtons > Joy::ButtonCount)
                    myNbButtons = Joy::ButtonCount;

                myAxes[Joy::AxisX]   = true;
                myAxes[Joy::AxisY]   = true;
                myAxes[Joy::AxisZ]   = (Caps.wCaps & JOYCAPS_HASZ) != 0;
                myAxes[Joy::AxisR]   = (Caps.wCaps & JOYCAPS_HASR) != 0;
                myAxes[Joy::AxisU]   = (Caps.wCaps & JOYCAPS_HASU) != 0;
                myAxes[Joy::AxisV]   = (Caps.wCaps & JOYCAPS_HASV) != 0;
                myAxes[Joy::AxisPOV] = (Caps.wCaps & JOYCAPS_HASPOV) != 0;
                myHasContinuousPOV   = (Caps.wCaps & JOYCAPS_POVCTS) != 0;

                return;
            }

            // Go to the next valid joystick
            ++NbFound;
        }
    }
}


////////////////////////////////////////////////////////////
/// Update the current joystick and return its new state
////////////////////////////////////////////////////////////
JoystickState Joystick::UpdateState()
{
    JoystickState State;

    if (myIsConnected)
    {
        // Get the joystick caps (for range conversions)
        JOYCAPS Caps;
        if (joyGetDevCaps(myIndex, &Caps, sizeof(Caps)) == JOYERR_NOERROR)
        {
            // Get the current joystick state
            JOYINFOEX Pos;
            Pos.dwFlags  = JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR | JOY_RETURNU | JOY_RETURNV | JOY_RETURNBUTTONS;
            Pos.dwFlags |= myHasContinuousPOV ? JOY_RETURNPOVCTS : JOY_RETURNPOV;
            Pos.dwSize   = sizeof(JOYINFOEX);
            if (joyGetPosEx(myIndex, &Pos) == JOYERR_NOERROR)
            {
                // Axes
                State.Axis[Joy::AxisX] = (Pos.dwXpos - (Caps.wXmax + Caps.wXmin) / 2.f) * 200.f / (Caps.wXmax - Caps.wXmin);
                State.Axis[Joy::AxisY] = (Pos.dwYpos - (Caps.wYmax + Caps.wYmin) / 2.f) * 200.f / (Caps.wYmax - Caps.wYmin);
                State.Axis[Joy::AxisZ] = (Pos.dwZpos - (Caps.wZmax + Caps.wZmin) / 2.f) * 200.f / (Caps.wZmax - Caps.wZmin);
                State.Axis[Joy::AxisR] = (Pos.dwRpos - (Caps.wRmax + Caps.wRmin) / 2.f) * 200.f / (Caps.wRmax - Caps.wRmin);
                State.Axis[Joy::AxisU] = (Pos.dwUpos - (Caps.wUmax + Caps.wUmin) / 2.f) * 200.f / (Caps.wUmax - Caps.wUmin);
                State.Axis[Joy::AxisV] = (Pos.dwVpos - (Caps.wVmax + Caps.wVmin) / 2.f) * 200.f / (Caps.wVmax - Caps.wVmin);

                // POV
                if (Pos.dwPOV != 0xFFFF)
                    State.Axis[Joy::AxisPOV] = Pos.dwPOV / 100.f;
                else
                    State.Axis[Joy::AxisPOV] = -1.f;

                // Buttons
                for (unsigned int i = 0; i < GetButtonsCount(); ++i)
                    State.Buttons[i] = (Pos.dwButtons & (1 << i)) != 0;
            }
        }
    }

    return State;
}


////////////////////////////////////////////////////////////
/// Check if the joystick supports the given axis
////////////////////////////////////////////////////////////
bool Joystick::HasAxis(Joy::Axis Axis) const
{
    return myAxes[Axis];
}


////////////////////////////////////////////////////////////
/// Get the number of buttons supported by the joystick
////////////////////////////////////////////////////////////
unsigned int Joystick::GetButtonsCount() const
{
    return myNbButtons;
}


} // namespace priv

} // namespace sf
