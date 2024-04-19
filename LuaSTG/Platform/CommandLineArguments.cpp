#include "CommandLineArguments.hpp"

namespace Platform {
	bool CommandLineArguments::Update(int argc, char *argv[]) {
		for (int i = 0; i < argc; i++) {
			m_args.push_back(argv[i]);
		}
		return true;
	}
	bool CommandLineArguments::GetArguments(std::vector<std::string_view>& list) {
		if (m_args.empty()) {
			// if (!Update())
			// {
				return false;
			// }
		}

		list.resize(m_args.size());
		for (size_t i = 0; i < m_args.size(); i += 1) {
			list[i] = m_args[i];
		}

		return true;
	}
	bool CommandLineArguments::IsOptionExist(std::string_view option) {
		if (m_args.empty()) {
			// if (!Update())
			// {
				return false;
			// }
		}

		for (auto const& v : m_args) {
			if (v == option) {
				return true;
			}
		}

		return false;
	}

	CommandLineArguments::CommandLineArguments()
	{
	}
	CommandLineArguments::~CommandLineArguments()
	{
	}

	CommandLineArguments& CommandLineArguments::Get()
	{
		static CommandLineArguments instace;
		return instace;
	}
}
