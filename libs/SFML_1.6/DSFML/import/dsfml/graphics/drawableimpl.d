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

module dsfml.graphics.drawableimpl;

public import dsfml.system.common;
import dsfml.system.vector2;

import dsfml.graphics.idrawable;
import dsfml.graphics.color;
import dsfml.graphics.blendmode;
import dsfml.graphics.renderwindow;


package
{
    struct sfSprite{};
    struct sfShape{};
    struct sfString{};
}

/*
*   Package base class of all drawable.
*   Provide implementation of IDrawable and functions aliases. 
*/
package class Drawableimpl(T) : DSFMLObject, IDrawable
{
    void setX(float x)
    {
        sfDrawable_SetX(m_ptr, x);
    }

    void setY(float y)
    {
        sfDrawable_SetY(m_ptr, y);
    }

    void setPosition(float x, float y)
    {
        sfDrawable_SetPosition(m_ptr, x, y);
    }

    void setPosition(Vector2f vec)
    {
        sfDrawable_SetPosition(m_ptr, vec.x, vec.y);
    }

    void setScaleX(float scale)
    {
        if (scale > 0)
            sfDrawable_SetScaleX(m_ptr, scale);
    }
    
    void setScaleY(float scale)
    {
        if (scale > 0)
            sfDrawable_SetScaleY(m_ptr, scale);
    }

    void setScale(float scaleX, float scaleY)
    {
        if (scaleX > 0 && scaleY > 0)
            sfDrawable_SetScale(m_ptr, scaleX, scaleY);
    }

    void setScale(Vector2f scale)
    {
        if (scale.x > 0 && scale.y > 0)
            sfDrawable_SetScale(m_ptr, scale.x, scale.y);
    }

    void setCenter(float centerX, float centerY)
    {
        sfDrawable_SetCenter(m_ptr, centerX, centerY);
    }

    void setCenter(Vector2f center)
    {
        sfDrawable_SetCenter(m_ptr, center.x, center.y);
    }

    void setRotation(float angle)
    {
        sfDrawable_SetRotation(m_ptr, angle);
    }

    void setColor(Color c)
    {
        sfDrawable_SetColor(m_ptr, c);
    }
            
    void setBlendMode(BlendMode mode)
	{
		sfDrawable_SetBlendMode(m_ptr, mode);
	}

    Vector2f getPosition()
    {
        return Vector2f(sfDrawable_GetX(m_ptr), sfDrawable_GetY(m_ptr));
    }

    Vector2f getScale()
    {
        return Vector2f(sfDrawable_GetScaleX(m_ptr), sfDrawable_GetScaleY(m_ptr));
    }

    Vector2f getCenter()
    {
        return Vector2f(sfDrawable_GetCenterX(m_ptr),  sfDrawable_GetCenterY(m_ptr));
    }
    
    float getRotation()
    {
        return sfDrawable_GetRotation(m_ptr);
    }

    Color getColor()
    {
        return sfDrawable_GetColor(m_ptr);
    }

    BlendMode getBlendMode()
	{
		return cast(BlendMode)(sfDrawable_GetBlendMode(m_ptr));
	}

    void rotate(float angle)
    {
        sfDrawable_Rotate(m_ptr, angle);
    }

    void move(float offsetX, float offsetY)
    {
        sfDrawable_Move(m_ptr, offsetX, offsetY);
    }

    void move(Vector2f offset)
    {
        sfDrawable_Move(m_ptr, offset.x, offset.y);
    }

    void scale(float scaleX, float scaleY)
    {
        if (scaleX > 0 && scaleY > 0)
            sfDrawable_SetScale(m_ptr, scaleX, scaleY);
    }

    void scale(Vector2f scale)
    {
        if (scale.x > 0 && scale.y > 0)
            sfDrawable_SetScale(m_ptr, scale.x, scale.y);
    }
    
    Vector2f tranformToLocal(Vector2f point)
    {
        Vector2f ret;
        sfDrawable_TransformToLocal(m_ptr, point.x, point.y, &ret.x, &ret.y);
        return ret;
    }

    Vector2f tranformToGlobal(Vector2f point)
    {
        Vector2f ret;
        sfDrawable_TransformToLocal(m_ptr, point.x, point.y, &ret.x, &ret.y);
        return ret;
    }

    void render(RenderWindow window)
    {
        sfRenderWindow_DrawThis(window.getNativePointer, m_ptr);
    }

    override void dispose()
    {
        sfDrawable_Destroy(m_ptr);
    }

protected:
    this()
    {
        super(sfDrawable_Create());
    }

    this(void* ptr)
    {
        super(ptr);
    }
    
private:

    extern (C)
    {
        typedef void* function() pf_sfDrawable_Create;
    	typedef void function(void*) pf_sfDrawable_Destroy;
    	typedef void function(void*, float) pf_sfDrawable_SetX;
    	typedef void function(void*, float) pf_sfDrawable_SetY;
    	typedef void function(void*, float, float) pf_sfDrawable_SetPosition;
    	typedef void function(void*, float) pf_sfDrawable_SetScaleX;
    	typedef void function(void*, float) pf_sfDrawable_SetScaleY;
    	typedef void function(void*, float, float) pf_sfDrawable_SetScale;
    	typedef void function(void*, float) pf_sfDrawable_SetRotation;
    	typedef void function(void*, float, float) pf_sfDrawable_SetCenter;
    	typedef void function(void*, Color) pf_sfDrawable_SetColor;
    	typedef void function(void*, BlendMode) pf_sfDrawable_SetBlendMode;
    	typedef float function(void*) pf_sfDrawable_GetX;
    	typedef float function(void*) pf_sfDrawable_GetY;
    	typedef float function(void*) pf_sfDrawable_GetScaleX;
    	typedef float function(void*) pf_sfDrawable_GetScaleY;
    	typedef float function(void*) pf_sfDrawable_GetRotation;
    	typedef float function(void*) pf_sfDrawable_GetCenterX;
    	typedef float function(void*) pf_sfDrawable_GetCenterY;
    	typedef Color function(void*) pf_sfDrawable_GetColor;
    	typedef BlendMode function(void*) pf_sfDrawable_GetBlendMode;
    	typedef void function(void*, float, float) pf_sfDrawable_Move;
    	typedef void function(void*, float, float) pf_sfDrawable_Scale;
    	typedef void function(void*, float) pf_sfDrawable_Rotate;
    	typedef void function(void*, float, float, float*, float*) pf_sfDrawable_TransformToLocal;
    	typedef void function(void*, float, float, float*, float*) pf_sfDrawable_TransformToGlobal;
    	
        typedef void function(void*, void*) pf_sfRenderWindow_DrawThis;
        
    	static pf_sfDrawable_Create sfDrawable_Create;
    	static pf_sfDrawable_Destroy sfDrawable_Destroy;
    	static pf_sfDrawable_SetX sfDrawable_SetX;
    	static pf_sfDrawable_SetY sfDrawable_SetY;
    	static pf_sfDrawable_SetPosition sfDrawable_SetPosition;
    	static pf_sfDrawable_SetScaleX sfDrawable_SetScaleX;
    	static pf_sfDrawable_SetScaleY sfDrawable_SetScaleY;
    	static pf_sfDrawable_SetScale sfDrawable_SetScale;
    	static pf_sfDrawable_SetRotation sfDrawable_SetRotation;
    	static pf_sfDrawable_SetCenter sfDrawable_SetCenter;
    	static pf_sfDrawable_SetColor sfDrawable_SetColor;
    	static pf_sfDrawable_SetBlendMode sfDrawable_SetBlendMode;
    	static pf_sfDrawable_GetX sfDrawable_GetX;
    	static pf_sfDrawable_GetY sfDrawable_GetY;
    	static pf_sfDrawable_GetScaleX sfDrawable_GetScaleX;
    	static pf_sfDrawable_GetScaleY sfDrawable_GetScaleY;
    	static pf_sfDrawable_GetRotation sfDrawable_GetRotation;
    	static pf_sfDrawable_GetCenterX sfDrawable_GetCenterX;
    	static pf_sfDrawable_GetCenterY sfDrawable_GetCenterY;
    	static pf_sfDrawable_GetColor sfDrawable_GetColor;
    	static pf_sfDrawable_GetBlendMode sfDrawable_GetBlendMode;
    	static pf_sfDrawable_Move sfDrawable_Move;
    	static pf_sfDrawable_Scale sfDrawable_Scale;
    	static pf_sfDrawable_Rotate sfDrawable_Rotate;
    	static pf_sfDrawable_TransformToLocal sfDrawable_TransformToLocal;
    	static pf_sfDrawable_TransformToGlobal sfDrawable_TransformToGlobal;
    	
    	static pf_sfRenderWindow_DrawThis sfRenderWindow_DrawThis;
    }

    static this()
    {
        DllLoader dll = DllLoader.load("csfml-graphics");
        
        static if (is (T : sfSprite))
        {
            char[] symbol = "sfSprite";
        }
        else static if (is (T : sfString))
        {
            char[] symbol = "sfString";
        }
        else static if (is (T : sfShape))
        {
            char[] symbol = "sfShape";
        }
            
        sfDrawable_Create = cast(pf_sfDrawable_Create)dll.getSymbol(symbol ~ "_Create");
        sfDrawable_Destroy = cast(pf_sfDrawable_Destroy)dll.getSymbol(symbol ~ "_Destroy");
        sfDrawable_SetX = cast(pf_sfDrawable_SetX)dll.getSymbol(symbol ~ "_SetX");
        sfDrawable_SetY = cast(pf_sfDrawable_SetY)dll.getSymbol(symbol ~ "_SetY");
        sfDrawable_SetPosition = cast(pf_sfDrawable_SetPosition)dll.getSymbol(symbol ~ "_SetPosition");
        sfDrawable_SetScaleX = cast(pf_sfDrawable_SetScaleX)dll.getSymbol(symbol ~ "_SetScaleX");
        sfDrawable_SetScaleY = cast(pf_sfDrawable_SetScaleY)dll.getSymbol(symbol ~ "_SetScaleY");
        sfDrawable_SetScale = cast(pf_sfDrawable_SetScale)dll.getSymbol(symbol ~ "_SetScale");
        sfDrawable_SetRotation = cast(pf_sfDrawable_SetRotation)dll.getSymbol(symbol ~ "_SetRotation");
        sfDrawable_SetCenter = cast(pf_sfDrawable_SetCenter)dll.getSymbol(symbol ~ "_SetCenter");
        sfDrawable_SetColor = cast(pf_sfDrawable_SetColor)dll.getSymbol(symbol ~ "_SetColor");
        sfDrawable_SetBlendMode = cast(pf_sfDrawable_SetBlendMode)dll.getSymbol(symbol ~ "_SetBlendMode");
        sfDrawable_GetX = cast(pf_sfDrawable_GetX)dll.getSymbol(symbol ~ "_GetX");
        sfDrawable_GetY = cast(pf_sfDrawable_GetY)dll.getSymbol(symbol ~ "_GetY");
        sfDrawable_GetScaleX = cast(pf_sfDrawable_GetScaleX)dll.getSymbol(symbol ~ "_GetScaleX");
        sfDrawable_GetScaleY = cast(pf_sfDrawable_GetScaleY)dll.getSymbol(symbol ~ "_GetScaleX");
        sfDrawable_GetRotation = cast(pf_sfDrawable_GetRotation)dll.getSymbol(symbol ~ "_GetRotation");
        sfDrawable_GetCenterX = cast(pf_sfDrawable_GetCenterX)dll.getSymbol(symbol ~ "_GetCenterX");
        sfDrawable_GetCenterY = cast(pf_sfDrawable_GetCenterY)dll.getSymbol(symbol ~ "_GetCenterY");
        sfDrawable_GetColor = cast(pf_sfDrawable_GetColor)dll.getSymbol(symbol ~ "_GetColor");
        sfDrawable_GetBlendMode = cast(pf_sfDrawable_GetBlendMode)dll.getSymbol(symbol ~ "_GetBlendMode");
        sfDrawable_Move = cast(pf_sfDrawable_Move)dll.getSymbol(symbol ~ "_Move");
        sfDrawable_Scale = cast(pf_sfDrawable_Scale)dll.getSymbol(symbol ~ "_Scale");
        sfDrawable_Rotate = cast(pf_sfDrawable_Rotate)dll.getSymbol(symbol ~ "_Rotate");
        sfDrawable_TransformToLocal = cast(pf_sfDrawable_TransformToLocal)dll.getSymbol(symbol ~ "_TransformToLocal");
        sfDrawable_TransformToGlobal = cast(pf_sfDrawable_TransformToGlobal)dll.getSymbol(symbol ~ "_TransformToGlobal");
        
        sfRenderWindow_DrawThis = cast(pf_sfRenderWindow_DrawThis)dll.getSymbol("sfRenderWindow_Draw" ~ symbol[2..$]);
    }
}
