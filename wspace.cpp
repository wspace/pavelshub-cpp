/**
 *   Copyright (C) 2008  Pavel Shub
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Version:
 *	$Id: wspace.cpp,v 1.8 2008/04/04 21:51:02 cogent Exp cogent $
 * Revisions:
 *	$Log: wspace.cpp,v $
 *	Revision 1.8  2008/04/04 21:51:02  cogent
 *	fixed up command line parameters, done?
 *
 *	Revision 1.7  2007/07/24 02:40:50  cogent
 *	all bugs fixed
 *
 *	Revision 1.6  2007/07/22 05:27:20  cogent
 *	fixed a lot of errors. works for some files not all.
 *
 *	Revision 1.5  2007/07/22 03:21:47  cogent
 *	added lots of flags and command line arguments
 *	still errors to debug
 *
 *	Revision 1.4  2007/07/22 00:07:49  cogent
 *	changed pcs from iterator to unsigned long and it works
 *
 *	Revision 1.3  2007/07/21 23:01:40  cogent
 *	tried to fix bugs. Going for a new strategy
 *
 *	Revision 1.2  2007/07/21 19:50:22  cogent
 *	done, bugs exist?
 *
 *	Revision 1.1  2007/05/26 17:19:24  cogent
 *	Initial revision
 *
 *
*/
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>


using namespace std;

// Constants
const char C('\n');	// Newline
const char B('\t');	// Tab
const char A(' ');	// Space

// Stuff to kill computer memory:

// Memory
map<long, long> heap;
vector<long> stack;
// Internal tables
map<long, unsigned long> symtab;
map<unsigned long, long> numtab;

vector<unsigned long> return_stack;
vector<int> program_stack;

// Global flags
bool print_asm(false);
bool print_debug(false);
bool print_letters(false);
bool print_detail(false);

// Function definitions
long convert_num(string wsnum, bool sign=false);
void show_usage();
void show_help();

