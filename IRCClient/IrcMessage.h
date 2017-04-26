#pragma once

class IrcMessage
{
public:
  IrcMessage(const std::string& message);
  ~IrcMessage();

  enum CommandType {
    UKNOWN,
    PING,
    JOIN,
    PART,
    PRVMSG,
    NOTICE,
    QUIT,
    NONE
  };

  virtual const CommandType GetCommandType() const = 0;
  virtual const std::string GetMessageString() const = 0;

protected:
  struct CommandMessage
  {
    CommandMessage() : Command(CommandType::UKNOWN) {}
    std::string      Message;
    CommandType Command;
  } commandMessage_;

  virtual void ConvertMessageToCommandMessage() = 0;
  virtual const CommandType GetCommandTypeFromString(const std::string& commandString) const = 0;

  std::string initialMessage_;
};

