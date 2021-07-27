/**
 * Author: Pavel Shub
 *
 * Version:
 *	$Id: assembler.cpp,v 1.2 2007/05/23 03:12:46 cogent Exp cogent $
 *
 * Revisions:
 *	$Log: assembler.cpp,v $
 *	Revision 1.2  2007/05/23 03:12:46  cogent
 *	WSA complete!
 *
 *
*/
#include <iostream>
#include <fstream>
#include <map>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::ifstream;
using std::string;
using std::map;
using std::pair;

string num_to_wsn(string num, bool sign=false);
string next_ws_token(string &line);

int main(int argc, char *argv[]){
	if(argc < 2){
		cerr << "Usage: assember file.wsa" << endl;
		return EXIT_FAILURE;
	}
	// Prepare a table of opcodes
	map<string,string> opc;
	// Stack Manipulation
	opc.insert( pair<string,string>("push","  ") );
	opc.insert( pair<string,string>("dup", " \n ") );
	opc.insert( pair<string,string>("swap", " \n\t") );
	opc.insert( pair<string,string>("discard", " \n\n") );
	opc.insert( pair<string,string>("copy", " \t ") );
	opc.insert( pair<string,string>("slide", " \t\n") );
	// Arithmatic
	opc.insert( pair<string,string>("add", "\t   ") );
	opc.insert( pair<string,string>("sub", "\t  \t") );
	opc.insert( pair<string,string>("mul", "\t  \n") );
	opc.insert( pair<string,string>("div", "\t \t ") );
	opc.insert( pair<string,string>("mod", "\t \t\t") );
	// Heap access
	opc.insert( pair<string,string>("store", "\t\t ") );
	opc.insert( pair<string,string>("retrieve", "\t\t\t") );
	// Flo' Control
	opc.insert( pair<string,string>("label", "\n  ") );
	opc.insert( pair<string,string>("call", "\n \t") );
	opc.insert( pair<string,string>("jump", "\n \n") );
	opc.insert( pair<string,string>("jz", "\n\t ") );
	opc.insert( pair<string,string>("jn", "\n\t\t") );
	opc.insert( pair<string,string>("ret", "\n\t\n") );
	opc.insert( pair<string,string>("exit", "\n\n\n") );
	// IO
	opc.insert( pair<string,string>("outchar", "\t\n  ") );
	opc.insert( pair<string,string>("outnum", "\t\n \t") );
	opc.insert( pair<string,string>("readchar", "\t\n\t ") );
	opc.insert( pair<string,string>("readnum", "\t\n\t\t") );

	// Start actual program.
	string program;
	string opc_com;
	//ostream out = cout;
	//string fname("test.wsa");
	ifstream in;
	in.open(argv[1]);
	string line;
	while(! in.eof()){
		getline(in,line);
		if(line[0] == '#'){
			continue;
		}
		opc_com = next_ws_token(line);
		// Check for : label
		if(*(opc_com.rbegin())==':'){
			opc_com.resize(opc_com.length()-1);
			line = opc_com;
			opc_com = "label";
		}
		//cout << "Testing: " << "opcode is " << opc_com <<endl;
		// Check for parameters that accept numbers
		if(opc_com == "push" || opc_com == "slide" ||
			opc_com == "copy"){
			program += opc[opc_com];
			program += num_to_wsn(next_ws_token(line),true)+'\n';
		}
		else{
			// Check for parameters that accept labels
			if( opc_com == "label" || opc_com == "call" ||
				opc_com == "jump" || opc_com == "jz" ||
				opc_com == "jn"){
				program += opc[opc_com];
				program += num_to_wsn(next_ws_token(line))+'\n';
			}
			else{
				// Just add the opcode
				program += opc[opc_com];
			}
		}
	}
	cout << program << flush;
	return EXIT_SUCCESS;
}

string num_to_wsn(string num, bool sign){
	long dnum(atol(num.c_str()));
	bool neg(false);
	string res("");
	int rem;
	if(dnum < 0){
		neg = true;
		dnum *= -1;
	}
	while(dnum > 0){
		rem = dnum % 2;
		dnum /= 2;
		res = (rem == 0 ? ' ' : '\t') + res;
	}
	if(sign){
		if(neg){
			return ('\t'+res);
		}
		else{
			return (' '+res);
		}
	}
	return res;
}

string next_ws_token(string &line){
	unsigned int i(0);
	//skip starting ws
	while(line.length() > i && (line[i] == ' ' || line[i] == '\t') ){
		i++;
	}
	int start(i);
	while(line.length() > i && (line[i] != ' ' && line[i] != '\t') ){
		i++;
	}
	string res(line.substr(start, i - start));
	line = line.substr(i, i - line.length());
	return res;
}
