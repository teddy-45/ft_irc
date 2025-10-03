ft_irc

A custom IRC (Internet Relay Chat) server built in C++98, following the RFC 1459
 specification.
This project is part of the 42 curriculum, and its goal is to deepen understanding of network programming, sockets, and asynchronous I/O.

🚀 Features

TCP socket server handling multiple clients simultaneously.

IRC commands implemented according to RFC 1459:

NICK, USER, JOIN, PRIVMSG, KICK, INVITE, TOPIC, MODE, QUIT, etc.

Support for channels with modes:

Invite-only, topic restriction, operators, user limits…

Authentication using password on connection.

Management of operators, channel membership, and user privileges.

Proper handling of errors with numeric reply codes.

Graceful shutdown and cleanup of clients/channels.

📂 Project Structure
ft_irc/
├── src/          # Source files (Server, Client, Channel, Command handling...)
├── includes/     # Header files
├── Makefile      # Compilation rules
└── README.md     # Documentation

⚙️ Installation & Usage
1. Clone the repository
git clone https://github.com/your-username/ft_irc.git
cd ft_irc

2. Compile
make

3. Run the server
./ircserv <port> <password>


<port> → TCP port number to listen on (e.g., 6667).

<password> → Password required for clients to connect.

Example:

./ircserv 6667 secretpass

4. Connect with an IRC client

You can use netcat for testing:

nc localhost 6667


Or a real IRC client like:

HexChat

irssi

WeeChat

📖 Example Session
> nc localhost 6667
PASS secretpass
NICK yassin
USER yassin 0 * :Yassin Example
JOIN #42
PRIVMSG #42 :Hello everyone!

🎮 Bonus – XO Bot

As a bonus feature, an IRC Bot was implemented that allows users to play Tic-Tac-Toe (XO) directly inside the IRC server.

How it works

The bot runs as a separate client that connects automatically to the IRC server.

Users can challenge the bot or another user to play in a channel.

The bot manages the game state, checks for wins/draws, and updates the board in real time.

 X |   |  
-----------
   | O |  
-----------
   |   |  

🛠️ Development Notes

Language: C++98

No external libraries (only system and standard C++ libraries).

Uses poll() (or select/epoll depending on implementation) to handle multiple clients concurrently.

Strict compliance with RFC1459 minimal requirements.

✅ To Do / Possible Improvements

Implement SSL/TLS support.

Add advanced IRC commands (WHOIS, AWAY, MOTD…).

Support services like NickServ/ChanServ.

Enhance error handling and logging.
