#ifndef __MOLPRO_FORMAT_HPP__
#define __MOLPRO_FORMAT_HPP__
#include"Format.hpp"
#include<boost/regex.hpp>

using boost::regex;
using boost::regex_match;
using boost::regex_search;
using boost::smatch;

struct molpro_regex
{
	boost::regex first_line, last_line, empty_or_comment, empty_string, comment_only, el, pars, shell, no_L_shell, L_shell;
	molpro_regex():
		first_line("^\\s*\\basis\\s*(!.*)*$"), last_line("^\\s*\\$END\\s*(!.*)*$"), empty_or_comment("^\\s*(!.*)*$"),
		empty_string("^\\s*$"), comment_only("^\\s*!.*$"), el("^\\s*[a-zA-Z]{4,}.*$"), pars("\\s*[^\\s]+"), 
		shell("^\\s*([a-zA-Z])\\s+(\\d+)\\s*$"),
		no_L_shell("^\\s*\\d+\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s*(!.*)*$"),
		L_shell("^\\s*\\d+\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s*(!.*)*$")
		{}
};

template<class T>
struct MolproFormat: public Format<T, molpro_regex>
{
	MolproFormat():Format<T, molpro_regex>(){}
	MolproFormat(char const * __file_name):Format<T, molpro_regex>(__file_name){}
	virtual int find_head(std::istream &);
	virtual int get_elementName(std::istream &, std::vector<std::string> & );
	virtual int get_elementContent(std::istream & , std::vector<std::vector<std::vector<std::pair<T, T> > > > & );
protected:
	virtual int get_LN(int &, int&)const;
};

template<class T>
int MolproFormat<T>::find_head(std::istream & inp)
{
	std::string & str = this->line;
	molpro_regex * regx = &this->fmt_regex;
	while( std::getline(inp, str) )
	{
		this->line_number++;
		if( boost::regex_match(str, regx->empty_or_comment) ) continue;
		if( boost::regex_match(str, regx->first_line) )
			return 0;
		std::cerr << "Error: [MolproFormat<T>::find_head(istream &)]" << std::endl;
		this->info_msg(std::cerr);
		return 1;
	}
	std::cerr << "Error: [MolproFormat<T>::find_head(istream &)]" << std::endl;
	std::cerr << "file \'" << this->file_name << "\' is empty!" << std::endl;
	return 2;
}

template<class T>
int MolproFormat<T>::get_elementName(std::istream & inp, std::vector<std::string> & elementLabel )
{
	std::string & str = this->line;
	molpro_regex * regx = &this->fmt_regex;
	while( std::getline(inp, str) )
	{
		this->line_number++;
		if( boost::regex_match(str, regx->empty_or_comment) ) continue;
		if( boost::regex_match(str, regx->last_line ) ) return -1;
		if( boost::regex_match(str, regx->el ) )
		{
			elementLabel.clear();
			boost::sregex_iterator it(str.begin(), str.end(), regx->pars);
			boost::sregex_iterator itbad;
			while( it != itbad )
				elementLabel.push_back( it++->str() );
			return 0;
		}
		std::cerr << "Error: [MolproFormat<T>::get_elementName(istream &, vector<string> &)]" << std::endl;
		std::cerr << "line doesn't match any element name" << std::endl;
		this->info_msg(std::cerr);
		return 1;
	}
	std::cerr << "Error: [MolproFormat<T>::get_elementName(istream &, vector<string> &)]" << std::endl;
	std::cerr << "unexpected end of input \"" << this->file_name << "\": last line \'$END\' not found" << std::endl;
	return 2;
}

template<class T>
int MolproFormat<T>::get_LN(int & L, int & N)const
{
	boost::smatch result;
	molpro_regex const * regx = &this->fmt_regex;
	if( !boost::regex_search(this->line, result, regx->shell) ) return 1;
	int tmp_l = -2;
	if( result[1].str() =="s" || result[1].str() =="S" ) tmp_l = 0;
	if( result[1].str() =="p" || result[1].str() =="P" ) tmp_l = 1;
	if( result[1].str() =="d" || result[1].str() =="D" ) tmp_l = 2;
	if( result[1].str() =="f" || result[1].str() =="F" ) tmp_l = 3;
	if( result[1].str() =="g" || result[1].str() =="G" ) tmp_l = 4;
	if( result[1].str() =="h" || result[1].str() =="H" ) tmp_l = 5;
	if( result[1].str() =="i" || result[1].str() =="I" ) tmp_l = 6;
	if( result[1].str() =="k" || result[1].str() =="K" ) tmp_l = 7;
	if( result[1].str() =="l") tmp_l = 8;
	if( result[1].str() =="L") tmp_l = -1;
	if( result[1].str() =="m" || result[1].str() =="M" ) tmp_l = 9;
	if( result[1].str() =="n" || result[1].str() =="N" ) tmp_l = 10;
	if( tmp_l == -2 ) return 2;
	N = std::atoi( result[2].str().c_str() );
	if( N == 0 )
		std::cerr << "Warning: found 0 primitives to be set" << std::endl;
	L = tmp_l;
	return 0;
}

