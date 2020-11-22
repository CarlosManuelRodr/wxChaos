/*
*   DSFML - SFML Library binding in D language.
*   Copyright (C) 2008 Julien Dagorn (sirjulio13@gmail.com)
*
*   This software is provided 'as-is', without any express or
*   implied warranty. In no event will the authors be held
*   liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose,
*   including commercial applications, and to alter it and redistribute
*   it freely, subject to the following restrictions:
*
*   1.  The origin of this software must not be misrepresented;
*       you must not claim that you wrote the original software.
*       If you use this software in a product, an acknowledgment
*       in the product documentation would be appreciated but
*       is not required.
*
*   2.  Altered source versions must be plainly marked as such,
*       and must not be misrepresented as being the original software.
*
*   3.  This notice may not be removed or altered from any
*       source distribution.
*/

module dsfml.window.input;

import dsfml.system.common;

import dsfml.window.event;

/**
*   Input handles real-time input from keyboard and mouse.
*   Use it instead of events to handle continuous moves and more
*   game-friendly inputs
*/
class Input : DSFMLObject
{
    /**
    *   Get the state of a key
    *
    *   Params:
    *       key = Key to check
    *
    *   Returns:
    *       True if key is down, false if key is up
    */
    bool isKeyDown(KeyCode key)
	{
		return cast(bool)sfInput_IsKeyDown(m_ptr, key);
	}

    /**
    *   Get the state of a mouse button
    *
    *   Params: 
    *       button = Button to check
    *
    *   Returns: 
    *       True if button is down, false if button is up
    */
    bool isMouseButtonDown(MouseButtons button)
	{
		return cast(bool)sfInput_IsMouseButtonDown(m_ptr, button);
	}

    /**
    *   Get the state of a joystick button
    *
    *   Params: 
    *       joyId = Identifier of the joystick to check (0 or 1)
    *       button = Button to check
    *
    *   Returns: 
    *       True if button is down, false if button is up
    */
    bool isJoystickButtonDown(uint joyId, uint button)
	{
		return cast(bool)sfInput_IsJoystickButtonDown(m_ptr, joyId, button);
	}

    /*
    *   Get the mouse X position
    *
    *   Returns:
    *       Current mouse left position, relative to owner window
    */
    int getMouseX()
	{
		return sfInput_GetMouseX(m_ptr);
	}

    /**
    *   Get the mouse Y position
    *
    *   Returns:
    *       Current mouse top position, relative to owner window
    *
    */
    int getMouseY()
	{
		return sfInput_GetMouseY(m_ptr);
	}

    /**
    * Get a joystick axis position
    *
    *   Params:    
    *       joyId = Identifier of the joystick to check (0 or 1)
    *       axis =  Axis to get
    *
    *   Returns:
    *       Current axis position, in the range [-100, 100] (except for POV, which is [0, 360])
    */
    float getJoystickAxis(uint joyId, JoyAxis axis)
	{
		return sfInput_GetJoystickAxis(m_ptr, joyId, axis);
	}

    this(void* input)
	{
		super(input, true);
	}

    override void dispose()
    {
        // nothing to do
    }

private:

// External ====================================================================

    extern (C)
    {
        typedef int function(void*, KeyCode) pf_sfInput_IsKeyDown;
    	typedef int function(void*, MouseButtons) pf_sfInput_IsMouseButtonDown;
    	typedef int function(void*, uint, uint) pf_sfInput_IsJoystickButtonDown;
    	typedef uint function (void*) pf_sfInput_GetMouseX;
    	typedef uint function(void*) pf_sfInput_GetMouseY;
    	typedef float function(void*, uint, JoyAxis) pf_sfInput_GetJoystickAxis;
    
    	static pf_sfInput_IsKeyDown sfInput_IsKeyDown;
    	static pf_sfInput_IsMouseButtonDown sfInput_IsMouseButtonDown;
    	static pf_sfInput_IsJoystickButtonDown sfInput_IsJoystickButtonDown;
    	static pf_sfInput_GetMouseX sfInput_GetMouseX;
    	static pf_sfInput_GetMouseY sfInput_GetMouseY;
    	static pf_sfInput_GetJoystickAxis sfInput_GetJoystickAxis;
    }

    static this()
    {
        DllLoader dll = DllLoader.load("csfml-window");
        
        sfInput_IsKeyDown = cast(pf_sfInput_IsKeyDown)dll.getSymbol("sfInput_IsKeyDown");
        sfInput_IsMouseButtonDown = cast(pf_sfInput_IsMouseButtonDown)dll.getSymbol("sfInput_IsMouseButtonDown");
        sfInput_IsJoystickButtonDown = cast(pf_sfInput_IsJoystickButtonDown)dll.getSymbol("sfInput_IsJoystickButtonDown");
        sfInput_GetMouseX = cast(pf_sfInput_GetMouseX)dll.getSymbol("sfInput_GetMouseX");
        sfInput_GetMouseY = cast(pf_sfInput_GetMouseY)dll.getSymbol("sfInput_GetMouseY");
        sfInput_GetJoystickAxis = cast(pf_sfInput_GetJoystickAxis)dll.getSymbol("sfInput_GetJoystickAxis");
    }
}
