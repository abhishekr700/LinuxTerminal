#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <vector>
#include <fstream>


using namespace std;

bool exitflag = false;
vector<string> path;


vector<string> splitlinebychar(string s, char ch) {
    vector<string> str;
    string temp;
    for (int i = 0; i <= s.length(); ++i) {
        if (s[i] == ch || s[i] == '\0') {
            str.push_back(temp);
            temp.clear();
            continue;
        }
        temp += s[i];
    }
    return str;
}

string findLocation(string command) {
    ifstream f;
    for (int i = 0; i < path.size(); ++i) {
        f.open(path[i] + '/' + command);
        if (f) {
            return path[i] + '/' + command;
        }
    }
    return "_000_";
}

void processPath() {
    string pathval = getenv("PATH");
//    cout<<pathval<<endl;
    path = splitlinebychar(pathval, ':');
//    for (int i = 0; i < path.size(); ++i) {
//        cout<<path[i]<<endl;
//    }
}

void processCommand(string s) {
    if (s == "exit") {
        exitflag = true;
        return;
    }
//    cout<<"Forking";

    int pipes[2];
    if (pipe(pipes) == -1) {
        cout << "Error creating pipe!";
        exit(0);
    }
    pid_t p = fork();

    if (p == 0) {
        dup2(pipes[1], STDOUT_FILENO);
        close(pipes[0]);
        close(pipes[1]);
//        cout<<"forked output\n";
        vector<string> cmds = splitlinebychar(s, ' ');
        string location = findLocation(cmds[0]);
        if (location == "_000_") {
            cout << "Command not found !\n";
        } else
            execl(location.c_str(), cmds[0].c_str(), (char *) NULL);
        exit(0);
    } else if (p == -1) {
        cout << "error";
        exit(0);
    } else {
        close(pipes[1]);
        FILE *input = fdopen(pipes[0], "r");
        if (input == NULL) perror("Error opening file");
        char buf[100];
        while (!feof(input)) {
            if (fgets(buf, 100, input) == NULL) break;
            fputs(buf, stdout);
        }
        fclose(input);
        wait(NULL);
    }
}

int main() {
    processPath();
    cout << "Linux Shell 1.0\n";
    string cmd;
    while (!exitflag) {
        cout << "myShell>";
        getline(cin, cmd);
//        cmd="";
//        cout<<"Got cmd:"<<cmd<<endl;
        processCommand(cmd);
    }
}