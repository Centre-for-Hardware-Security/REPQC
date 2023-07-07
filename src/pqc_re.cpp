#include <iostream> 
#include <fstream>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>
#include <chrono>
#include "params.h"

using namespace std;

bool parseZscore(string input);
bool parseGrouping(string input);
bool parseDepends(string input);

void printGroups();
void printShrinkGroups(float tolerance);
void printGroupHits();
void findStuff(int candidates);
void eliminateGroupsByRegisterPath(int candidates);
void eliminateGroupsByLowNumber();
void eliminateMembersWithoutHits();
void findWinnerGroup();
void findWinnerIndiv();

struct reg {
	int id;
	float score;
	int hits;
};

struct group {
	int id;
	vector<string> members;
	vector<string> deleted_members;
	float avg_score;
	int dominant_frequency;
	float dominant_score;
	int uniques;
	int non_uniques;
	float score;
};

map<string, reg> myregs;
vector<string> myregs_zordered;
vector<group> mygroups;
map<string, vector<string>> fanin; // hash would make this much faster...
map<string, vector<string>> fanout; // hash would make this much faster...

// global for simplicity
int min_group_size;

int main(int argc, char** argv) { 
	if (argc != 6) {
		cout << "wrong number of parameters" << std::endl;
		cout << "the correct usage is " << argv[0] << " file.zscore file.grouping file.depends min_group_size n_candidates" << std::endl;
		cout << "Example: " << argv[0] << " ../results/input.zscore ../results/input.grouping ../results/input.depends 64 300" << std::endl;
		return 0;
	}

	auto start = chrono::high_resolution_clock::now();

	string zscore_file = argv[1];
	string grouping_file = argv[2];
	string depends_file = argv[3];
	min_group_size = atoi(argv[4]);
	float tolerance = 0.0; // kept here as a reminder that some old code expects this as input
	int candidates = atoi(argv[5]);
	
	bool ret;
	ret = parseZscore(zscore_file);
	if (!ret) return 0;

	ret = parseGrouping(grouping_file);	
	if (!ret) return 0;

	ret = parseDepends(depends_file);
	if (!ret) return 0;

	// this is strategy 1, it works if the groups can be divided very close to the word size of 64bits and the entire 64-bit register lies in the same group
	// this strategy fails if the groups become fractioned, which  happens quite easily. we have abandoned this strategy. 
	//printShrinkGroups(tolerance);
	//findStuff(candidates);
	//printGroupHits();

	// this is strategy 2, it works by eliminating groups based on z-score ordering. it works well if the keccak state registers are either at the top of the list
	// or they look very different from other regs
	#ifdef STRAT_G
		printGroups();
		eliminateGroupsByRegisterPath(candidates);
		eliminateGroupsByLowNumber();
		eliminateMembersWithoutHits();
		findWinnerGroup();
	#endif

	// this is strat 3, works for masked
	#ifdef STRAT_I
		printGroups();
		eliminateGroupsByRegisterPath(candidates);
		eliminateMembersWithoutHits();
		findWinnerIndiv();
	#endif

	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
 
	cout << "re_pqc took " << (float)duration.count()/1000000 << " seconds" << endl;
	return 0; 
}

bool parseZscore(string input) {
	std::ifstream file(input);
	if (!file) {
		std::cout << "error opening file" << input << std::endl;
		return false;
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
			myreg.hits = 0;
			myregs[part3] = myreg;

			myregs_zordered.push_back(part3); // to keep they ordered by z score!
		}
	}
	cout << "done parsing scores for a total of " << myregs.size() << " regs" << endl;
	return true;
}

bool parseGrouping(string input) {
	std::ifstream file(input);
	if (!file) {
		std::cout << "error opening file" << input << std::endl;
		return false;
	}
	else {
		cout << "reading " << input << " ..." << endl;
	}

	int count = 0;
	string word;
	group temp;

	temp.avg_score = 0.0;
	temp.dominant_frequency = 0;
	temp.dominant_score = 0.0;
	temp.uniques = 0;
	temp.non_uniques = 0;

	while(getline(file,word)) {
		if (word == "") { // this is an empty line, marks the end of a group
			if (temp.members.size() >= min_group_size) {
				temp.id = count;
				mygroups.push_back(temp);
			}
			temp.members.clear();
			count++;
		} else {
			temp.members.push_back(word);
		}
	}
	cout << "done parsing groups, considered " << mygroups.size() << " groups out of " << count << endl;
	return true;
}

