#pragma once
#ifndef _DATAKIT_SDK_ABSTRACT_MANAGER_H_
#define _DATAKIT_SDK_ABSTRACT_MANAGER_H_

#include <string>

namespace com::ft::sdk::internal
{
	class AbstractManager
	{
	public:
		AbstractManager() {}

	protected:
		std::string m_name;
	};

}
#endif // !_DATAKIT_SDK_ABSTRACT_MANAGER_H_



