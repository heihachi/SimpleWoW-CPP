#ifndef G__CONFIG_H
#define G__CONFIG_H

/*
 * Config.h
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Copyright 2003  The libConfig++ library project
 */

#include <cstdlib>
#include <cstdio>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <iostream>

class errGetVal {
   public:
      std::string key;
      errGetVal(const std::string err_key) {
         key = err_key;
      };
      virtual ~errGetVal() {};
};

class errSetVal : public errGetVal {
    public:
	std::string val;
	std::string token;
	errSetVal ( const std::string err_key, 
		    const std::string err_val, 
		    const std::string err_token="")
		: errGetVal(err_key) {
		val	= err_val;
		token	= err_token;
	};
	
};

extern FILE *conf_in;

enum conf_operation {
    CONF_OP_IS_SET,
    CONF_OP_EQ,
    CONF_OP_NE,
    CONF_OP_RLIKE,
    CONF_OP_NOT_RLIKE
};

class ConfVP {
    public:
	std::string key;
	std::string value;
	conf_operation op;
	std::string print() const;
};

class Section;
typedef std::map<std::string, Section *> SectionList;
typedef std::list<ConfVP> Args;
typedef Args::iterator Args_I;
typedef Args::const_iterator Args_CI;

class Section {
    private:
	std::string section_type;
	Args args;
    public:
	Section() { section_type = ""; };
	Section(const std::string t_str) { 
	    section_type = t_str; 
	}
	Section(const std::string t_str, const Args &t_args) { 
	    section_type = t_str; 
	    args = t_args;
	};
	~Section();
	std::string getType() const { return section_type; }
	Args getArgs() const { return args; }
	std::string getArgString() const;
	
	std::string getVal(const std::string key) const;
	SectionList sectionList;
	std::map<std::string, std::string>	item;
};

typedef SectionList::const_iterator	CSI;
typedef SectionList::iterator		SI;
typedef std::map<std::string, std::string>::const_iterator	CII;
typedef std::map<std::string, std::string>::iterator		II;

class ConfEntry {
    public:
	FILE *f;
	Section *sect;
	std::string f_name;
};


struct ParserParam {
    std::stack<Section *> sect_stack;
    std::stack<FILE *> in_stack;
    std::queue<ConfEntry> confEntryQueue; //this is for parcing multiple files.
    Section *root_p;
    const std::string *value_ptr;
    Section *sect_ptr;
    //std::list<std::string> value_list;
    ConfVP vp;
    std::list<ConfVP> value_pair_list;
    ParserParam() { 
	value_ptr = NULL; sect_ptr = NULL; 
	root_p = NULL; 
    }
};

Section * parseConfig(const char *confFileName);
void printSection(Section *sect, int level=0);
void printSection(std::ostream &Out, Section *sect, int level=0);

int conf_parse(void * param_p);

class SetupBaseType {
    public:
	virtual ~SetupBaseType() {}
	virtual void set(const std::string &c_val) = 0;
};


class SetupBaseType_set : public SetupBaseType {
    typedef std::map<std::string, unsigned long> TrSet;
    private:
	unsigned long &val;
	TrSet tr_set;
	std::string delimiter;
	std::string::size_type pos,start_pos;
	std::string::size_type shift(const std::string::size_type npos) {
	    if (pos == npos) 	{ return pos; }
	    else 		{ return (pos+delimiter.size()); }
	}
    public:
	SetupBaseType_set(unsigned long &c_val, const TrSet &c_tr_set, const std::string c_delimiter="|") : 
	    val(c_val),
	    tr_set(c_tr_set),
	    delimiter(c_delimiter) { val = 0; }
	void set(const std::string &str_val) {
	    std::string token;
	    for (pos =0, start_pos = 0; 
		start_pos != str_val.npos; 
		start_pos=shift(str_val.npos)) {

		pos = str_val.find(delimiter, start_pos);
		if (pos == str_val.npos) {
		    token = str_val.substr(start_pos, str_val.size()-start_pos);
		}
		else {
		    token = str_val.substr(start_pos, pos-start_pos);
		}
		TrSet::const_iterator ts_i = tr_set.find(token);
		if (ts_i == tr_set.end()) {
		    throw errGetVal(token);
		    //there isn't key, it's impossible to translate into set.
		}
		val |= ts_i->second;		
	    } 
	}
};

class SetupBaseType_int : public SetupBaseType {
    private:
	int &val;
    public:
	SetupBaseType_int(int &c_val) : val(c_val) {}
	void set(const std::string &str_val) {
	    val = (int)strtoul(str_val.c_str(), NULL, 0);
	}
};

class SetupBaseType_uint : public SetupBaseType {
    private:
	unsigned int &val;
    public:
	SetupBaseType_uint(unsigned int &c_val) : val(c_val) {}
	void set(const std::string &str_val) {
	    val = (unsigned int) strtoul(str_val.c_str(), NULL, 0);
	}
};

class SetupBaseType_short : public SetupBaseType {
    private:
	short &val;
    public:
	SetupBaseType_short(short &c_val) : val(c_val) {}
	void set(const std::string &str_val) {
	    val = (short) strtol(str_val.c_str(), NULL, 0);
	}
};