bool parseDepends(string input) {
	std::ifstream file(input);
	if (!file) {
		std::cout << "error opening file" << input << std::endl;
		return false;
	}
	else {
		cout << "reading " << input << " ..." << endl;
	}

	string part1, part2, part3;
	int count = 0;

	while(file) {
		file >> part1; // source
		file >> part2; // arrow symbol
		file >> part3; // destination
		count++;

		if (fanin.find(part3) == fanin.end()) {// meaning the map does not contain an entry for this reg yet
			vector<string> temp_fanin;
			temp_fanin.push_back(part1);
			fanin[part3] = temp_fanin;
		}
		else {
			fanin[part3].push_back(part1);
		}

		if (fanout.find(part1) == fanin.end()) {// meaning the map does not contain an entry for this reg yet
			vector<string> temp_fanout;
			temp_fanout.push_back(part3);
			fanout[part1] = temp_fanout;
		}
		else {
			fanout[part1].push_back(part3);
		}
	}
	cout << "done parsing depends file for a total of " << count << " dependencies" << endl;
	return true;
}


void printGroups() {
	ofstream logfile;
	logfile.open ("../work/groups.log");
	logfile << "generated by re_pqc" << endl;
	logfile << "src code available from https://github.com/Centre-for-Hardware-Security/re_pqc" << endl;
	logfile << "the structure is as follows:" << endl;
	logfile << "groups are separated by  ------------- group ID ------------- lines" << endl;
	logfile << "members of a group are listed by: name z-score" << endl; 

	for (auto& it1 : mygroups) {
		cout << "------------- group " << it1.id << " -------------" << endl;
		logfile << "------------- group " << it1.id << " -------------" << endl;

		vector<float> dist;

		for (auto& it2 : it1.members) {
			logfile << it2 << " " << myregs[it2].score << endl;
			dist.push_back(myregs[it2].score);
		}
		// I am looking for groups of AT LEAST 64 members, no worries with div by 0
		double sum = std::accumulate(std::begin(dist), std::end(dist), 0.0);
		double m =  sum / dist.size();
		
		double accum = 0.0;
		std::for_each (std::begin(dist), std::end(dist), [&](const double d) {
		    accum += (d - m) * (d - m);
		});
		
		double stdev = sqrt(accum / (dist.size()-1));
	
		cout << "STATS: group " << it1.id << " has " << it1.members.size() << " members" << endl;
		logfile << "STATS: group " << it1.id << " has " << it1.members.size() << " members" << endl;
		cout << "STATS: group " << it1.id << " has an avg score of " << m << " with a std_dev of " << stdev << endl;
		logfile << "STATS: group " << it1.id << " has an avg score of " << m << " with a std_dev of " << stdev << endl;

	}
	logfile.close();
}

void eliminateMembersWithoutHits() {
	ofstream logfile;
	logfile.open ("../work/groups.log",  std::ios_base::app);

	for (auto& it1 : mygroups) {
		cout << "------------- group " << it1.id << " -------------" << endl;
		logfile << "------------- group " << it1.id << " -------------" << endl;
		cout << "STATS: group " << it1.id << " had " << it1.members.size() << " members" << endl;
		logfile << "STATS: group " << it1.id << " had " << it1.members.size() << " members" << endl;
	
		for (auto it2 = it1.members.begin(); it2 != it1.members.end(); ) {
			if (myregs[*it2].hits == 0) { // was never hit!
				it2 = it1.members.erase(it2);
			}
			else {
				it2++;
			}
		} 

		cout << "STATS: group " << it1.id << " has " << it1.members.size() << " members" << endl;
		logfile << "STATS: group " << it1.id << " has " << it1.members.size() << " members" << endl;
	}
	logfile.close();
}