// Program start
int main (int argc, char *argv[])
{
	// Pre-game
	if(argc < 2 ){
		show_usage();
		return EXIT_FAILURE;
	}
	// filter the command line parameters
	if(argc > 2){
		if(strchr(argv[1], 'a') != NULL) print_asm = true;
		if(strchr(argv[1], 'g') != NULL) print_debug = true;
		if(strchr(argv[1], 'l') != NULL) print_letters = true;
		if(strchr(argv[1], 'd') != NULL) print_detail = true;
	}
	// print the help screen and exit
	if(strcmp(argv[1], "h") == 0 || strcmp(argv[1], "--help") == 0){
		show_help();
		return EXIT_SUCCESS;
	}
	// Get the whole file and put it into c_string called program_start
	long length;
	ifstream in;
	in.open(argv[argc-1]);
	if ( in.fail() ){
		cerr << "Error opening file." << endl;
		return EXIT_FAILURE;
	}
	in.seekg(0, ios::end);
	length = in.tellg();
	in.seekg(0, ios::beg);
	char *program_start = new char[length];
	char *program_end = program_start + length;
	in.read(program_start,length);
	in.close();

	// Variables for processing
	string num("");
	int stage(0);
	long tmp;
	long in_num(0);
	char *pc(program_start);	// Program counter

	// Start the first pass.
	// All the commands will be read and put into the program_stack. All labels
	// will be put into symtab and all numbers will be read, coverted to long and
	// put into numtab. This also does syntax checking.
	while(pc < program_end){
		// This conditional skips letters that aren't whitespace
		if(*pc != A && *pc != B && *pc != C){
			pc++;
			continue;
		}
		// If the argument 'l' was passed then this program will only print the
		// whitespace program as letters ABC.
		if(print_letters){
			if(*pc == A) cout << "A";
			if(*pc == B) cout << "B";
			if(*pc == C) cout << "C" << endl;
			continue;
		}
		switch(stage){
			case 0: switch(*pc){
					case A: stage = 2; break;
					case B: stage = 1; break;
					case C: stage = 9; break;
				} break;
			case 1: switch(*pc){
					case A: stage = 5; break;
					case B: stage = 8; break;
					case C: stage = 13;break;
				} break;
			case 2: switch(*pc){
					case A: stage = 16; num = ""; break;
					case B: stage = 3; break;
					case C: stage = 4; break;
				} break;
			case 3: switch(*pc){
					case A: stage = 17; num = "";
					break;
					case B: stage = -1; break;
					case C: stage = 18; num = "";
					break;
				} break;
			case 4: switch(*pc){
					case A: if(print_asm){
							cout << "dup" << endl;
						}
						program_stack.push_back(104);
						stage = 0;
						break;
					case B: if(print_asm){
							cout << "swap" << endl;
						}
						program_stack.push_back(105);
						stage = 0;
						break;
					case C: if(print_asm){
							cout << "discard" << endl;
						}
						program_stack.push_back(106);
						stage = 0;
						break;
				} break;
			case 5: switch(*pc){
					case A: stage = 6; break;
					case B: stage = 7; break;
					case C: stage = -2; break;
				} break;
			case 6: switch(*pc){
					case A: if(print_asm){
							cout << "add" << endl;
						}
						program_stack.push_back(114);
						stage = 0;
						break;
					case B: if(print_asm){
							cout << "sub" << endl;
						}
						program_stack.push_back(115);
						stage = 0;
						break;
					case C: if(print_asm){
							cout << "mul" << endl;
						}
						program_stack.push_back(116);
						stage = 0;
						break;
				} break;
			case 7: switch(*pc){
					case A: if(print_asm){
							cout << "div" << endl;
						}
						program_stack.push_back(117);
						stage = 0;
						break;
					case B: if(print_asm){
							cout << "mod" << endl;
						}
						program_stack.push_back(118);
						stage = 0;
						break;
					case C: stage = -3; break;
				} break;
			case 8: switch(*pc){
					case A: if(print_asm){
							cout << "store" << endl;
						}
						program_stack.push_back(119);
						stage = 0;
						break;
					case B: if(print_asm){
							cout << "retrieve" << endl;
						}
						program_stack.push_back(120);
						stage = 0;
						break;
					case C: stage = -4; break;
				} break;
			case 9: switch(*pc){
					case A: stage = 10; break;
					case B: stage = 11; break;
					case C: stage = 12; break;
				} break;
			case 10: switch(*pc){
					case A: stage = 19; num = ""; break;
					case B: stage = 20; num = ""; break;
					case C: stage = 21; num = ""; break;
				} break;
			case 11: switch(*pc){
					case A: stage = 22; num = ""; break;
					case B: stage = 23; num = ""; break;
					case C: if(print_asm){
							cout << "ret" << endl;
						}
						program_stack.push_back(112);
						stage = 0;
						break;
				} break;
			case 12: switch(*pc){
					case A: stage = -5; break;
					case B: stage = -6; break;
					case C: if(print_asm){
							cout << "exit" << endl;
						}
						program_stack.push_back(113);
						stage = 0;
						break;
				} break;
			case 13: switch(*pc){
					case A: stage = 14; break;
					case B: stage = 15; break;
					case C: stage = -7; break;
				} break;
			case 14: switch(*pc){
					case A: if(print_asm){
							cout << "outchar" << endl;
						}
						program_stack.push_back(121);
						stage = 0;
						break;
					case B: if(print_asm){
							cout << "outnum" << endl;
						}
						program_stack.push_back(122);
						stage = 0;
						break;
					case C: stage = -8; break;
				} break;
			case 15: switch(*pc){
					case A: if(print_asm){
							cout << "readchar" << endl;
						}
						program_stack.push_back(123);
						stage = 0;
						break;
					case B: if(print_asm){
							cout << "readnum" << endl;
						}
						program_stack.push_back(124);
						stage = 0;
						break;
					case C: stage = -9; break;
				} break;
			// Cascade push, copy, slide. Because code is the same
			// only stage varies, 16+85 = 101, 18 + 85 = 103
			case 16:
			case 17:
			case 18: switch(*pc){
					case A: num += *pc; break;
					case B: num += *pc; break;
					case C: if(num.length() < 2){
							stage = -10;
							break;
						}
						tmp = convert_num(num, true);
						if(print_asm){
							switch(stage){
								case 16: cout << "push " << tmp << endl;
									break;
								case 17: cout << "copy " << tmp << endl;
									break;
								case 18: cout << "slide " << tmp << endl;
									break;
							}
						}
						program_stack.push_back(stage + 85);
						numtab[program_stack.size()-1] = tmp;
						stage = 0;
						break;
 					} break;
 			// Handle labels
 			case 19:
 				switch(*pc){
					case A: num += *pc; break;
					case B: num += *pc; break;
					case C: if(num.length() < 1){
							stage = -10;
							break;
						}
						tmp = convert_num(num,false);
						if(print_asm){
							cout << "label " << tmp << endl;
						}
						symtab[tmp] = program_stack.size();
						program_stack.push_back(107);
						stage = 0;
						break;
 					} break;
 			// Same thing as last cascade, but for call, jump, jn,
 			// and jz
 			case 20:
 			case 21:
 			case 22:
 			case 23: switch(*pc){
					case A: num += *pc; break;
					case B: num += *pc; break;
					case C: if(num.length() < 1){
							stage = -10;
							break;
						}
						tmp = convert_num(num, false);
						if(print_asm){
							switch(stage){
								case 20: cout << "call " << tmp << endl;
									break;
								case 21: cout << "jump " << tmp << endl;
									break;
								case 22: cout << "jz " << tmp << endl;
									break;
								case 23: cout << "jn " << tmp << endl;
									break;
							}
						}
						program_stack.push_back(stage + 88);
						numtab[program_stack.size()-1] = tmp;
						stage = 0;
						break;
 					} break;
		}
		// Print a meaningful message if a syntax error is encountered
		if( stage < 0 ){
			cerr << "Syntax Error" << endl;
			if(stage == -10){
				cerr << "Numerical Value not Entered." << endl;
				return EXIT_FAILURE;
			}
			cerr << "Non-Existant command entered: ";
			switch(stage){
				case -1: cerr << "[Spase][Tab][Tab]";
					break;
				case -2: cerr << "[Tab][Space][LF]";
					break;
				case -3: cerr << "[Tab][Space][Tab][LF]";
					break;
				case -4: cerr << "[Tab][Tab][LF]";
					break;
				case -5: cerr << "[LF][LF][Space]";
					break;
				case -6: cerr << "[LF][LF][Tab]";
					break;
				case -7: cerr << "[Tab][LF][LF]";
					break;
				case -8: cerr << "[Tab][LF][Space][LF]";
					break;
				case -9: cerr << "[Tab][LF][Tab][LF]";
					break;
			}
			cerr << " after command # " << program_stack.back() << endl;
			return EXIT_FAILURE;
		}
		pc++;
	}
	// Delete the ws file from memory
	delete[] program_start;
	// If they wanted to see the letters or ws assembly then exit and don't execute
	if(print_asm || print_letters){
		return EXIT_SUCCESS;
	}

	// variables that are needed for the execution of the program
	unsigned long pcs_size = program_stack.size();
	unsigned long pcs = 0; // program counter for stack
	map<long, unsigned long>::iterator check; // check var for pcs
	map<long, long>::iterator hcheck; // check var for heap
	char debug_char('s');
	long arg1, arg2;

	// now do the actual execution of the program
	while( pcs < pcs_size ){
		if(print_debug){
			cout << "#" << pcs+1 << " ";
		}
		switch( program_stack[pcs] ){
			// push
			case 101: stack.push_back(numtab[pcs]);
				if(print_debug) cout << " push " << numtab[pcs] <<  "\t: ";
				break;
			// copy
			case 102: if(stack.empty()){
					cerr << "Error: Cannot copy, stack is empty." << endl;
					return EXIT_FAILURE;
				}
				stack.push_back( *(stack.end() - ( numtab[pcs]+1 )) );
				if(print_debug) cout << " copy " << numtab[pcs] << "\t: ";
				break;
			// slide
			case 103: if(stack.empty()){
					cerr << "Error: Cannot slide, stack is empty." << endl;
					return EXIT_FAILURE;
				}
				stack.erase( stack.end() - (numtab[pcs]+1), stack.end() - 1 );
				if(print_debug) cout << " slide " << numtab[pcs] << "\t: ";
				break;
			// dup
			case 104: if(stack.empty()){
					cerr << "Error: Cannot dup, stack is empty." << endl;
					return EXIT_FAILURE;
				}
				stack.push_back( stack.back() );
				if(print_debug) cout << " dup\t: ";
				break;
			// swap
			case 105: if(stack.size() < 2){
					cerr << "Error: Cannot swap, not enough elements on the stack." << endl;
					return EXIT_FAILURE;
				}
				arg1 = stack.back();
				stack.pop_back();
				arg2 = stack.back();
				stack.pop_back();
				stack.push_back(arg1);
				stack.push_back(arg2);
				if(print_debug) cout << " swap\t: ";
				break;
			// discard
			case 106: stack.pop_back();
				if(print_debug) cout << " discard\t: ";
				break;
			// label
			case 107:
				if(print_debug) cout << " label " << numtab[pcs] << "\t: ";
				break;
			// call
			case 108: return_stack.push_back(pcs);
				check = symtab.find(numtab[pcs]);
				if(check != symtab.end()){
					pcs = check->second;
				}
				else{
					cerr << "Error: Cannot jump to " << numtab[pcs] << endl;
					return EXIT_FAILURE;
				}
				if(print_debug) cout << " call " << numtab[pcs] << "\t: ";
				break;
			// jump
			case 109:
				check = symtab.find(numtab[pcs]);
				if(check != symtab.end()){
					pcs = check->second;
				}
				else{
					cerr << "Error: Cannot jump to " << numtab[pcs] << endl;
					return EXIT_FAILURE;
				}
				if(print_debug) cout << " jump " << numtab[pcs] << "\t: ";
				break;
			// jz
			case 110: if(stack.back() == 0){
					check = symtab.find(numtab[pcs]);
					if(check != symtab.end()){
						pcs = check->second;
					}
					else{
						cerr << "Error: Cannot jump to " << numtab[pcs] << endl;
						return EXIT_FAILURE;
					}
				}
				stack.pop_back();
				if(print_debug) cout << " jz " << numtab[pcs] << "\t: ";
				break;
			// jn
			case 111: if(stack.back() < 0){
					check = symtab.find(numtab[pcs]);
					if(check != symtab.end()){
						pcs = check->second;
					}
					else{
						cerr << "Error: Cannot jump to " << numtab[pcs] << endl;
						return EXIT_FAILURE;
					}
				}
				stack.pop_back();
				if(print_debug) cout << " jn " << numtab[pcs] << "\t: ";
				break;
			// ret
			case 112: if(!return_stack.empty()){
					pcs = *(return_stack.end()-1);
				}
				else{
					cerr << "Error: Cannot return, nothing to return to." << endl;
					return EXIT_FAILURE;
				}
				return_stack.pop_back();
				if(print_debug) cout << " push\t: ";
				break;
			// exit
			case 113: pcs = program_stack.size() - 1;
				if(print_debug) cout << " exit\t: ";
				break;
			// add
			case 114: arg1 = stack.back();
				stack.pop_back();
				arg2 = stack.back();
				stack.pop_back();
				stack.push_back(arg2 + arg1);
				if(print_debug) cout << " add\t: ";
				break;
			// sub
			case 115: arg1 = stack.back();
				stack.pop_back();
				arg2 = stack.back();
				stack.pop_back();
				stack.push_back(arg2 - arg1);
				if(print_debug) cout << " sub\t: ";
				break;
			// mul
			case 116: arg1 = stack.back();
				stack.pop_back();
				arg2 = stack.back();
				stack.pop_back();
				stack.push_back(arg2 * arg1);
				if(print_debug) cout << " mul\t: ";
				break;
			// div
			case 117: arg1 = stack.back();
				stack.pop_back();
				arg2 = stack.back();
				stack.pop_back();
				if(arg1 == 0){
					cerr << "Error: Cannot div by zero." << endl;
					return EXIT_FAILURE;
				}
				stack.push_back(arg2 / arg1);
				if(print_debug) cout << " div\t: ";
				break;
			// mod
			case 118: arg1 = stack.back();
				stack.pop_back();
				arg2 = stack.back();
				stack.pop_back();
				if(arg1 == 0){
					cerr << "Error: Cannot mod by zero." << endl;
					return EXIT_FAILURE;
				}
				stack.push_back(arg2 % arg1);
				if(print_debug) cout << " mod\t: ";
				break;
			// store
			case 119: arg1 = stack.back();
				stack.pop_back();
				arg2 = stack.back();
				stack.pop_back();
				heap[arg2] = arg1;
				if(print_debug) cout << " store\t: ";
				break;
			// retrieve
			case 120: arg1 = stack.back();
				stack.pop_back();
				hcheck = heap.find(arg1);
				if(hcheck != heap.end()){
					stack.push_back(hcheck->second);
				}
				else{
					cerr << "Error: Cannot retrieve from " << arg1 << endl;
					return EXIT_FAILURE;
				}
				if(print_debug) cout << " retrieve\t: ";
				break;
			// outchar
			case 121: arg1 = stack.back();
				stack.pop_back();
				cout << (char)arg1;
				if(print_debug) cout << " outchar\t: ";
				break;
			// outnum
			case 122: arg1 = stack.back();
				stack.pop_back();
				cout << arg1;
				if(print_debug) cout << " outnum\t: ";
				break;
			// readchar
			case 123:
				if( stack.empty() ){
					cerr << "Error: Cannot readchar, stack is empty." << endl;
					return EXIT_FAILURE;
				}
				else{
					in_num = cin.get();
					heap[ stack.back() ] = in_num;
					stack.pop_back();
				}
				if(print_debug) cout << " readchar\t: ";
				break;
			// readnum
			case 124:
				if( stack.empty() ){
					cerr << "Error: Cannot readnum, stack is empty." << endl;
					return EXIT_FAILURE;
				}
				else{
					cin >> in_num;
					heap[ stack.back() ] = in_num;
					stack.pop_back();
				}
				if(print_debug) cout << " readnum\t: ";
				break;
		}
		// Debugging mode. r for return stack
		// h for heap values
		// f to exit debugging mode and finish execution
		// e to halt execution
		if(print_debug){
			for(unsigned int i = 0; i < stack.size(); i++){
				cout << "-> " <<  stack[i];
			}
			cout << endl;
			debug_char = getchar();
			while(debug_char == 'r' || debug_char == 'h' || debug_char == 'f' || debug_char == 'e'){
				// print return stack
				if(debug_char == 'r'){
					cout << "Return stack: ";
					for(unsigned int i = 0; i < return_stack.size(); i++){
						cout << "-> " <<  return_stack[i]+1;
					}
					cout << endl;
				}
				if(debug_char == 'h'){
					cout << "Heap: ";
					map<long, long>::iterator it;
					for ( it=heap.begin() ; it != heap.end(); it++ ){
						cout << (*it).first << " => " << (*it).second << ", ";
					}
					cout << endl;
				}
				if(debug_char == 'f'){
					print_debug = false;
				}
				if(debug_char == 'e'){
					return EXIT_SUCCESS;
				}
				debug_char = getchar();
			}
		}
		pcs++;
	}
	// if the 'd' flag was specified then print the heap and stack size like
	// the original ws interpreter
	if(print_detail){
		cout << "Done.\nStack size " << stack.size() << endl;
		cout << "Heap size " << heap.size() << endl;
	}
	return EXIT_SUCCESS;
}

