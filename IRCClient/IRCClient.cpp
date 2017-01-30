// IRCClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

// Handling socket reading/writing
int sendMessage(SOCKET& connectedSocket, string& message);
int readMessage(SOCKET& connectedSocket);

// Handling recieved/sent message parsing
void parseMessage(string& recievedMessage, string& tempMessage);
void parseInput(string& command);

SOCKET connectSocket = INVALID_SOCKET;
string currentChannel = "";
bool isExiting = false;

int main(int argc, char* argv[])
{
  // Some defaults if we aren't given anything
  string server = "chat.freenode.net";
  string port   = "8000";
  string pass   = "none";
  string nick   = "kb";
  string user   = "irctest 8 * :irctest";
  if( argc > 1 )
  {
    server = string(argv[1]);
    port   = string(argv[2]);
    pass   = string(argv[3]);
    nick   = string(argv[4]);
    user   = string(argv[5]);
  }
  
  // Initialize Winsock.

  WSAData wsaData;

  int ret = 0;
  ret = WSAStartup(MAKEWORD(2,2), &wsaData);
  if( ret != 0 )
  {
    printf("WSAStartup Failed: %d\n", ret);
    return 1;
  }

  // Create a socket.

  struct addrinfo *result = nullptr,
                  *ptr    = nullptr,
                   hints;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  ret = getaddrinfo(server.c_str(), port.c_str(), &hints, &result);
  if( ret != 0 )
  {
    printf("getaddrinfo failed: %d\n", ret);
    WSACleanup();
    return 1;
  }

  //SOCKET connectSocket = INVALID_SOCKET;

  ptr = result;

  for( ; ptr != nullptr; ptr = ptr->ai_next )
  {
    connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if( connectSocket == INVALID_SOCKET )
    {
      printf("Error at socket(): %ld\n", WSAGetLastError());
      freeaddrinfo(result);
      WSACleanup();
      return 1;
    }
    
    // Connect to the server.
    ret = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if( ret == SOCKET_ERROR )
    {
      closesocket(connectSocket);
      connectSocket = INVALID_SOCKET;
      continue;
    }
    break;
  }

  freeaddrinfo(result);

  if( connectSocket == INVALID_SOCKET )
  {
    printf("Unable to connect to server: %d\n");
    WSACleanup();
    return 1;
  }

  // Create a thread to handle reading and printing recieved messages
  thread readThread(readMessage, connectSocket);
 
  // Log in
  string sendBuf;
  
  // Send Pass
  sendBuf = "PASS " + pass;
  ret = sendMessage(connectSocket, sendBuf);

  // Send Nick
  sendBuf = "NICK " + nick;
  ret = sendMessage(connectSocket, sendBuf);

  // Send User
  sendBuf = "USER " + user;
  ret = sendMessage(connectSocket, sendBuf);

  // Main loop
  string cmd;
  while( !isExiting )
  {
    getline(cin, cmd);
    parseInput(cmd);
  }

  printf("%s", "Disconnecting...\n");
  
  // Disconnect.
  ret = shutdown(connectSocket, SD_SEND);
  readThread.join();
  if( ret == SOCKET_ERROR )
  {
    printf("Shutdown failed: %d\n", WSAGetLastError());
    closesocket(connectSocket);
    WSACleanup();
    return 1;
  }

  printf("%s", "Disconnected.\n");
  closesocket(connectSocket);
  WSACleanup();

	return 0;
}

int readMessage(SOCKET& connectedSocket)
{
  const int buf = 512;
  int ret;

  string tempMessage;
  char recvBuf[buf];
  memset(recvBuf, 0, buf);

  do
  {
    ret = recv(connectedSocket, recvBuf, buf, 0);
    //printf("%d", ret);
    if( ret < 1 )
    {
      printf("recv failed: %d\n", WSAGetLastError());
      isExiting = true;
    }
    else
    {
      //printf("Bytes received: %d\n", ret);
      //printf("%s", recvBuf);
      
      // Parse message here
      parseMessage(string(recvBuf), tempMessage);
    }
  } while( !isExiting );

  return ret;
}

void parseMessage(string& recievedMessage, string& tempMessage)
{
  // do some parsing stuff (e.g. "PING PONG"/"PRIVMSG"/etc)
  int endIndex = recievedMessage.find("\r\n");
  while( endIndex != -1 )
  {
    tempMessage += recievedMessage.substr(0, endIndex) + "\r\n";
    //messages.push(tempMessage);
    cout.write(tempMessage.c_str(), tempMessage.length());
    
    recievedMessage = recievedMessage.substr(endIndex + 2, recievedMessage.length());
    tempMessage.clear();
    endIndex = recievedMessage.find("\r\n");
  }

  // If we have an uncompleted message, start building it up
  if( recievedMessage.length() > 0 )
  {
    tempMessage += recievedMessage;
  }
}

int sendMessage(SOCKET& connectedSocket, string& message)
{
  message += "\r\n";
  int ret = send(connectedSocket, message.c_str(), (int)strlen(message.c_str()), 0);
  if( ret == SOCKET_ERROR )
  {
    printf("Send failed: %d\n", WSAGetLastError());
    closesocket(connectedSocket);
    WSACleanup();
    return 1;
  }
  return ret;
}

void parseInput(string& input)
{
  int commandIndex = input.find(":");

  // We have a command
  if( commandIndex != -1 )
  {
    // Grab the command
    string command = input.substr(0, commandIndex);

    // Figure out what is being asked and act accordingly
    if( command == "q" || command == "Q" )
    {
      sendMessage(connectSocket, string("QUIT"));
      isExiting = true;
    }
    else if( command == "join" || command == "JOIN" )
    {
      // Grab channel name
      string channel = input.substr(commandIndex + 1, input.length());
      string join = "JOIN ";
      if( channel.find("#") == -1 )
        channel.insert(0, "#");
      sendMessage(connectSocket, join + channel);
      currentChannel = channel;
    }
  }
  // No command
  else
  {
    // Send the input as a message to wherever we're currently looking
    if( currentChannel != "" )
      sendMessage(connectSocket, "PRIVMSG" + currentChannel + " :" + input);
  }
}