void printShrinkGroups(float tolerance) {
	ofstream logfile;
	logfile.open ("../work/groups.log");
	logfile << "generated by pqc_re. src code available from https://github.com/Centre-for-Hardware-Security/re_pqc \n";
	logfile << "the structure is as follows:" << endl;
	logfile << "groups are separated by -------- group ID -------- lines" << endl;
	logfile << "members of a group are listed by name z-score frequency" << endl; 

	for (auto& it1 : mygroups) {
		cout << "------------- group " << it1.id << " -------------" << endl;
		logfile << "------------- group " << it1.id << " -------------" << endl;
		
		map<float, int> table;

		for (auto& it2 : it1.members) {
			logfile << it2 << " " << myregs[it2].score << endl;
		
			float reference = myregs[it2].score;
			
			if (table.find(reference) == table.end()) {// meaning the table does not contain an occurence for reference
				table[reference] = 0;
			
				for (auto& it3 : it1.members) { // 
					float candidate = myregs[it3].score;
					if (candidate == reference) {
						table[reference]++;
					}
				}
			}
		}

		int freq = 0;
		float score = 0.0;
		for(const auto& elem : table) {
			logfile << "table: " << elem.first << " " << elem.second << "\n"; // reg name | z-score | frequency of the z-score
			if (elem.second > freq) { 
				freq = elem.second;
				score = elem.first;
			}
		}

		it1.dominant_frequency = freq;
		it1.dominant_score = score;		

		cout << "STATS: group " << it1.id << " has " << it1.members.size() << " members" << endl;
		logfile << "STATS: group " << it1.id << " has " << it1.members.size() << " members" << endl;
		logfile << "STATS: group " << it1.id << " has a dominant score of " << it1.dominant_score << " with a frequency of " << it1.dominant_frequency << endl;

		if (tolerance == 0.0) {
			continue;
		}
		else {
			for (auto it2 = it1.members.begin(); it2 != it1.members.end(); ) {
				float upper_limit = it1.dominant_score * (1 + tolerance);
				float lower_limit = it1.dominant_score * (1 - tolerance);
				float score = myregs[*it2].score;

				if ((score >= upper_limit) || (score <= lower_limit)) { // group member must be deleted
					it1.deleted_members.push_back(*it2);
					it2 = it1.members.erase(it2);
				}
				else {
					it2++;
				}
			}
			logfile << "STATS: group " << it1.id << " has " << it1.deleted_members.size() << " deleted members" << endl;
			logfile << "STATS: group " << it1.id << " has " << it1.members.size() << " current members" << endl;		
		}		

	}
	logfile.close();
}

void findStuff(int candidates) {
	int regcount = 0;

	for (auto& it1 : myregs_zordered) {
		vector <string> deps;
		deps = fanin[it1];
		
		//cout << "considering " << it1 << " as target. it has " << deps.size() << " dependencies" << endl;

		for (auto& it2 : mygroups) {
			int hits = 0;
			int misses = 0;
			for (auto& it3 : it2.members) {
				// check if member it3 of group it2 has a path to reg it1
				//cout << "checking if " << it3 << " has a path to " << it1 << endl;
				
				if ( find(deps.begin(), deps.end(), it3) != deps.end() ) {
					hits++;
					myregs[it3].hits++; // is this enough? maybe i need to iterate over all
				}
				else {
					misses++;
				}
			}

			//cout << "group " << it2.id << " finished with " << hits << " hits and " << misses << " misses" << endl;
			if (hits==1) {
				it2.uniques++;
			}
			else {
				it2.non_uniques++;
			}
		}
		regcount++;

		if (regcount == candidates) {break;}; // reached the number of candidates regs to be considered
	}
}

