#include <fstream>
#include "ConfigParser.h"
#include "StringFuncs.h"
using namespace std;

ConfigParser::ConfigParser(string _filename)
{
    filename = _filename;
    ifstream file;
    file.open(filename.c_str());

    // Analyzes the file.
    if(file.is_open())
    {
        const int BUFFER_SIZE = 200;
        char buffer[BUFFER_SIZE];
        string line;

        while(!file.eof())
        {
            file.getline(buffer, BUFFER_SIZE);
            line = buffer;
            int equalPos = -1;
            for(unsigned int i=0; i<line.length(); i++)
            {
                if(line[i] == '=')
                {
                    equalPos = i;
                    break;
                }
            }
            if(equalPos != -1)
            {
                labels.push_back(line.substr(0,equalPos));
            #ifdef _WIN32
                args.push_back(line.substr(equalPos+1, line.length()-equalPos-1));
            #elif __linux__
                // Search for last position.
                unsigned int lastPos = line.length();
                for(unsigned int i=equalPos+1; i<line.length(); i++)
                {
                    if(line[i] == '\r')
                    {
                        lastPos = i;
                        break;
                    }
                }
                args.push_back(line.substr(equalPos+1, lastPos-equalPos-1));
            #endif
            }
        }
        fileOpened = true;
    }
    else fileOpened = false;
    file.close();
}
bool ConfigParser::FileOpened()
{
    return fileOpened;
}
void ConfigParser::ReplaceArg(string label, string replaceArg)
{
    ifstream inFile;
    inFile.open(filename.c_str());

    // Analyze every label.
    const int BUFFER_SIZE = 200;
    char buffer[BUFFER_SIZE];
    string line, outText = "";
    while(!inFile.eof())
    {
        inFile.getline(buffer, BUFFER_SIZE);
        line = buffer;

        int equalPos = -1;
        for(unsigned int i=0; i<line.length(); i++)
        {
            if(line[i] == '=')
            {
                equalPos = i;
                break;
            }
        }
        if(equalPos != -1)
        {
            // If there is a label match replaces the argument.
            string lineLabel  = line.substr(0, equalPos);
            if(is_there_substr(lineLabel, label))
            {
                outText += line.substr(0, equalPos);
                outText += "=";
                outText += replaceArg;
            }
            else outText += line;
        }
        else outText += line;
        outText += "\n";
    }
    outText.pop_back();
    inFile.close();

    // Write output file.
    ofstream outFile;
    outFile.open(filename.c_str());
    for(unsigned int i=0; i<outText.length(); i++)
        outFile << outText[i];
    outFile.close();
}

bool ConfigParser::IntArgToVar(int& myVar, string expr, int defaultValue)
{
    bool found = false;
    for(unsigned int i=0; i<labels.size(); i++)
    {
        if(labels[i] == expr)
        {
            myVar = string_to_int(args[i]);
            found = true;
            break;
        }
    }
    if(!found) myVar = defaultValue;
    return found;
}
bool ConfigParser::UIntArgToVar(unsigned int& myVar, string expr, unsigned int defaultValue)
{
    bool found = false;
    for(unsigned int i=0; i<labels.size(); i++)
    {
        if(labels[i] == expr)
        {
            myVar = string_to_int(args[i]);
            found = true;
            break;
        }
    }
    if(!found) myVar = defaultValue;
    return found;
}
bool ConfigParser::DblArgToVar(double& myVar, string expr, double defaultValue)
{
    bool found = false;
    for(unsigned int i=0; i<labels.size(); i++)
    {
        if(labels[i] == expr)
        {
            myVar = string_to_double(args[i]);
            found = true;
            break;
        }
    }
    if(!found) myVar = defaultValue;
    return found;
}
bool ConfigParser::BoolArgToVar(bool& myVar, string expr, bool defaultValue)
{
    bool found = false;
    for(unsigned int i=0; i<labels.size(); i++)
    {
        if(labels[i] == expr)
        {
            if(args[i] == "TRUE" || args[i] == "True" || args[i] == "true") myVar = true;
            else myVar = false;
            found = true;
            break;
        }
    }
    if(!found) myVar = defaultValue;
    return found;
}
bool ConfigParser::StringArgToVar(string& myVar, string expr, string defaultValue)
{
    bool found = false;
    for(unsigned int i=0; i<labels.size(); i++)
    {
        if(labels[i] == expr)
        {
            myVar = args[i];
            found = true;
            break;
        }
    }
    if(!found) myVar = defaultValue;
    return found;
}