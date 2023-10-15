#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

class Automata {
    int initialState;
    std::vector<int> finalStates;

    std::unordered_map<int, bool> allStates;
    std::unordered_map<int, bool> reachedStates;
    std::unordered_map<int, bool> deadEndStates;
    std::unordered_map<int, std::vector<int> > transitionsMap;

public:
    Automata(std::vector<std::string> fileLines) {
        initialState = std::stoi(fileLines[2]);
        std::string finalStatesLine = fileLines[3].substr(2);

        size_t start = 0, end = 0;

        while ((end = finalStatesLine.find(' ', start)) != std::string::npos) {
            std::string finalState = finalStatesLine.substr(start, end - start);
            finalStates.push_back(std::stoi(finalState));
            start = end + 1;
        }

        std::vector<std::string> transitionRecords(fileLines.begin() + 4, fileLines.end());

        createTransitionsMap(transitionRecords);
    }

    void createTransitionsMap(const std::vector<std::string>& transitionRecords) {
        for (const std::string& record : transitionRecords) {
            size_t spacePos = record.find(' ');
            int from = std::stoi(record.substr(0, spacePos));
            int to = std::stoi(record.substr(spacePos + 3));
          
            transitionsMap[from].push_back(to);
          
            allStates[from] = true;
            allStates[to] = true;
        }
    }

    void scanTransitionsFromState(const int& state) {
        reachedStates[state] = true;

        if (transitionsMap.find(state) == transitionsMap.end() && 
            std::find(finalStates.begin(), finalStates.end(), state) == finalStates.end()) {
            deadEndStates[state] = true;
            return;
        }

        for (const int& nextState : transitionsMap[state]) {
            scanTransitionsFromState(nextState);
        }
    }

    std::vector<int> getUnreachedStatesArray() {
        std::vector<int> unreachedStates;
        for (const auto& state : allStates) {
            if (!reachedStates[state.first]) {
                unreachedStates.push_back(state.first);
            }
        }
        std::sort(unreachedStates.begin(), unreachedStates.end());
        return unreachedStates;
    }

    bool findFinishFromState(const int& state) {
        if (deadEndStates.find(state) != deadEndStates.end()) {
            return false;
        }
        
        if (std::find(finalStates.begin(), finalStates.end(), state) != finalStates.end()) {
            return true;
        }

        if (transitionsMap.find(state) == transitionsMap.end()) {
            deadEndStates[state] = true;
            return false;
        }

        for (const int& nextState : transitionsMap[state]) {
            if (findFinishFromState(nextState)) {
                return true;
            }
        }

        deadEndStates[state] = true;
        return false;
    }

    void getDeadEndStatesFromUnreachable(const std::vector<int>& unreachableStates) {
        for (const int& state : unreachableStates) {
            findFinishFromState(state);
        }
    }

    std::vector<int> getDeadEndStates() {
        std::vector<int> deadEndStatesVec;
        for (const auto& state : deadEndStates) {
            deadEndStatesVec.push_back(state.first);
        }
        std::sort(deadEndStatesVec.begin(), deadEndStatesVec.end());
        return deadEndStatesVec;
    }

    void scanAutomata() {
        try {
            scanTransitionsFromState(initialState);

            std::vector<int> unreachableStates = getUnreachedStatesArray();

            getDeadEndStatesFromUnreachable(unreachableStates);

            std::vector<int> deadEndStatesVec = getDeadEndStates();

            std::cout << "Unreachable states:";
            for (const int& state : unreachableStates) {
                std::cout << " " << state;
            }
            std::cout << std::endl;

            std::cout << "Dead end states:";
            for (const int& state : deadEndStatesVec) {
                std::cout << " " << state;
            }
            std::cout << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error scanning automata" << std::endl;
        }
    }
};

int main() {
    std::ifstream file("file.txt");

    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    std::vector<std::string> fileLines;
    std::string line;

    while (std::getline(file, line)) {
        fileLines.push_back(line);
    }

    file.close();

    Automata automata(fileLines);
    automata.scanAutomata();

    return 0;
}
