#ifndef __FORMAT_HPP__
#define __FORMAT_HPP__
#include<vector>
#include<string>
#include<iostream>
#include<map>

using std::vector;
using std::string;
using std::pair;
//
using std::istream;
using std::ostream;
//
using std::cerr;
using std::endl;
//

template<class T, class regexT>
struct Format
{
	typedef T value_type;
	typedef regexT regex_type;
protected:
	virtual int get_LN(int & L, int & N)const{return 1;}
	int line_number;
	std::string line;
	char const * file_name;
	regexT fmt_regex;
public:
	Format():line_number(0), line(), file_name(0), fmt_regex(){}
	Format(char const * __file_name):line_number(0), line(), file_name(__file_name), fmt_regex(){}
	virtual ~Format(){}
	virtual int find_head(std::istream & inp){return 1;}
	virtual int get_elementName(std::istream & inp, std::vector<std::string> & element_label ){return 1;}
	virtual int get_elementContent(std::istream & inp, std::vector<std::vector<std::vector<std::pair<T, T> > > > & element_content){return 1;}
	virtual void info_msg(std::ostream & out)const
	{
		out << "file_name : '" << file_name << "'" << std::endl;
		out << "line_number : " << line_number << std::endl;
		out << "line : " << line << std::endl;
	}
};

#endif//__FORMAT_HPP__
