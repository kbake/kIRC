#pragma once
#include "IrcMessage.h"
class IrcReceiveMessage :
  public IrcMessage
{
public:
  IrcReceiveMessage(const std::string&);
  ~IrcReceiveMessage();

  const CommandType GetCommandType() const;
  const std::string GetMessageString() const;

protected:
  void ConvertMessageToCommandMessage();
  const CommandType GetCommandTypeFromString(const std::string& commandString) const;
};

