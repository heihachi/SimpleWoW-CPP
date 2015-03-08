/*
 * Config.cpp
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

#include <cstdio>
#include <iostream>
#include "Config.h"
#include "config.tab.h"

using namespace std;

/*
const int CONF_OP_IS_SET	= 0; // only value exist.
const int CONF_OP_EQ		= 1; // ==
const int CONF_OP_NE		= 2; // !=
const int CONF_OP_RLIKE		= 3; // =~
const int CONF_OP_NOT_RLIKE	= 4; // !~
*/

std::string ConfVP :: print() const {
    std::string ret;
    switch (op) {
	case CONF_OP_IS_SET:
	    ret = "\"" + key + "\"";
	    break;
	case CONF_OP_EQ:
	    ret = key + " == \"" + value + "\"";
	    break;
	case CONF_OP_NE:
	    ret = key + " != \"" + value + "\"";
	    break;
	case CONF_OP_RLIKE:
	    ret = key + " =~ \"" + value + "\"";
	    break;
	case CONF_OP_NOT_RLIKE:
	    ret = key + " !~ \"" + value + "\"";
	    break;
    }
    return ret;
}

std::string Section :: getVal(const std::string key) const {
    CII ii = item.find(key);
    if (ii == item.end()) {
       throw errGetVal(key);
    }
    else {
       return ii->second;
    }
};


Section * parseConfig(const char *confFileName) {
    if (confFileName == NULL) {
	conf_in = stdin;
    }
    else {
	conf_in = fopen(confFileName, "r");
    }
    if (conf_in == NULL) {
	return NULL;
    }

    Section *conf_p = new Section;
    
    ParserParam pp;
    ConfEntry ce;
    ce.f = conf_in;
    ce.f_name = confFileName;
    ce.sect = conf_p;
    pp.confEntryQueue.push(ce);
    pp.root_p = conf_p;

    try {
      while (!pp.confEntryQueue.empty()) {
	ce = pp.confEntryQueue.front();
	pp.confEntryQueue.pop();
	conf_in = ce.f;
	pp.sect_stack.push(ce.sect);
	if (conf_parse((void*) &pp)!=0) {
	    printf(" in file %s\n", ce.f_name.c_str());
	    delete conf_p;
	    conf_p = NULL;
	    fclose(conf_in);
	    break;
	}
	fclose(conf_in);
      }
    } catch(...) {
	if (conf_p != NULL) delete conf_p;
	conf_p = NULL;
	fclose(conf_in);
    }
    return conf_p;
}

Section :: ~Section() {
    for (SI p = sectionList.begin();
	    p != sectionList.end(); 
	    p++) {
	delete (p->second);
    }
}

std::string Section::getArgString() const {
    std::string r_args;
    int i=0;
    for (Args_CI a_i = args.begin(); a_i != args.end(); ++a_i, ++i) {
	if (i==0) {
	    r_args += a_i->print();
	}
	else {
	    r_args += ", " + a_i->print();
	}
    }
    return r_args;
}

void printSection(std::ostream &Out, Section *sect, int level) {
    typedef map<string,Section *>::const_iterator CI;
    std::string s_type;
    for (CI p = sect->sectionList.begin();
	    p != sect->sectionList.end(); 
	    p++) {
	for (int i=0; i<level; i++) {
	    Out << "   ";
	}
	
	if ((s_type = p->second->getType()) == "") {
	    Out << p->first << " { " << std::endl;
	}
	else {
	    Out << "//" << p->first << std::endl;
	    for (int i=0; i<level; i++) {
		Out << "   ";
	    }
	    Out << s_type << " (" << p->second->getArgString() << ") { " << std::endl;
	}
	printSection(Out, p->second, level+1);
	for (int i=0; i<level; i++) {
             Out << "   ";
        }
	Out << "}" << std::endl;
    }
    
    typedef map<string, string>::const_iterator CIV;
    for (CIV p = sect->item.begin();
	    p != sect->item.end(); 
	    p++) {
	for (int i=0; i<level; i++) {
	    Out << "   ";
	}
	Out << p->first << " = \"" << p->second << "\";" << endl;
    }
}


void printSection(Section *sect, int level) {
    printSection(std::cout, sect, level);
}