template<class T>
int MolproFormat<T>::get_elementContent(std::istream & inp, std::vector<std::vector<std::vector<std::pair<T, T> > > > & elementContent)
{
	T Ai = T(0), Ci = T(0), CLi = T(0);
	int N = 0, L = 0;
	boost::smatch result;
	boost::smatch CiAiNoL;
	boost::smatch CiAiL;
	elementContent.clear();
	elementContent.resize(20 ); // с запасом, потом отрезать
	int Lmax=-1; //	а это как раз будет запоминать максимальную L, чтобы остальное отрезать
	std::vector<std::pair<T, T> > basisFunc1;
	std::vector<std::pair<T, T> > basisFunc2;  // для L оболочки
	std::string & str = this->line;
	molpro_regex * regx = &this->fmt_regex;
	while (std::getline(inp, str) ) {
		this->line_number++;
		if( boost::regex_match(str, regx->empty_string) )
		{
			elementContent.resize(Lmax+1);
			return 0;
		}
		if( boost::regex_match(str, regx->comment_only) ) continue; 
		if( boost::regex_match(str, regx->last_line))// last line found
		{
			std::cerr << "Warning: [MolproFormat<T>::get_elementContent(istream &, ...)]" << std::endl;
			std::cerr << "There should be empty line befor key word \"$END\"" << std::endl;
			this->info_msg(std::cerr);
			elementContent.resize(Lmax+1);
			return -1;
		}
		if( !boost::regex_match(str, result, regx->shell) )
		{
			std::cerr << "Error: [MolproFormat<T>::get_elementContent(istream &, ...)]" << std::endl;
			std::cerr << "line " << this->line_number << " : " << this->line << ", doesn't match any shell" << std::endl;
			return 1;
		}
		if( this->get_LN(L,N) )
		{
			std::cerr << "Error: [MolproFormat<T>::get_elementContent(istream &, ...)]" << std::endl;
			std::cerr << "line " << this->line_number << " : " << this->line << std::endl;
			std::cerr << "expected format of input : \"shell-name    number-of-primitives\"" << std::endl;
			return 2;
		}
		// вот тут мы можем находиться после того, как считалась оболочка
		if( Lmax < L ) Lmax = L;
		basisFunc1.clear();
		basisFunc2.clear();
		for (int i = 0; i < N; i++)
		{
			if( !std::getline(inp, str) )
			{
				std::cerr << "Error: [MolproFormat<T>::get_elementContent(istream &, ...)]" << std::endl;
				std::cerr << "[3] unexpected end of input \"" << this->file_name << "\": last line \"$END\" not found" << std::endl;
				return 3;
			}
			this->line_number++;
			if( boost::regex_match(str, regx->comment_only) )
			{
				i--;
				continue; 
			}
			if( L == -1 && boost::regex_match(str, CiAiL, regx->L_shell) )
			{
				Ai = atof( (CiAiL[1].str()).c_str() );
				Ci = atof( (CiAiL[2].str()).c_str() );
				CLi= atof( (CiAiL[3].str()).c_str() );
				//std::cout << Ai << " " << Ci << " " << CLi << std::endl;
				basisFunc1.push_back(std::make_pair(Ai, Ci));
				basisFunc2.push_back(std::make_pair(Ai, CLi));
				continue;
			}
			if( L > -1 && boost::regex_match(str, CiAiNoL, regx->no_L_shell) )
			{
				Ai = atof( (CiAiNoL[1].str()).c_str() );
				Ci = atof( (CiAiNoL[2].str()).c_str() );
				//std::cout << Ai << " " << Ci << std::endl;
				basisFunc1.push_back(std::make_pair(Ai, Ci));
				continue;
			}
			std::cerr << "Error: [MolproFormat<T>::get_elementContent(istream &, ...)]" << std::endl;
			std::cerr << "line " << this->line_number << " : \"" << this->line << "\" doesn't match any shell" << std::endl;
			std::cerr << "reading from file \"" << this->file_name << "\" will be interrupted" << std::endl;
			return 4;
		}
		if(L == -1)
		{
			elementContent[0].push_back(basisFunc1);
			elementContent[1].push_back(basisFunc2);
		}
		else
			elementContent[L].push_back(basisFunc1);
	}
	std::cerr << "Error: [MolproFormat<T>::get_elementContent(istream &, ...)]" << std::endl;
	std::cerr << "[5] unexpected end of input \"" << this->file_name << "\": last line \"$END\" not found" << std::endl;
	return 5;
}
#endif//__MOLPRO_FORMAT_HPP__
