/** 
* @file wxGradient.h
* @brief Defines a gradient.
*
* @author Evan Cordell
*
* @date 7/19/2012
*/

#pragma once

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
	std::vector<wxColour> _stops;
	unsigned int _min, _max;

public:
	wxGradient();
	wxGradient(const std::vector<wxColour>& stops, unsigned int min, unsigned int max);
	virtual ~wxGradient();
	[[nodiscard]] wxColour GetColorAt(double value) const;
	void AddColorStop(const wxColour& col);
	void InsertColorStop(unsigned int index, const wxColour& col);
	void RemoveColorStop(unsigned int index);
	void EditColorStop(unsigned int index, const wxColour& col);
	void SetMin(unsigned int min);
	void SetMax(unsigned int max);
	[[nodiscard]] unsigned int GetMin() const;
	[[nodiscard]] unsigned int GetMax() const;
	void SetStops(const std::vector<wxColour>& stops);
	std::vector<wxColour> GetStops();

	///@brief Creates gradient from wxString.
	///@param str An example of gradient wxString would be: rgb(255,255,255);rgb(34,92,12);rgb(0,0,0);.
	void FromString(wxString str);
	wxString ToString();
};
