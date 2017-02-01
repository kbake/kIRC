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

  const CommandType GetCommandType() const;
  const std::string GetMessageString() const;

private:
  struct CommandMessage
  {
    CommandMessage() : Command(CommandType::UKNOWN) {}
    std::string      Message;
    CommandType Command;
  } commandMessage_;

  void ConvertMessageToCommandMessage();
  const CommandType GetCommandTypeFromString(const std::string& commandString) const;

  std::string initialMessage_;
};