void eliminateGroupsByRegisterPath(int candidates) {
	int regcount = 0;
	int considered = 0;

	ofstream logfile;
	logfile.open ("../work/candidates.log");

	for (auto& it1 : myregs_zordered) {
		vector<int> to_delete;
		vector <string> deps;

		if (regcount == candidates) { // reached the number of candidates regs to be considered
			break;
		}

		deps = fanin[it1];  
		
		cout << "analyzing " << it1 << " as target. fanin " << deps.size()  << ", fanout " << fanout[it1].size() << ", z-score  " << myregs[it1].score << ". ";

		//if (myregs[it1].score >= 1.0) break; // TODO: I am not sure this always works well, but it would eliminate any reg that is obviously control oriented
		// for now it is not being used. it would speed up the execution time...

		if (deps.size() < FANIN_FLOOR) {
			cout << "dropped!" << endl;
			regcount++;
			continue;
		}; // this could be much smarter, actually 64 is a soft number.

		if (deps.size() > FANIN_CEILING) {
			cout << "dropped!" << endl;
			regcount++;
			continue;
		}; // this could be much smarter

		deps = fanout[it1];
		if (deps.size() < FANOUT_FLOOR) {
			cout << "dropped!" << endl;
			regcount++;
			continue;
		}; // this could be much smarter, actually 64 is a soft number.

		if (deps.size() > FANOUT_CEILING) {
			cout << "dropped!" << endl;
			regcount++;
			continue;
		}; // this could be much smarter

		deps = fanin[it1];

		cout << endl;

		logfile << "analyzing " << it1 << " as target. fanin " << deps.size()  << ", fanout " << fanout[it1].size() << ", z-score  " << myregs[it1].score << endl;
	
		cout << "-------------------------------------------------------------- " << endl;

		bool all_failed = true;
		for (auto it2 = mygroups.begin(); it2 != mygroups.end(); it2++) {
			int hits = 0;
			int misses = 0;
			for (auto it3 = it2->members.begin(); it3 != it2->members.end(); ) {
				// check if member it3 of group it2 has a path to reg it1
				//cout << "checking if " << it3 << " has a path to " << it1 << endl;

				if (it1 == *it3) { // checking if there is a path from reg_123 to reg_123 is useless?
					it3 = it2->members.erase(it3);
					//hits = 0;
					continue;
				}

				if ( find(deps.begin(), deps.end(), *it3) != deps.end() ) {
					hits++;
					myregs[*it3].hits++;
				}
				else {
					misses++;
				}
				it3++;
			}

			if (hits==0) {
				#ifdef PRINT_DETAILS
					cout << "group " << it2->id << " H/M " << hits << "/" << misses;
					cout << " (deletion)" << endl;
				#endif
				//to_delete.push_back(it2->id);		// you cannot enable this if the target registers are spread in different groups
			}
			else { // probably not needed anymore
				#ifdef PRINT_DETAILS
					cout << "group " << it2->id << " H/M " << hits << "/" << misses;
					cout << endl;
				#endif

				all_failed = false;
				if (hits==1) {
					it2->uniques++;
				}
				else {
					it2->non_uniques++;
				}
			}			
		}
		regcount++;

		if (all_failed) {
			continue;
		}
		else {
			considered++;
			if ( (mygroups.size() - to_delete.size()) > 0 ) { // meaning I can delete and there will still be something left
				for (auto it3 : to_delete) {
					for (auto it2 = mygroups.begin(); it2 != mygroups.end(); ) {
						if (it2->id == it3) {
							it2 = mygroups.erase(it2);
						}
						else { it2++;}
					}
				}
			}
			else {
				cout << "ALGORITHM ended, will stop trying new regs!" << endl;
				break;
			}
		}
	}

	cout << "-------------------------------------------------------------- " << endl;
	cout << "a total of " << regcount << " candidates were analyzed"; 
	cout << " but only " << considered << " were considered" << endl;

	logfile.close();

	return;
}

void eliminateGroupsByLowNumber() {
	ofstream logfile;
	logfile.open ("../work/groups.log",  ios_base::app);

	int highest_score = 0;
	cout << "----------------------------------------" << endl;
	logfile << "----------------------------------------" << endl;
	cout << "considering LOW REG COUNT! any group that survived without at least " << min_group_size << " members in it will be deleted" << endl;
	logfile << "considering LOW REG COUNT! any group that survived without at least " << min_group_size << " members in it will be deleted" << endl;

	for (auto it1 = mygroups.begin(); it1 != mygroups.end(); ) {
		int hits = 0;
		for (auto& it2 : it1->members) {
			if (myregs[it2].hits != 0) { 
				hits++;
			}
		}

		if (hits < min_group_size) {
			cout << "group " << it1->id << " will be eliminated" << endl;
			logfile << "group " << it1->id << " will be eliminated" << endl;
			it1 = mygroups.erase(it1);
		}
		else {
			it1++;
		}
	}	

	logfile.close();
}


void printGroupHits() {
	ofstream logfile;
	logfile.open ("../work/result.log");

	int highest_score = 0;

	for (auto& it1 : mygroups) {
		int hits = 0;
		for (auto& it2 : it1.members) {
			if (myregs[it2].hits != 0) { 
				hits++;
				cout << "group " << it1.id << ", reg " << it2 << ", score " << myregs[it2].score << ", hits " << myregs[it2].hits << endl;
			}
		}

		float score = (hits*it1.uniques) - (hits*it1.non_uniques);
		it1.score = score;

		if (score > highest_score) {
			highest_score = score;
		}
		
		cout << "group " << it1.id << " has a total of " << hits << "/" << it1.members.size() << " hit regs/total regs and ";
		cout << it1.uniques << "/" << it1.non_uniques << " uniques/non_uniques and its score is: " << score << endl;
	}
	return;
	for (auto& it1 : mygroups) {
		if (it1.score==highest_score) {
			cout << "the winner with " << highest_score << " score is group " << it1.id << endl;
			logfile << "the winner with " << highest_score << " score is group " << it1.id << endl;
			cout << "its \"hit\" members are: " << endl;
			logfile << "its \"hit\" members are: " << endl;
			for (auto& it2 : it1.members) {
				if (myregs[it2].hits != 0) { 
					cout << it2 << " " << myregs[it2].score << endl;
					logfile << it2 << " " << myregs[it2].score << endl;
				}
			}			

			cout << "its \"non-hit\" members are: " << endl;
			for (auto& it2 : it1.members) {
				if (myregs[it2].hits == 0) { 
					logfile << it2 << " " << myregs[it2].score << " " << myregs[it2].hits << endl;
				}
			}
		}
	}

	logfile.close();
}