void show_usage(){
	cout << "Usage: wspace [hgdal] file.ws" << endl;
}

void show_help(){
	show_usage();
	// general help
	cout << "General Help:" << endl;
	cout << " a\tprint whitespace assembly language instead of program execution" << endl;
	cout << " l\tprints the code using ABC instead of space, tab, and new line \n\trespectively"
		<< " then exits." << endl;
	cout << " g\tdebugging mode, will step through each command of the program manually" << endl;
	cout << " h\tprint this menu then exit" << endl;
	cout << " d\tprints stack and heap size after program execution\n\tsimilar to original ws interpreter" << endl;
	// debugging mode help
	cout << "Debug Mode:" << endl;
	cout << " Debug mode steps through the code line by line printing the stack after\n command execution" << endl;
	cout << " Entering these letters will have the following effects: " << endl;
	cout << " r\tprints the return stack" << endl;
	cout << " h\tprints the heap key value pairs" << endl;
	cout << " f\tfinishes execution of program" << endl;
	cout << " e\texits the program" << endl;
	cout << " Entering any other values will advance program execution." << endl;
	// Copyright and license information.
	cout << endl;
	cout << "Whitespace C++ interpreter (c) 2008 Pavel Shub. original by Edwin Brady" << endl;
	cout << "This program is published under GNU General Public license." << endl;
}

long convert_num(string wsnum, bool sign){
	long composite(0);
	long pwr(1);
	string::reverse_iterator it;
	for(it = wsnum.rbegin(); it < wsnum.rend()-1; it++){
		if(*it == B){
			composite += pwr;
		}
		pwr *= 2;
	}
	if(sign){
		if(wsnum[0] == B){
			return (composite * -1);
		}
		else{
			return composite;
		}
	}
	if(wsnum[0] == B){
		composite += pwr;
	}
	return composite;
}
