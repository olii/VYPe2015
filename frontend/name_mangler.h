#ifndef FRONTEND_NAME_MANGLER_H
#define FRONTEND_NAME_MANGLER_H

#include <string>
#include "frontend/symbol.h"

namespace frontend {

class NameMangler
{
public:
	NameMangler() = delete;
	NameMangler(const NameMangler&) = delete;
	~NameMangler() {}

	static std::string mangle(const std::string& functionName, const std::vector<Symbol::DataType>& paramTypes);
};

} // namespace frontend

#endif
