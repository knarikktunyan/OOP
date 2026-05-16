#pragma once

enum class ParserState
{
	Start,
	WaitOperator,
	WaitOperand,
	Error,
	End
};
