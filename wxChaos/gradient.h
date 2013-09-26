/** 
* @file gradient.h 
* @brief Defines a gradient.
*
* @author Evan Cordell
*
* @date 7/19/2012
*/

#pragma once
#ifndef WXGRADIENT_H
#define WXGRADIENT_H

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
	wxColor lerp(wxColor c1, wxColor c2, double value);
	std::vector<wxColor> m_stops;
	int m_min, m_max;

public:
	wxGradient();
	wxGradient(std::vector<wxColor> stops, int min, int max);
	bool Create(std::vector<wxColor> stops, int min, int max);
	virtual ~wxGradient();
	wxColour getColorAt(int value);
	void addColorStop(wxColor col);
	void insertColorStop(int index, wxColour col);
	void removeColorStop(int index);
	void editColorStop(int index, wxColour col);
	void setMin(int min);
	int getMin();
	void setMax(int max);
	int getMax();
	void setStops(std::vector<wxColor> stops);

	///@brief Creates gradient from wxString.
	///@param str An example of gradient wxString would be: rgb(255,255,255);rgb(34,92,12);rgb(0,0,0);.
	void fromString(wxString str);
	wxString toString();
	std::vector<wxColor> getStops();
};

#endif