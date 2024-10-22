#include <iostream> 
#include <fstream>
#include <string>

using namespace std;

void parseZscore(string input, string base, string base2);

struct reg {
	int id;
	float score;
	string name;
};

int main(int argc, char** argv) 
{
	if (argc != 4) {
		cout << "wrong number of parameters" << std::endl;
		cout << "the correct usage is " << argv[0] << "file.zscore Keccak_reg_basename target_base_name" << std::endl;
		cout << "Example: " << argv[0] << " input.zscore KeccakCore_state_out_ doutb_3" << std::endl;
		return 0;
	}

	cout << "the verifier will also generate a .csv file for easy visualization" << std::endl;

	string input = argv[1];
	string base = argv[2];
	string base2 = argv[3];
	
	parseZscore(input, base, base2);

	return 0; 
}

void parseZscore(string input, string base, string base2) {
	float lowest = 999.9;
	float highest = 0.0;
	string lowest_name = "UNK";
	string highest_name = "UNK";
	float sum = 0.0;
	int count = 0;

	ofstream csv;
	csv.open ("../work/csv.csv");

	csv << "id, z-score, label" << std::endl;

	std::ifstream file(input);
	if (!file) {
		std::cout << "error opening file" << input << std::endl;
		return;
	}
	else {
		cout << "reading " << input << " ..." << endl;
	}

	string part1, part2, part3;
	int hit = 0; 
	int miss = 0;
	int id = 0;

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
			myreg.name = part3;
			id++;

			if (myreg.name.find(base) != std::string::npos) { // meaning that base is a substring of the reg name
				if (myreg.score < lowest) {
					lowest = myreg.score;
					lowest_name = myreg.name;
				}
				if (myreg.score > highest) {
					highest = myreg.score;
					highest_name = myreg.name;
				}
				sum = sum + myreg.score;
				count = count + 1;

				cout << "found " << part3 << ", id of " << part1 << " and score of " << part2;
				hit++;
				cout << " (" << hit << " out of " << (hit+miss) << ")" << endl;
				csv << id << ", " << part2 << ", state" << std::endl;
			}
			else if (myreg.name.find(base2) != std::string::npos) {
				csv << id << ", " << part2 << ", target" << std::endl;
				miss++;
			}
			else {
				csv << id << ", " << part2 << ", other" << std::endl;
				miss++;
			}
		}
	}

	cout << "LOWEST: " << lowest_name << " with a score of " << lowest << endl;	
	cout << "HIGHEST: " << highest_name << " with a score of " << highest << endl;
	cout << "AVERAGE: " << count << " registers with an avg score of " << sum/count << endl;
}
