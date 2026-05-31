#include <sstream>
#include "StringFuncs.h"
using namespace std;

string str_num_to_string(const int d)
{
    ostringstream oss;
    oss << d;
    return oss.str();
}

string str_num_to_string(const double d)
{
    ostringstream oss;
    oss << d;
    return oss.str();
}

string str_bool_to_string(const bool d)
{
    return d ? "True" : "False";
}

wxString num_to_string(const int d)
{
    wxString num;
    num << d;
    return num;
}

wxString num_to_string(const double d)
{
    ostringstream oss;
    oss << d;
    wxString num(oss.str().c_str(), wxConvUTF8);
    return num;
}

wxString bool_to_string(const bool d)
{
    return d ? wxString(wxT("True")) : wxString(wxT("False"));
}

double string_to_double(const wxString& s)
{
    // s.ToDouble(&num) fails on linux.
    string temp(s.mb_str());
    istringstream i(temp);
    double x;
    return !(i >> x) ? 0 : x;
}

int string_to_int(const wxString& s)
{
    return wxAtoi(s);
}

double string_to_double(const string& s)
{
    istringstream i(s);
    double x;
    return !(i >> x) ? 0 : x;
}

int string_to_int(const string& s)
{
    istringstream i(s);
    double x;
    return !(i >> x) ? 0 : static_cast<int>(x);
}

bool is_there_substr(const wxString &cad, const wxString &cadBus)
{
    const size_t nCad = cad.length();
    const size_t nCadBus = cadBus.length();

    for (size_t i=0; i<=nCad-nCadBus; i++)
    {
        wxString subCad = cad.substr(i, nCadBus);
        if (subCad == cadBus)
            return true;
    }
    return false;
}

bool is_there_substr(const string &cad, const string &cadBus)
{
    const size_t nCad = cad.length();
    const size_t nCadBus = cadBus.length();

    for (size_t i=0; i<=nCad-nCadBus; i++)
    {
        string subCad = cad.substr(i, nCadBus);
        if (subCad == cadBus)
            return true;
    }
    return false;
}

string replace_ext(const string &filepath, const string &newExt)
{
    // Changes file extension.
    unsigned int extDot = 0;
    string out = filepath;
    for (unsigned int i=0; i<filepath.size(); i++)
    {
        if (filepath[i] == '.') extDot = i;
    }
    out.erase(extDot, filepath.size()-extDot);
    out += newExt;
    return out;
}
bool check_ext(const string &filename, const string &ext)
{
    for (unsigned int i=0; i<filename.length(); i++)
    {
        if (filename[i] == '.' && i+1 != filename.length())
        {
            if (filename.substr(i+1, filename.size()-1-i) == ext)
                return true;
        }
    }
    return false;
}

vector<int> get_int_list(const wxString& in)
{
    vector<int> out;
    unsigned int firstPos = 0;

    for (unsigned int i=0; i<in.length(); i++)
    {
        if (in[i] == ',')
        {
            out.push_back(string_to_int(in.substr(firstPos, i-firstPos)));
            firstPos = i + 1;
        }
    }

    // Get last number.
    out.push_back(string_to_int(in.substr(firstPos, in.size()-firstPos)));
    return out;
}