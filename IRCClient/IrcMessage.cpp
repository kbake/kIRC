#include "stdafx.h"
#include "IrcMessage.h"


IrcMessage::IrcMessage(const std::string& message)
{
  initialMessage_ = message;
}


IrcMessage::~IrcMessage()
{
}