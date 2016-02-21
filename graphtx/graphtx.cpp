// Processes a text file (ASCII) to create a word usage diagram using Dot from GraphViz

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

void genHeader(ofstream& ostrm){
	ostrm << "digraph autograph { \n";
}

void genFooter(ofstream& ostrm){
	ostrm << "}\n";
}

int main(int argc, char** argv){
	// process args
	if(argc < 2) {
		cout << "Useage: " << argv[0] << " infile [outfile]\n";
		return 0;
	}
	string infile = argv[1];
	string outfile = (argc > 2) ? argv[2] : "out.dot";

	// Open files
	ifstream istrm;
	ofstream ostrm;
	istrm.open(infile.c_str(), ifstream::in);
	ostrm.open(outfile.c_str(), ofstream::out | ofstream::trunc);

	cout << "Processing " << infile << " to " << outfile << "... "; cout.flush();

	// Process Files
	genHeader(ostrm);
	char in; string lword = "", word = "";
	while(istrm.good()) {
		istrm.get(in);
		if((in >= 'A' && in <= 'Z') || (in >= 'a' && in <= 'z')) {
			word += in;
		} else if(in =='.') {
			ostrm << "\t\"" << lword << "\"->\"" << word << "\";\n";
			lword = word = "";
		} else if( lword == "" && word != "" ) {
			lword = word;
			word = "";
		} else if(lword != "" && word != "") {
			ostrm << "\t\"" << lword << "\"->\"" << word << "\";\n";
			lword = word;
			word = "";
		}
	}
	genFooter(ostrm);

	// cleanup
	istrm.close();
	ostrm.close();

	cout << "Done.\n";
	return 0;

}
