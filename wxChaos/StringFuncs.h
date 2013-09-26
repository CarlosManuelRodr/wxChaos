/**
* @file StringFuncs.h
* @brief Some useful wxString related functions.
*
* @author Carlos Manuel Rodriguez y Martinez
* @copyright GNU Public License.
* @date 7/19/2012
*/

#ifndef __stringFuncs
#define __stringFuncs

#include <sstream>
#include <wx/wx.h>
#include <vector>
using namespace std;

std::string str_num_to_string(const int d);
std::string str_num_to_string(const double d);
std::string str_bool_to_string(const bool d);
wxString num_to_string(const int d);
wxString num_to_string(const double d);
wxString bool_to_string(const bool d);
double string_to_double(const wxString& s);
double string_to_double(const std::string& s);
int string_to_int(const wxString& s );
int string_to_int(const std::string& s);
bool is_there_substr(const wxString cad, const wxString cadBus);
bool is_there_substr(const string cad, const string cadBus);
bool is_there_function(const wxString input);
string replace_ext(const string filepath, const string newExt);
bool check_ext(const string filename, const string ext);
vector<int> get_int_list(const wxString in);

#endif