class SetupBaseType_ushort : public SetupBaseType {
    private:
	unsigned short &val;
    public:
	SetupBaseType_ushort(unsigned short &c_val) : val(c_val) {}
	void set(const std::string &str_val) {
	    val = (unsigned short) strtoul(str_val.c_str(), NULL, 0);
	}
};

class SetupBaseType_long : public SetupBaseType {
    private:
	long &val;
    public:
	SetupBaseType_long(long &c_val) : val(c_val) {}
	void set(const std::string &str_val) {
	    val = strtol(str_val.c_str(), NULL, 0);
	}
};

class SetupBaseType_ulong : public SetupBaseType {
    private:
	unsigned long &val;
    public:
	SetupBaseType_ulong(unsigned long &c_val) : val(c_val) {}
	void set(const std::string &str_val) {
	    val = strtoul(str_val.c_str(), NULL, 0);
	}
};

class SetupBaseType_double : public SetupBaseType {
    private:
	double &val;
    public:
	SetupBaseType_double(double &c_val) : val(c_val) {}
	void set(const std::string &str_val) {
	    val = strtod(str_val.c_str(), NULL);
	}
};

class SetupBaseType_string : public SetupBaseType {
    private:
	std::string &val;
    public:
	SetupBaseType_string(std::string &c_val) : val(c_val) {}
	void set(const std::string &str_val) {
	    val = str_val;
	}
};

class SetupBaseType_yesno : public SetupBaseType {
    private:
	bool &val;
    public:
	SetupBaseType_yesno(bool &c_val) : val(c_val) {}
	void set(const std::string &str_val) {
	    if (str_val == "yes") {
		val = true;
	    }
	    else {
		val = false;
	    }
	}
};

class SetupRow {
   public:
      std::string    key;
      SetupBaseType  *val_converter;
      bool           fatality;
      std::string    default_val;
      bool	     do_default;
      
      SetupRow(const std::string &n_key, SetupBaseType *n_val_converter, 
    		bool n_fatality, std::string &n_default_val, 
		bool n_do_default) :
         key(n_key),
	 val_converter(n_val_converter),
	 fatality(n_fatality),
	 default_val(n_default_val),
	 do_default(n_do_default) {}
      SetupRow(const char *n_key, SetupBaseType *n_val_converter, 
    		bool n_fatality, std::string &n_default_val,
		bool n_do_default) :
	 val_converter(n_val_converter),
	 fatality(n_fatality),
	 default_val(n_default_val),
	 do_default(n_do_default) {
	 key = n_key;
      }
      ~SetupRow() {
          delete val_converter;
	  val_converter = NULL;
      }
};

class SetupVars {
    private:
	std::vector<SetupRow *> var_row;
    public:
	~SetupVars() {
	    for (size_t i=0; i<var_row.size(); ++i) {
		delete var_row[i];
	    }
	};
	void Add(const std::string &key, SetupBaseType * sbt, 
		bool fatality, std::string default_val) {
	    var_row.push_back(new SetupRow(key, sbt, fatality, default_val, true));
	}
	void Add(const std::string &key, SetupBaseType * sbt, 
		bool fatality, const char* default_val) {
	    if (default_val == NULL ) {
		std::string t_str("");
		var_row.push_back(new SetupRow(key, sbt, fatality, t_str, false));
	    }
	    else {
		std::string t_str(default_val);
		var_row.push_back(new SetupRow(key, sbt, fatality, t_str, true));
	    }
	}
	void Add(const char *key, SetupBaseType * sbt, 
		bool fatality, std::string default_val) {
	    var_row.push_back(new SetupRow(key, sbt, fatality, default_val, true));
	}
	void Add(const char *key, SetupBaseType * sbt, 
		bool fatality, const char* default_val) {
	    if (default_val == NULL ) {
		std::string t_str("");
		var_row.push_back(new SetupRow(key, sbt, fatality, t_str, false));
	    }
	    else {
		std::string t_str(default_val);
		var_row.push_back(new SetupRow(key, sbt, fatality, t_str, true));
	    }
	}
	void Set(const Section *sect) {
	    std::string conf_value;
	    for (size_t i=0; i<var_row.size(); ++i) {
		try {
		    conf_value = sect->getVal(var_row[i]->key);
		    //var_row[i]->val_converter->set(conf_value);
		} catch (...) {
		    if (var_row[i]->fatality == true) { 
			//fatal error, should be right field in config file
			throw;
		    }
		    if (var_row[i]->do_default == true) {//set default value
			conf_value = var_row[i]->default_val;
			//var_row[i]->val_converter->set(var_row[i]->default_val);
		    }
		    else { //don't change variable if the default is NULL.
			continue;
		    }
		}
		try {
		    var_row[i]->val_converter->set(conf_value);
		} catch (errGetVal &er) {
		    //we have value but can't convert into right format
		    throw errSetVal(var_row[i]->key, conf_value, er.key);
		} catch (...) { 
		    //we have value but can't convert into right format
		    throw errSetVal(var_row[i]->key, conf_value);
		}
		
	    }
	}
};

#endif
