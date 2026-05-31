/** 
* @file wxGradient.h
* @brief Defines a gradient.
*
* @author Evan Cordell
*
* @date 7/19/2012
*/

#pragma once
#ifndef WX_GRADIENT_H
#define WX_GRADIENT_H

#include <wx/colour.h>
#include <wx/wx.h>
#include <vector>

/**
* @class wxGradient
* @brief Gradient to create color palette.
*/
class wxGradient
{
protected:
	static wxColour Lerp(const wxColour& c1, const wxColour& c2, double value);
	std::vector<wxColour> m_stops;
	int m_min, m_max;

public:
	wxGradient();
	wxGradient(const std::vector<wxColour>& stops, int min, int max);
	virtual ~wxGradient();
	wxColour GetColorAt(int value) const;
	void AddColorStop(const wxColour& col);
	void InsertColorStop(int index, const wxColour& col);
	void RemoveColorStop(int index);
	void EditColorStop(int index, const wxColour& col);
	void SetMin(int min);
	int GetMin() const;
	void SetMax(int max);
	int GetMax() const;
	void SetStops(const std::vector<wxColour>& stops);
	std::vector<wxColour> GetStops();

	///@brief Creates gradient from wxString.
	///@param str An example of gradient wxString would be: rgb(255,255,255);rgb(34,92,12);rgb(0,0,0);.
	void FromString(wxString str);
	wxString ToString();
};

#endif