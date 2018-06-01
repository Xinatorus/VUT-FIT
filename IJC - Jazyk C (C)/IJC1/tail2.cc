/**
	IJC DU2

	xsztef02
	Patrik Sztefek
**/
#include <iostream>
#include <string>
#include <fstream>
#include <queue>

using namespace std;

int main(int argc, char *argv[]){
	int lines=10;
	string buff;
	queue<string> buffArray;
	ifstream f;
	istream* file = &std::cin;

	ios::sync_with_stdio(false);

	cout << "start" << endl;

	if (argc == 1){
		//none
	}else if (argc == 2){ //name file?
		f.open(argv[1]);
		if (f.is_open()){ // otevreni souboru
			file = &f;
		}
		else{
			cerr << "File error!\n";
			return 1;
		}
	}
	else if (argc == 3){ // -n num ??
		if (argv[1] == string("-n")){
			lines = stoi(argv[2]);
		}
		else{
			cerr << "Arg error 0!\nUSE:n\n  tail -n NUM_OF_LINES \n  tail FILENAME\n";
			return 1;
		}
	}

	while (getline(*file, buff)){
		buffArray.push(buff);
		if (buffArray.size() > lines) buffArray.pop();
	}
	
	while (buffArray.size()){
		cout << buffArray.front() << endl;
		buffArray.pop();
	}


	f.close();
	cin.get();
	return 0;
}