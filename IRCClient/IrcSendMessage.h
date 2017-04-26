#pragma once
#include "IrcMessage.h"
class IrcSendMessage :
  public IrcMessage
{
public:
  IrcSendMessage(const std::string&);
  ~IrcSendMessage();

  virtual const CommandType GetCommandType() const;
  virtual const std::string GetMessageString() const;

protected:
  virtual void ConvertMessageToCommandMessage();
  virtual const CommandType GetCommandTypeFromString(const std::string& commandString) const;
};

