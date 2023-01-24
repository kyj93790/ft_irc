#include "Message.hpp"
#include "Server.hpp"
// for debug
#include <iostream>

Message::Message(User *user, const string& msg): _user(user) {
    parse(msg);
    // for debug: print command and params
    cout << "COMMAND: " << _command << endl;
    for (vector<string>::iterator it = _params.begin(); it != _params.end(); ++it) {
        cout <<"PARAMS: " << *it << endl;
    }
}

Message::~Message() {
    _command.clear();
    _params.clear();
}

vector<string> Message::split(const string& str, const char delimeter) {
    vector<string> splited;
    size_t cursorPos = 0;
    size_t delimeterPos;

    while ((delimeterPos = str.find(delimeter, cursorPos)) != string::npos) {
        splited.push_back(str.substr(cursorPos, delimeterPos - cursorPos));
        while (str.at(delimeterPos) == delimeter) {
            if (++delimeterPos == str.length()) return splited;
        }
        cursorPos = delimeterPos;
    }
    splited.push_back(str.substr(cursorPos, string::npos));
    return splited;
}

void Message::parse(const string& msg) {
    vector<string> splitedBySpace = split(msg, ' ');

    for (vector<string>::size_type i = 0; i < splitedBySpace.size(); ++i) {
        if (i == 0) {
            _command = splitedBySpace[i];
            continue ;
        }
        if (splitedBySpace[i][0] == ':') {
            string mergedString;
            splitedBySpace[i].erase(0, 1);
            while (i < splitedBySpace.size()) {
                mergedString += splitedBySpace[i];
                if (i != splitedBySpace.size() - 1) mergedString += ' ';
                ++i;
            }
            _params.push_back(mergedString);
            return ;
        } else _params.push_back(splitedBySpace[i]);
    }
}

void Message::runCommand(Server& server) {
    if (_command == "PRIVMSG") cmdPrivmsg(server);
}

void Message::cmdPrivmsg(Server& server) {
    if (_params.size() != 2) return ;

    vector<string> targetList = split(_params[0], ',');
    for (vector<string>::const_iterator it = targetList.begin(); it != targetList.end(); ++it) {
        string targetName = *it;
        if (targetName[0] == '#') {
            Channel *targetChannel;

            targetChannel = server.findChannelByName(targetName.substr(1, string::npos));
            if (targetChannel == NULL) continue;
            targetChannel->broadcast(_params[1] + '\n', _user->getFd());
        } else {
            User *targetUser;

            targetUser = server.findClientByNickname(targetName);
            if (targetUser == NULL) continue;
            targetUser->addToReplyBuffer(_params[1] + '\n');
        }
    }
}
