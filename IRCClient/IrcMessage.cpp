#include "stdafx.h"
#include "IrcMessage.h"


IrcMessage::IrcMessage(const std::string& message)
{
  initialMessage_ = message;
  ConvertMessageToCommandMessage();
}


IrcMessage::~IrcMessage()
{
}

void IrcMessage::ConvertMessageToCommandMessage()
{
  int commandIndex = initialMessage_.find(":");
  if( commandIndex != -1 )
  {
    commandMessage_.Command = GetCommandTypeFromString(initialMessage_.substr(0, commandIndex));

    // If we didn't figure a specific command, just use the whole string as our message
    if( commandMessage_.Command == NONE )
      commandMessage_.Message = initialMessage_;
    else
      commandMessage_.Message = initialMessage_.substr(commandIndex + 1, initialMessage_.length());
  }
  else
  {
    commandMessage_.Command = CommandType::NONE;
    commandMessage_.Message = initialMessage_;
  }
}

const IrcMessage::CommandType IrcMessage::GetCommandTypeFromString(const std::string& commandString) const
{
  std::string lower = commandString;
  std::transform(commandString.begin(), commandString.end(), lower.begin(), ::tolower);

  if( lower == "ping"   ) return PING;
  if( lower == "join"   ) return JOIN;
  if( lower == "part"   ) return PART;
  if( lower == "prvmsg" ) return PRVMSG;
  if( lower == "quit"   ) return QUIT;
  if( lower == "notice" ) return NOTICE;
  else                    return NONE;
}

const std::string IrcMessage::GetMessageString() const
{
  return commandMessage_.Message;
}

const IrcMessage::CommandType IrcMessage::GetCommandType() const
{
  return commandMessage_.Command;
}