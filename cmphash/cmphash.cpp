/* A program to compare two hash-tables to determine the difference between two filesystems */

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <regex>

using namespace std;

regex hashline("([0-9a-f]*)\\s+(.+)\n");

#define CMP_CODE_SAME 0
#define CMP_CODE_CHANGED 1
#define CMP_CODE_ADDED 2
#define CMP_CODE_REMOVED 3

// An std::string based replacemnt for ifstream::getlne
inline string read_line( ifstream &ifh, char eol = '\n' ) {
	string line;
	char next;
	do {
		ifh.get(next);
		line += next;
	} while ( ifh.good() && next != eol );
	return line;
}

// load a hash-file
inline bool read_file(const string &filepath, map<string, string> &hashmap) {
	// open file for reading
	ifstream ifh(filepath);
	if( ! ifh.good() ) {
		return false; // Failed to open file
	}

	// Read every line in the file
	smatch line;
	do {
		string tmp = read_line(ifh); // std::regex will not parse from temporary strings.
		regex_match(tmp, line, hashline);
		if( line.size() == 3 ) {
			hashmap[line[2]] = line[1];
		}
	} while( ifh.good() );

	return true; // file read
}

// compare two hash files
inline map<string, int> cmp_hash( map<string, string> origin, map<string, string> target ) {
	map<string, int> cmp_data;

	// Check for SAME, CHANGED, and ADDED
	for(auto &hash_pair : target) {
		auto itr = origin.find(hash_pair.first);
		if( itr == origin.end() ) {
			cmp_data[hash_pair.first] = CMP_CODE_ADDED;
		} else if ( origin[hash_pair.first] == hash_pair.second ) {
			cmp_data[hash_pair.first] = CMP_CODE_SAME;
		} else {
			cmp_data[hash_pair.first] = CMP_CODE_CHANGED;
		}
	}

	// Check for REMOVED
	for(auto &hash_pair : origin) {
		auto itr = target.find(hash_pair.first);
		if( itr == target.end() ) {
			cmp_data[hash_pair.first] = CMP_CODE_REMOVED;
		}
	}

	return cmp_data;
}

int main( int argc, char** argv ) {
	map<string, string> origin_hash, target_hash;
	map<string, int> cmp_data;
	bool die = false; // Flag for "something went wrong"; will kill program at specified points instead of immediatly
	
	// Check our arguments
	if( argc != 3 ) {
		cerr << "*************************************************************************************" << endl;
		cerr << " I am a stupid program for comparing two hash files" << endl;
		cerr << " But I am smart enough to know that my invocation was incorrect" << endl;
		cerr << " I must insist that you give me only two arguments: ORIGIN and TARGET, in that order" << endl;
		cerr << " Failure to do this may cause unintented results" << endl;
		cerr << "*************************************************************************************" << endl;
		exit(1);
	}

	// Process files
	if( ! read_file( string( argv[1] ), origin_hash ) ) {
		cerr << "Failed to read file 1 \"ORIGIN\"" << endl;
		die = true;
	}
	if( ! read_file( string( argv[2] ), target_hash ) ) {
		cerr << "Failed to read file 2 \"TARGET\"" << endl;
		die = true;
	}

	if( die ) {
		exit(1);
	}

	// Compare files
	cmp_data = cmp_hash(origin_hash, target_hash);

	// Report
	int all = 0, same = 0, changed = 0, added = 0, removed = 0, undefined = 0;
	for( auto &itr : cmp_data ) {
		switch(itr.second) {
			case CMP_CODE_SAME:
				same++;
				break;
			case CMP_CODE_CHANGED:
				changed++;
				cout << "CHANGED =+ " << itr.first << endl;
				break;
			case CMP_CODE_ADDED:
				added++;
				cout << "ADDED   =+ " << itr.first << endl;
				break;
			case CMP_CODE_REMOVED:
				removed++;
				cout << "REMOVED =+ " << itr.first << endl;
				break;
			default:
				undefined++;
				break;
		}
		all++;
	}
	cout << "----------------------------" << endl;
	cout << "SAME.count    == " << same << endl;
	cout << "CHANGED.count == " << changed << endl;
	cout << "ADDED.count   == " << added << endl;
	cout << "REMOVED.count == " << removed << endl;
	cout << "ALL.count     == " << all << endl;

	return 0;
}