void findWinnerGroup() {
	ofstream logfile;
	logfile.open ("../work/result.log");

	if (mygroups.size() == 0) { // no group survived, I will just put some note on the log
		logfile << "algorithm did not find a group :( " << endl;
		cout << "algorithm did not find a group :( " << endl;
		logfile.close();
		return;
	}

	float lowest_sum = std::numeric_limits<float>::max(); // highest possible float
	float lowest_g = std::numeric_limits<float>::max(); // highest group score
	int g_id;

	for (auto& it1 : mygroups) {
		vector<float> scores;
		float sum = 0.0;

		for (auto& it2 : it1.members) {
			scores.push_back(myregs[it2].score);
		}

		sort(scores.begin(), scores.end());
	
		int count = 0;
		for (auto& score : scores) {    
			sum = sum + score;
			count++;
			if (count == 64) break;
		}

		if (sum < lowest_g) { 
			lowest_g = sum;
			g_id = it1.id;
		}
	
		cout << "group " << it1.id << " has a score of " << sum << endl;
	}

	cout << "overall lowest score is from group " << g_id << endl;

	for (auto& it1 : mygroups) {
		if (it1.id == g_id) {
			multimap<float, string> scores;
	
			for (auto& it2 : it1.members) {
				scores.insert(pair<float,string>(myregs[it2].score, it2));
				//scores[myregs[it2].score] = it2;
			}
	
			int count = 1;
			for (auto& score : scores) {    
				if (count < 10) {
					cout << "candidate " << count << ": " << score.first << " " << score.second << " " << myregs[score.second].hits <<  " " << fanin[score.second].size() << " " << fanout[score.second].size() << endl;
				}
				else if (count <= 64) {
					cout << "candidate " << count << ": " << score.first << " " << score.second << " " << myregs[score.second].hits <<  " " << fanin[score.second].size() << " " << fanout[score.second].size() << endl;
				}

				logfile << count << ": " << score.first << " " << score.second << " " << myregs[score.second].hits << endl;
				
				count++;
			}
		}
	
	}

	for (auto& it1 : mygroups) {
		if (it1.id == g_id) {
			map<float, int> table;
			
			for (auto& it2 : it1.members) {
				float reference = myregs[it2].score;
			
				if (table.find(reference) == table.end()) {// meaning the table does not contain an occurence for reference
					table[reference] = 0;
			
					for (auto& it3 : it1.members) { // 
						float candidate = myregs[it3].score;
						if (candidate == reference) {
							table[reference]++;
						}
					}
				}
			}
	

			int freq = 0;
			float score = 0.0;
			for(const auto& elem : table) {
				//cout << "table: " << elem.first << " " << elem.second << "\n"; // reg name | z-score | frequency of the z-score
				if (elem.second > freq) { 
					freq = elem.second;
					score = elem.first;
				}
			}
		}
	}

	logfile.close();
}

void findWinnerIndiv() {
	ofstream logfile;
	logfile.open ("../work/result.log");

	if (mygroups.size() == 0) { // no group survived, I will just put some note on the log
		logfile << "algorithm did not find a group :( " << endl;
		cout << "algorithm did not find a group :( " << endl;
		logfile.close();
		return;
	}

	float lowest_sum = std::numeric_limits<float>::max(); // highest possible float
	float lowest_g = std::numeric_limits<float>::max(); // highest group score
	int g_id;
	int survivors = 0;
	multimap<float, string> scores;

	for (auto& it1 : mygroups) {
		float sum = 0.0;

		for (auto& it2 : it1.members) {
			if (myregs[it2].hits > 0) {
				logfile << "reg " << it2 << " " << myregs[it2].score << " " << myregs[it2].hits << endl;
				scores.insert(pair<float,string>(myregs[it2].score, it2));
				survivors++;
			}
		}
	}

	for (auto& score : scores) {    
		if (myregs[score.second].hits <= THRESHOLD) {
			cout << "candidate " << ": " << score.first << " " << score.second << " " << myregs[score.second].hits <<  " " << fanin[score.second].size() << " " << fanout[score.second].size() << endl;
		}
	}

	cout << "total survivors: " << survivors << endl;
	logfile.close();
}




