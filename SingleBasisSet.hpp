#ifndef __SINGLE_BASIS_SET_HPP__
#define __SINGLE_BASIS_SET_HPP__
#include"GamessFormat.hpp"
#include<fstream>

#define __error_sbs__  "Error: SingleBasisSet<T>::"

using namespace std;

template<class T>
struct SingleBasisSet
{
protected:
	map<vector<string>, vector<vector<vector<pair<T, T> > > > > content;
public:
	void print_basisSet(char const * __file_name)const;
	void print_basisSet(ostream & out)const;
	void print_basisSet()const;
	int import_gamessFormat(char const * __file_name);
	int import_molproFormat(char const * __file_name);
	int import_molproIntFormat(char const * __file_name);
};

template<class T>
int SingleBasisSet<T>::import_gamessFormat(char const * __file_name)
{
	if( !content.empty() )
	{
		cerr << __error_sbs__ << "import_gamessFormat(char const * __file_name)" << endl;
		cerr << "Basis set already exists!" << endl; 
		cerr << "__file_name : \'" << __file_name << "\'" << endl;
		return 1;
	}
	vector<string> elementLabel;
	vector<vector<vector<pair<T, T> > > > elementContent;
	GamessFormat<T> gamess_fmt( __file_name );
	string str;
	int exitCode = 0;
	ifstream inp( __file_name );
	// open file
	if( !inp.is_open() )
	{
		cerr << __error_sbs__ << "import_gamessFormat(char const * __file_name)" << endl;
		cerr << "Can't open file \'" << __file_name << '\'' << endl;
		return 2;
	}
	// find begining of basis set
	if( gamess_fmt.find_head(inp) )
	{
		cerr << __error_sbs__ << "import_gamessFormat(char const * __file_name)" << endl;
		cerr << "Can't find begining of basis set: \'$DATA\'" << endl;
		cerr << "__file_name : \'" << __file_name << "\'" << endl;
		return 3;
	}
	// read basis set
	while(true)
	{
		// get element name
		exitCode = gamess_fmt.get_elementName(inp, elementLabel);
		if( exitCode == -1 )// last line
			return 0;
		else if( exitCode )// error: last line not found or line doesn't match element name
		{
			//cerr << __error_sbs__ << "import_gamessFormat(char const * __file_name)" << endl;
			cerr << " from SingleBasisSet<T>::import_gamessFormat(char const * __file_name)" << endl;
			cerr << "exitCode : " << exitCode << endl;
			return 4;
		}
		if( content.find( elementLabel ) != content.end() )
		{
			cerr << __error_sbs__ << "import_gamessFormat(char const * __file_name)" << endl;
			cerr << "such element \'" << str << "\' already exists" << endl;
			gamess_fmt.info_msg(cerr);
			return 5;
		}
		// get element (basis set) content
		exitCode = gamess_fmt.get_elementContent(inp, elementContent );
		if( exitCode == -1 )// last line
		{
			content[elementLabel] = elementContent;
			return 0;
		}
		else if( exitCode )
		{
			//cerr << __error_sbs__ << "import_gamessFormat(char const * __file_name)" << endl;
			cerr << " from SingleBasisSet<T>::import_gamessFormat(char const * __file_name)" << endl;
			cerr << "exitCode : " << exitCode << endl;
			return 6;
		}
		content[elementLabel] = elementContent;
	}
	cerr << __error_sbs__ << "import_gamessFormat(char const * __file_name)" << endl;
	cerr << "Something goes wrong..." << endl;
	cerr << "__file_name : \'" << __file_name << "\'" << endl;
	return 7;
}

#endif//__SINGLE_BASIS_SET_HPP__
