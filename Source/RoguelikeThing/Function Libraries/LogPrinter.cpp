﻿// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/LogPrinter.h"

DEFINE_LOG_CATEGORY(BluePrintMessage);

void ULogPrinter::PrintLog(FString Message, FMessageType Type)
{
	switch (Type)
	{
	case FMessageType::LOG:
		UE_LOG(BluePrintMessage, Log, TEXT("%s"), *Message);
		break;
	case FMessageType::WARNING:
		UE_LOG(BluePrintMessage, Warning, TEXT("%s"), *Message);
		break;
	case FMessageType::ERROR:
		UE_LOG(BluePrintMessage, Error, TEXT("%s"), *Message);
		break;
	default:
		break;
	}
}
