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

module dsfml.system.common;

public import dsfml.system.dllloader;

/**
*   Base class for all DSFML classes.
*/
class DSFMLObject
{
    this(void* ptr, bool preventDelete = false)
    {
        m_ptr = ptr;
    }
    
    ~this()
    {
        if (!m_preventDelete)
            dispose();
            
        m_ptr = m_ptr.init;
    }
    
    abstract void dispose();
    
    void* getNativePointer()
    {
        return m_ptr;
    }

    void setHandled(bool handled)
    {
        m_preventDelete = handled;
    }
    
    protected invariant()
    {
        assert(m_ptr !is null, "Problem occurs with a null pointer in " ~ this.toString);
    }

protected:
    void* m_ptr;
    
private:
    bool m_preventDelete;
}
