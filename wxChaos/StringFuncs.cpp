#include "StringFuncs.h"

std::string str_num_to_string(const int d)
{
	std::ostringstream oss;
	oss << d;
	return oss.str();
}
std::string str_num_to_string(const double d)
{
	std::ostringstream oss;
	oss << d;
	return oss.str();
}
std::string str_bool_to_string(const bool d)
{
	if(d) return std::string("True");
	else return std::string("False");
}
wxString num_to_string(const int d)
{
	wxString num;
	num << d;
	return num;
}
wxString num_to_string(const double d)
{
	std::ostringstream oss;
	oss << d;
	wxString num(oss.str().c_str(), wxConvUTF8);
	return num;
}
wxString bool_to_string(const bool d)
{
	if(d) return wxString(wxT("True"));
	else return wxString(wxT("False"));
}
double string_to_double(const wxString& s)
{
	// s.ToDouble(&num) fails on linux.
	std::string temp(s.mb_str());
	std::istringstream i(temp);
	double x;
	if (!(i >> x))
		return 0;
	return x;
}
int string_to_int(const wxString& s)
{
	return wxAtoi(s);
}
double string_to_double(const std::string& s)
{
	std::istringstream i(s);
	double x;
	if (!(i >> x))
		return 0;
	return x;
}
int string_to_int(const std::string& s)
{
	std::istringstream i(s);
	double x;
	if (!(i >> x))
		return 0;
	return static_cast<int>(x);
}

bool is_there_substr(const wxString cad, const wxString cadBus)
{
	int nCad, nCadBus;
	nCad=cad.length();
	nCadBus=cadBus.length();
	wxString subCad;

	for(int i=0; i<=nCad-nCadBus; i++)
	{
		subCad = cad.substr(i, nCadBus);
		if(subCad == cadBus) return true;
	}
	return false;
}
bool is_there_substr(const string cad, const string cadBus)
{
	int nCad, nCadBus;
	nCad=cad.length();
	nCadBus=cadBus.length();
	string subCad;

	for(int i=0; i<=nCad-nCadBus; i++)
	{
		subCad = cad.substr(i, nCadBus);
		if(subCad == cadBus) return true;
	}
	return false;
}

bool is_there_function(const wxString input)
{
	// Look for a sign outside the parenthesis.
	int signLevel = 0;
	for(unsigned int i=0; i<input.length(); i++)
	{
		if(input[i] == '(') signLevel--;
		if(input[i] == ')') signLevel++;
		if(signLevel == 0)
		{
			switch((char)input[i])
			{
			case '+':
			case '-':
			case '*':
			case '^':
			case'/': return false;
				break;
			}
		}
	}
	// Search function.
	bool pLeft = false, pRight = false;
	for(unsigned int i=0; i<input.length(); i++)
	{
		if(input[i] == '(') pLeft = true;
		if(input[i] == ')') pRight = true;
	}
	if(pLeft && pRight) return true;
	else return false;
}
string replace_ext(const string filepath, const string newExt)
{
	// Changes file extension.
	unsigned int extDot;
	string out = filepath;
	for(unsigned int i=0; i<filepath.size(); i++)
	{
		if(filepath[i] == '.') extDot = i;
	}
	out.erase(extDot, filepath.size()-extDot);
	out += newExt;
	return out;
}
bool check_ext(const string filename, const string ext)
{
	for(unsigned int i=0; i<filename.length(); i++)
	{
		if(filename[i] == '.' && i+1 != filename.length())
		{
			if(filename.substr(i+1, filename.size()-1-i) == ext)
				return true;
		}
	}
	return false;
}
vector<int> get_int_list(const wxString in)
{
	vector<int> out;
	int firstPos = 0;

	for(unsigned int i=0; i<in.length(); i++)
	{
		if(in[i] == ',')
		{
			out.push_back(string_to_int(in.substr(firstPos, i-firstPos)));
			firstPos = i+1;
		}
	}

	// Get last number.
	out.push_back(string_to_int(in.substr(firstPos, in.size()-firstPos)));

	return out;
}
