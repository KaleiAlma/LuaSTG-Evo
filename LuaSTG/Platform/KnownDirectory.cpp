#include "Shared.hpp"
#include <cstdlib>
#include <tuple>
#include "KnownDirectory.hpp"

namespace Platform
{
	// inline bool getDirectory(KNOWNFOLDERID const& id, std::wstring& out)
	// {
	// 	out.clear();
	// 	bool br = false;
	// 	HRESULT hr = S_OK;
	// 	Microsoft::WRL::ComPtr<IKnownFolderManager> manager;
	// 	hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&manager));
	// 	if (SUCCEEDED(hr))
	// 	{
	// 		Microsoft::WRL::ComPtr<IKnownFolder> folder;
	// 		hr = manager->GetFolder(id, &folder);
	// 		if (SUCCEEDED(hr))
	// 		{
	// 			LPWSTR str = NULL;
	// 			hr = folder->GetPath(0, &str);
	// 			if (str != NULL)
	// 			{
	// 				if (SUCCEEDED(hr))
	// 				{
	// 					out = str;
	// 					br = true;
	// 				}
	// 				CoTaskMemFree(str);
	// 			}
	// 		}
	// 	}
	// 	return br;
	// }

	// bool KnownDirectory::getLocalAppDataW(std::wstring& buf)
	// {
	// 	return getDirectory(FOLDERID_LocalAppData, buf);
	// }
	// bool KnownDirectory::getRoamingAppDataW(std::wstring& buf)
	// {
	// 	return getDirectory(FOLDERID_RoamingAppData, buf);
	// }
	// bool KnownDirectory::makeAppDataW(std::wstring_view const& company, std::wstring_view const& product, std::wstring& buf)
	// {
	// 	buf.clear();
	// 	std::wstring ret;
	// 	BOOL br = FALSE;

	// 	if (!getRoamingAppDataW(ret))
	// 	{
	// 		return false;
	// 	}

	// 	ret.push_back(L'\\');
	// 	ret.append(company);
	// 	br = CreateDirectoryW(ret.c_str(), NULL);
	// 	if (!br && (GetLastError() != ERROR_ALREADY_EXISTS))
	// 	{
	// 		return false;
	// 	}

	// 	ret.push_back(L'\\');
	// 	ret.append(product);
	// 	br = CreateDirectoryW(ret.c_str(), NULL);
	// 	if (!br && (GetLastError() != ERROR_ALREADY_EXISTS))
	// 	{
	// 		return false;
	// 	}

	// 	buf = std::move(ret);
	// 	return true;
	// }

	bool KnownDirectory::getAppData(std::string& buf)
	{
		buf.clear();
		// std::wstring ret;
		// if (getLocalAppDataW(ret))
		// {
		// 	buf = std::move(to_utf8(ret));
		// 	return true;
		// }

#ifdef _WIN32
		{
			const char* home_drive = std::getenv("HOMEDRIVE");
			if (!home_drive) return false;
			buf.append(home_drive);
			const char* home_path = std::getenv("HOMEPATH");
			if (!home_path) return false;
			buf.append(home_path);
		}
#else
		{
			const char* home = std::getenv("HOME");
			if (!home) return false;
			buf.append(home);
		}
#endif
		buf.append("/.luastg-evo/");
		return true;
	}
	// bool KnownDirectory::getRoamingAppData(std::string& buf)
	// {
	// 	buf.clear();
	// 	std::wstring ret;
	// 	if (getRoamingAppDataW(ret))
	// 	{
	// 		buf = std::move(to_utf8(ret));
	// 		return true;
	// 	}
	// 	return false;
	// }
	bool KnownDirectory::makeAppData(std::string_view const& company, std::string_view const& product, std::string& buf)
	{
		buf.clear();
		std::wstring ret;
		if (makeAppDataW(to_wide(company), to_wide(product), ret))
		{
			buf = std::move(to_utf8(ret));
			return true;
		}
		return false;
	}
}
