#include <iostream> 
#include <fstream>
#include <string>
#include <map>
#include <vector>

using namespace std;

void parseZscore(string input);
void parseGrouping(string input, int min_group_size);

struct reg {
	int id;
	float score;
};

map<string, reg> myregs;
vector<vector<string>> mygroups;

int main(int argc, char** argv) 
{ 
	if (argc != 4) {
		cout << "wrong number of parameters" << std::endl;
		cout << "the correct usage is " << argv[0] << "file.zscore file.grouping min_group_size" << std::endl;
		cout << "Example: " << argv[0] << " ../results/input.zscore ../results/input.grouping 64" << std::endl;
		return 0;
	}

	string zscore_file = argv[1];
	string grouping_file = argv[2];
	int min_group_size = atoi(argv[3]);
	
	parseZscore(zscore_file);
	parseGrouping(grouping_file, min_group_size);

	return 0; 
}

void parseZscore(string input) {
	std::ifstream file(input);
	if (!file) {
		std::cout << "error opening file" << input << std::endl;
		return;
	}
	else {
		cout << "reading " << input << " ..." << endl;
	}

	string part1, part2, part3;

	while(file) {
		file >> part1;
		file >> part2;
		file >> part3;

		if (part2 == "Z-score") {
			// this is the first line, the header, we will ignore it
			file >> part1;
			continue;
		}
		else {
			reg myreg;
			myreg.id = stoi(part1);
			myreg.score = stof(part2);
			myregs[part3] = myreg;
		}
	}
	cout << "done parsing scores for a total of " << myregs.size() << " regs" << endl;
}

void parseGrouping(string input, int min_group_size) {
	std::ifstream file(input);
	if (!file) {
		std::cout << "error opening file" << input << std::endl;
		return;
	}
	else {
		cout << "reading " << input << " ..." << endl;
	}

	string word;
	vector<string> temp;
	int count = 0;

	while(getline(file,word)) {
		if (word == "") { // this is an empty line, marks the end of a group
			if (temp.size() >= min_group_size) {
				mygroups.push_back(temp);
			}
			temp.clear();
			count++;
		} else {
			temp.push_back(word);
		}
	}
	cout << "done parsing groups, considered " << mygroups.size() << " groups out of " << count << endl;
}
