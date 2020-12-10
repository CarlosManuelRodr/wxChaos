/** 
* @file ConfigParser.h 
* @brief This header file contains the ConfigParser class.
*
* The ConfigParser class is used to retrieve values from a configuration file.
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
*
* @date 7/18/2012
*/

#pragma once
#ifndef _configParser
#define _configParser
#include "Styles.h"
#include "FractalClasses.h"
#include <vector>
using namespace std;

/**
* @class ConfigParser
* @brief Analyzes a configuration file.
*
* It's used when the program starts to load parameters from the "config.ini" file. It's also used to read
* parameters from the renderJob files.
*/

class ConfigParser
{
private:
    vector<string> labels;   ///< Vector that contains all the labels found.
    vector<string> args;     ///< Vector that contains all the arguments found.
    bool fileOpened;         ///< Status of the opened file.
    string filename;
public:
    ///@brief Constructor
    ///@param filename File to analyze.
    ConfigParser(string _filename);

    ///@brief Ask for the state of the opened file.
    ///@return true if file was opened. false if not.
    bool FileOpened();

    ///@brief Replaces the argument of the label.
    ///@param label Label to replace argument.
    ///@param replaceArg New argument.
    void ReplaceArg(string label, string replaceArg);

    ///@brief Gets int value from expresion.
    ///@param myVar int var to store value.
    ///@param expr Label to find.
    ///@param defaultValue If expr isn't found stores this default value.
    ///@return true if expr was found. False if not.
    bool IntArgToVar(int& myVar, string expr, int defaultValue);

    ///@brief Gets uInt value from expresion.
    ///@param myVar uInt var to store value.
    ///@param expr Label to find.
    ///@param defaultValue If expr isn't found stores this default value.
    ///@return true if expr was found. False if not.
    bool UIntArgToVar(unsigned int& myVar, string expr, unsigned int defaultValue);

    ///@brief Gets double value from expresion.
    ///@param myVar double var to store value.
    ///@param expr Label to find.
    ///@param defaultValue If expr isn't found stores this default value.
    ///@return true if expr was found. False if not.
    bool DblArgToVar(double& myVar, string expr, double defaultValue);

    ///@brief Gets bool value from expresion.
    ///@param myVar bool var to store value.
    ///@param expr Label to find.
    ///@param defaultValue If expr isn't found stores this default value.
    ///@return true if expr was found. False if not.
    bool BoolArgToVar(bool& myVar, string expr, bool defaultValue);

    ///@brief Gets string value from expresion.
    ///@param myVar string var to store value.
    ///@param expr Label to find.
    ///@param defaultValue If expr isn't found stores this default value.
    ///@return true if expr was found. False if not.
    bool StringArgToVar(string& myVar, string expr, string defaultValue);

    /**
    *@brief Stores M element in myVar.
    *
    * A vector of labels to search is given in the options parameter, also a vector of values.
    * In order to work properly they must have the same order.
    * @param myVar M var to store value.
    * @param expr Label to find.
    * @param options Vector which contains all the labels of the options to find.
    * @param values Vector of values to store in myVar.
    * @param defaultValue If expr isn't found stores this default value.
    */
    template<class M> void OptionToVar(M& myVar, string expr, vector<string> options, vector<M> values, M defaultValue);
};

template<class M> void ConfigParser::OptionToVar(M& myVar, string expr, vector<string> options, vector<M> values, M defaultValue)
{
    bool found = false;
    if(options.size() == values.size())
    {
        for(unsigned int i=0; i<labels.size() && !found; i++)
        {
            if(labels[i] == expr)
            {
                for(unsigned int j=0; j<options.size(); j++)
                {
                    if(args[i] == options[j])
                    {
                        myVar = values[j];
                        found = true;
                        break;
                    }
                }
            }
        }
        if(!found) myVar = defaultValue;
    }
    else myVar = defaultValue;
}

#endif