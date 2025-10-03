#include "../Server.hpp"

static std::vector<std::string> splitByComma(const std::string &input) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string item;
    while (std::getline(ss, item, ',')) {
        size_t start = item.find_first_not_of(" \t");
        size_t end = item.find_last_not_of(" \t");
        if (start != std::string::npos && end != std::string::npos)
            result.push_back(item.substr(start, end - start + 1));
        else if (start != std::string::npos)
            result.push_back(item.substr(start));
        else
            result.push_back("");
    }
    return result;
}

void Server::handlePrivmsg(size_t i, int client_fd, const std::vector<std::string> &tokens, const std::string &msg) {
    Client &client = clients[i - 1];
    const std::string &nickname = client.getNickname();

    if (tokens.size() < 2) {
        sendError(client_fd, 461, nickname, "", "Not enough parameters.");
        return;
    }

    std::string targets = tokens[1];
    std::string message;
    if (tokens.size() > 2 && tokens[2][0] == ':') {
        size_t pos =  msg.find(':');
        if (pos != std::string::npos) {
            message =  msg.substr(pos + 1);
            message = message.substr(0, message.find_last_of("\n"));
        }
    }
    else {
        message = extractMessage(tokens);
        cleanMessage(message);
    }

    if (message.empty() || message.find_first_not_of(" \t\r\n") == std::string::npos) {
        sendError(client_fd, 412, nickname, "", "No text to send");
        return;
    }

    std::vector<std::string> targetList = splitByComma(targets);
    for (size_t idx = 0; idx < targetList.size(); ++idx) {
        std::string &target = targetList[idx];
        if (target.empty()) // skip empty entries
            continue;
        if (isChannelTarget(target)) {
            std::string tar = target;
            tar = toLowerCase(tar);
            std::string &newtarget = tar;
            handleChannelPrivmsg(newtarget, message, client, client_fd);
        } else {
            handleUserPrivmsg(target, message, client, client_fd);
        }
    }
}


std::string Server::extractMessage(const std::vector<std::string> &tokens) {
    std::string msg;
    for (size_t j = 2; j < tokens.size(); ++j) {
        if (j == 2 && tokens[j][0] == ':') {
            msg = tokens[j].substr(1);
        } else {
            msg = tokens[j];
        }
        break; // only first token after target
    }
    return msg;
}

void Server::cleanMessage(std::string &msg) {
    if (!msg.empty()) {
        msg.erase(msg.find_last_not_of("\r\n") + 1);
    }

    if (!msg.empty() && msg[0] == '"') {
        size_t end_quote = msg.find('"', 1);
        if (end_quote != std::string::npos) {
            msg = msg.substr(0, end_quote);
        }
    }
}

bool Server::isChannelTarget(const std::string &target) {
    return (!target.empty() && (target[0] == '#' || target[0] == '&'));
}

void Server::handleChannelPrivmsg(const std::string &target, const std::string &msg, Client &sender, int client_fd) {
    Channel *target_channel = findChannelByName(target);
    if (!target_channel) {
        sendError(client_fd, 403, sender.getNickname(), target, "No such channel");
        return;
    }

    if (!isUserInChannel(target_channel, sender)) {
        sendError(client_fd, 404, sender.getNickname(), target, "Cannot send to channel");
        return;
    }

    sendPrivmsgToChannel(target_channel, msg, sender);
}

void Server::handleUserPrivmsg(const std::string &target, const std::string &msg, Client &sender, int client_fd) {
    for (size_t j = 0; j < clients.size(); ++j) {
        if (clients[j].getNickname() == target) {
            std::string privmsg = ":" + sender.getNickname() + "!~" + sender.getUsername() + "@" + sender.getIp() +" PRIVMSG " + target + " :" + msg + "\r\n";

            send(clients[j].getFd(), privmsg.c_str(), privmsg.length(), 0);
            return;
        }
    }

    sendError(client_fd, 401, sender.getNickname(), target, "No such nick/channel");
}

Channel *Server::findChannelByName(const std::string &name) {
    for (size_t j = 0; j < channels.size(); ++j) {
        if (channels[j].get_name() == name)
            return &channels[j];
    }
    return NULL;
}

bool Server::isUserInChannel(Channel *channel, Client &client) {
    const std::vector<Client> &channel_clients = channel->get_clients();
    for (size_t j = 0; j < channel_clients.size(); ++j) {
        if (channel_clients[j] == client)
            return true;
    }
    return false;
}

void Server::sendPrivmsgToChannel(Channel *channel, const std::string &msg, Client &sender) {
    const std::vector<Client> &channel_clients = channel->get_clients();
    std::string privmsg = ":" + sender.getNickname() + "!" + sender.getUsername() + "@" + sender.getIp() +" PRIVMSG " + channel->get_name() + " :" + msg + "\r\n";

    for (size_t j = 0; j < channel_clients.size(); ++j) {
        if (channel_clients[j].getFd() != sender.getFd()) {
            send(channel_clients[j].getFd(), privmsg.c_str(), privmsg.length(), 0);
        }
    }
}
