#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

void parse_and_run_command(const std::string &command) {
    /*
    REQUIREMENTS:
        run simple commands (e.g. /bin/cat foo.txt bar.txt)
        input redirection (e.g. /usr/bin/gcc -E - < somefile.text)
        output redirection (e.g. /usr/bin/gcc -E - > somefile.text)
        pipelines of multiple commands (e.g. /bin/cat foo.txt | /bin/grep bar | /bin/grep baz > output.txt)
        builtin command exit
        outputs the exit status of each command
        prints out error messages to stderr (e.g. via std:cerr)
        does not involve the system's shell
    */
   
    // Parse command line
    // source: https://www.geeksforgeeks.org/split-a-sentence-into-words-in-cpp/
    std::vector<std::string> commands;
    std::string tempWord;
    for (char c : command) {
        // Check if the character is a whitespace character
        if (isspace(c)) {
            // If the temporary string is not empty, add it to the list of words
            if (!tempWord.empty()) {
                commands.push_back(tempWord);
                tempWord.clear();
            }
        } else {
            // If the character is not a whitespace character, add it to the temporary string
            tempWord.push_back(c);
        }
    }
    if (!tempWord.empty()) {
        commands.push_back(tempWord);
    }
    

    // Run commands
    for (std::string token : commands) {
        std::cout << token << std::endl;
        if (token == "exit") {
            exit(0);
        }
        if (token[0] == '/') {
            // file path
        }
        if (token == "<") {
            // input redirection
        }
        if (token == ">") {
            // output redirection
        }
    }
  
    std::cerr << "Not implemented.\n";
}

int main(void) {
    std::string command;
    std::cout << "> ";
    while (std::getline(std::cin, command)) {
        parse_and_run_command(command);
        std::cout << "> ";
    }
    return 0;
}
