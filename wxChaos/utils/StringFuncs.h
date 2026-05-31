/**
* @file StringFuncs.h
* @brief Some useful wxString-related functions.
*
* @author Carlos Manuel Rodriguez y Martinez
* @copyright GNU Public License.
* @date 7/19/2012
*/

#ifndef __stringFuncs
#define __stringFuncs

#include <wx/wx.h>
#include <vector>

std::string str_num_to_string(int d);
std::string str_num_to_string(double d);
std::string str_bool_to_string(bool d);
wxString num_to_string(int d);
wxString num_to_string(double d);
wxString bool_to_string(bool d);
double string_to_double(const wxString& s);
double string_to_double(const std::string& s);
int string_to_int(const wxString& s);
int string_to_int(const std::string& s);
bool is_there_substr(const wxString &cad, const wxString &cadBus);
bool is_there_substr(const std::string &cad, const std::string &cadBus);
std::string replace_ext(const std::string &filepath, const std::string &newExt);
bool check_ext(const std::string &filename, const std::string &ext);
std::vector<int> get_int_list(const wxString& in);

#endif