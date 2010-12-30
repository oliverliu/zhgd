// Read an INI file into easy-to-access name/value pairs.

#include <cctype>
#include <cstdlib>
#include "ini.h"
#include "INIReader.h"

using std::string;

INIReader::INIReader(string filename)
{
    _error = ini_parse(filename.c_str(), ValueHandler, this);
}

INIReader::~INIReader()
{
    ;
}

int INIReader::ParseError()
{
    return _error;
}

string INIReader::Get(string section, string name, string default_value)
{
    string key = MakeKey(section, name);
    return _values.count(key) ? _values[key] : default_value;
}

unsigned long int INIReader::GetInteger(string section, string name, long default_value)
{
    string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    unsigned long int  n = strtoul(value, &end, 0);
    return end > value ? n : default_value;
}

string INIReader::MakeKey(string section, string name)
{
    string key = section + "." + name;
    // Convert to lower case to make lookups case-insensitive
    for (int i = 0; i < key.length(); i++)
        key[i] = tolower(key[i]);
    return key;
}

int INIReader::ValueHandler(void* user, const char* section, const char* name,
                            const char* value)
{
    INIReader* reader = (INIReader*)user;
    reader->_values[MakeKey(section, name)] = value;
    return 1;
}


//ZINIReader
 ZINIReader::ZINIReader(std::string filename) : INIReader(filename)
 {
 }

long ZINIReader::GetIDFromIp(std::string section, std::string value, long default_key)
{
    long ret = default_key;
     std::map<std::string, std::string>::iterator it ;
     for(it = _values.begin(); it != _values.end(); it++)
     {
        if (value == it->second )
        {
            std::string key = it->first;
            if (key.substr(0, section.length()) == section )
            {
                int pos = key.find("0x");    // position of "0x" in str
                const char* value =  key.substr (pos).c_str();
                char* end;
                // This parses "1234" (decimal) and also "0x4D2" (hex)
                unsigned long int  n = strtoul(value, &end, 0);
                ret =  end > value ? n : default_key;
                //break; //get first match
                //now get last match
            }
        }
    }
    return ret;   
}

