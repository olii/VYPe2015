/**
 * Project VYPe 2015/2016
 * Marek Milkovič (xmilko01) & Oliver Nemček (xnemce03)
 **/

#include <sstream>

#include "frontend/name_mangler.h"

namespace frontend {

std::string NameMangler::mangle(const std::string& functionName, const std::vector<Symbol::DataType>& paramTypes)
{
	std::stringstream ss;
	ss << functionName << "$" << paramTypes.size();
	for (const auto& paramType : paramTypes)
	{
		switch (paramType)
		{
			case Symbol::DataType::INT:
				ss << 'i';
				break;
			case Symbol::DataType::CHAR:
				ss << 'c';
				break;
			case Symbol::DataType::STRING:
				ss << 's';
				break;
			default:
				return nullptr; // This never happens
		}
	}
	ss << "$";
	return ss.str();
}

} // namespace frontend
