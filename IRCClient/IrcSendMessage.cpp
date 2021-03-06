#include "stdafx.h"
#include "IrcSendMessage.h"


IrcSendMessage::IrcSendMessage(const std::string& message)
  :IrcMessage(message)
{
  ConvertMessageToCommandMessage();
}


IrcSendMessage::~IrcSendMessage()
{
}

void IrcSendMessage::ConvertMessageToCommandMessage()
{
  if( initialMessage_.size() > 0 && initialMessage_.at(0) == '/' )
  {
    int spaceIndex = initialMessage_.find(" ");
    commandMessage_.Command = GetCommandTypeFromString(initialMessage_.substr(1, spaceIndex-1));

    // If we didn't figure a specific command, just use the whole string as our message
    if( commandMessage_.Command == NONE )
      commandMessage_.Message = initialMessage_;
    else
      commandMessage_.Message = initialMessage_.substr(spaceIndex + 1, initialMessage_.length());
  }
  else
  {
    commandMessage_.Command = CommandType::NONE;
    commandMessage_.Message = initialMessage_;
  }
}

const IrcMessage::CommandType IrcSendMessage::GetCommandTypeFromString(const std::string& commandString) const
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

const std::string IrcSendMessage::GetMessageString() const
{
  return commandMessage_.Message;
}

const IrcMessage::CommandType IrcSendMessage::GetCommandType() const
{
  return commandMessage_.Command;
}