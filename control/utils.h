#pragma once


namespace utils
{
	uint64_t hexToInteger(std::string hex)
	{
		std::stringstream ss2;
		uint64_t d2;
		ss2 << std::hex << hex;
		ss2 >> d2;
		return d2;
	}

	std::string IntegerTohex(uint64_t dec)
	{
		std::stringstream ss2;
		ss2 << std::hex << dec;
		return ss2.str();
	}

	std::string bytesToHexString(const BYTE* bytes, const int length)
	{
		if (bytes == NULL) {
			return "";
		}
		std::string buff;
		const int len = length;
		for (int j = 0; j < len; j++) {
			int high = bytes[j] / 16, low = bytes[j] % 16;
			buff += (high < 10) ? ('0' + high) : ('a' + high - 10);
			buff += (low < 10) ? ('0' + low) : ('a' + low - 10);
			buff += " ";
		}
		return buff;
	}

	std::string wstring_to_stirng(std::wstring wstr)
	{
		std::string result;
		//��ȡ��������С��������ռ䣬��������С�°��ֽڼ����  
		int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
		char* buffer = new char[len + 1];
		//���ֽڱ���ת���ɶ��ֽڱ���  
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
		buffer[len] = '\0';
		//ɾ��������������ֵ  
		result.append(buffer);
		delete[] buffer;
		return result;
	}

	std::wstring string_to_wstirng(std::string str)
	{
		std::wstring result;
		//��ȡ��������С��������ռ䣬��������С���ַ�����  
		int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
		TCHAR* buffer = new TCHAR[len + 1];
		//���ֽڱ���ת���ɿ��ֽڱ���  
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
		buffer[len] = '\0';             //����ַ�����β  
		result.append(buffer);
		delete[] buffer;
		return result;
	}

	std::string string_To_UTF8(const std::string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);

		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

		int nLen = (int)(::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL));

		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);

		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

		std::string retStr(pBuf);

		delete[]pwBuf;
		delete[]pBuf;

		pwBuf = NULL;
		pBuf = NULL;

		return retStr;
	}

	HICON GetProcessIcon(std::string path)
	{
		HICON hIcon{};
		ExtractIconExA(path.c_str(), 0, nullptr, &hIcon, 1);
		return hIcon;
	}

	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
		UINT num = 0, size = 0;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0)
			return -1;  // Failure

		Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));

		Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
		bool found = false;
		for (UINT ix = 0; !found && ix < num; ++ix)
		{
			if (_wcsicmp(pImageCodecInfo[ix].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[ix].Clsid;
				found = true;
				break;
			}
		}

		free(pImageCodecInfo);
		return found;
	}

	bool SaveIconToPng(HICON hIcon, std::string lpszPicFileName)
	{
		static bool init = false;
		if (!init)
		{
			ULONG_PTR m_gdiplusToken;
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
			init = true;
		}

		if (hIcon == NULL)
		{
			return false;
		}

		ICONINFO icInfo = { 0 };
		if (!::GetIconInfo(hIcon, &icInfo))
		{
			return false;
		}

		BITMAP bitmap;
		GetObject(icInfo.hbmColor, sizeof(BITMAP), &bitmap);

		Gdiplus::Bitmap* pBitmap = NULL;
		Gdiplus::Bitmap* pWrapBitmap = NULL;

		do
		{
			if (bitmap.bmBitsPixel != 32)
			{
				pBitmap = Gdiplus::Bitmap::FromHICON(hIcon);
			}
			else
			{
				pWrapBitmap = Gdiplus::Bitmap::FromHBITMAP(icInfo.hbmColor, NULL);
				if (!pWrapBitmap)
					break;

				Gdiplus::BitmapData bitmapData;
				Gdiplus::Rect rcImage(0, 0, pWrapBitmap->GetWidth(), pWrapBitmap->GetHeight());

				pWrapBitmap->LockBits(&rcImage, Gdiplus::ImageLockModeRead, pWrapBitmap->GetPixelFormat(), &bitmapData);
				pBitmap = new (Gdiplus::Bitmap)(bitmapData.Width, bitmapData.Height, bitmapData.Stride, PixelFormat32bppARGB, (BYTE*)bitmapData.Scan0);
				pWrapBitmap->UnlockBits(&bitmapData);
			}

			CLSID encoderCLSID;
			GetEncoderClsid(_T("image/png"), &encoderCLSID);
			std::wstring aa{ lpszPicFileName.begin(),lpszPicFileName.end() };
			Gdiplus::Status st = pBitmap->Save(aa.c_str(), &encoderCLSID, NULL);
			if (st != Gdiplus::Ok)
				break;

		} while (false);

		delete pBitmap;
		if (pWrapBitmap)
			delete pWrapBitmap;
		DeleteObject(icInfo.hbmColor);
		DeleteObject(icInfo.hbmMask);

		return true;
	}

	HMODULE GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName) {	// ���� PID ��ģ����(��Ҫд��׺���磺".dll")����ȡģ����ڵ�ַ��
		MODULEENTRY32 moduleEntry;
		HANDLE handle = NULL;
		handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid); //  ��ȡ���̿����а�����th32ProcessID��ָ���Ľ��̵����е�ģ�顣
		if (!handle) {
			CloseHandle(handle);
			return NULL;
		}
		ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
		moduleEntry.dwSize = sizeof(MODULEENTRY32);
		if (!Module32First(handle, &moduleEntry)) {
			CloseHandle(handle);
			return NULL;
		}
		do {
			if (_tcscmp(moduleEntry.szModule, moduleName) == 0) { return moduleEntry.hModule; }
		} while (Module32Next(handle, &moduleEntry));
		CloseHandle(handle);
		return 0;
	}

	BOOL RemoteInjectDLL(DWORD PID, const char* Path) 
	{
		HANDLE hProcess;
		DWORD dwSize = 0;
		LPVOID PDllAddr;
		FARPROC func;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
		if (hProcess == NULL) 
		{
			MessageBoxA(NULL,"OpenProcess Failed!","control",NULL);
			return false;
		}
		dwSize = strlen(Path) + 1;
		PDllAddr = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
		if (PDllAddr == NULL) 
		{
			MessageBoxA(NULL, "VirtualAllocEx Error", "control", NULL);
			return false;
		}
		BOOL pWrite = WriteProcessMemory(hProcess, PDllAddr, Path, dwSize, NULL);
		if (!pWrite) 
		{
			MessageBoxA(NULL, "WriteProcessMemory Error", "control", NULL);
			return false;
		}
		HMODULE hModule = GetModuleHandleA("kernel32.dll");
		func = GetProcAddress(hModule, "LoadLibraryA");
		if (func == NULL) 
		{
			MessageBoxA(NULL, "GetProcAddress Error", "control", NULL);
			return false;

		}
		HANDLE pRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)func, PDllAddr, 0, NULL);
		if (pRemoteThread == NULL) 
		{
			MessageBoxA(NULL, "CreateRemoteThread Error", "control", NULL);
			return false;
		}
		WaitForSingleObject(pRemoteThread, INFINITE);
		VirtualFreeEx(hProcess, PDllAddr, dwSize, MEM_COMMIT);
		CloseHandle(hProcess);
		return true;
	}

	bool RemoteCallFunction(DWORD PID,uint64_t addr,PVOID pargs,size_t args_size)
	{
		auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
		if (hProcess == NULL)
		{
			MessageBoxA(NULL, "OpenProcess Failed!", "control", NULL);
			return false;
		}
		//��������ڴ�
		auto lpParam = VirtualAllocEx(hProcess, NULL, args_size, MEM_COMMIT, PAGE_READWRITE);
		if (lpParam == NULL)
		{
			MessageBoxA(NULL, "VirtualAllocEx Params Failed!", "control", NULL);
			return false;
		}

		if (!WriteProcessMemory(hProcess, lpParam, pargs, args_size, NULL))
		{
			MessageBoxA(NULL, "Write Params Failed!", "control", NULL);
			return false;
		}
		HANDLE  pRemoteThread = CreateRemoteThread(hProcess, NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(addr), lpParam, NULL, NULL);
		if (pRemoteThread == NULL)
		{
			MessageBoxA(NULL, "CreateRemoteThread Error", "control", NULL);
			return false;
		}
		WaitForSingleObject(pRemoteThread, INFINITE);
		VirtualFreeEx(hProcess, lpParam, sizeof(ControlCmd), MEM_COMMIT);
		CloseHandle(hProcess);
		return true;
	}


	bool QueryValue(const std::string& ValueName, const std::string& szModuleName,std::string& RetStr)
	{
		bool bSuccess = FALSE;
		BYTE* m_lpVersionData = NULL;
		DWORD  m_dwLangCharset = 0;
		CHAR* tmpstr = NULL;
		do
		{
			if (!ValueName.size() || !szModuleName.size())
			{
				break;
			}
			DWORD dwHandle;
			// �ж�ϵͳ�ܷ������ָ���ļ��İ汾��Ϣ
			//��԰����˰汾��Դ��һ���ļ����ж����� �ļ��汾��Ϣ��Ҫһ�����Ļ�����
			//����ֵ˵��Long�������ļ��İ汾��Դ����Ļ��������ȡ����ļ��������汾��Ϣ���򷵻�һ��0ֵ��������GetLastError������
			DWORD dwDataSize = ::GetFileVersionInfoSizeA((LPCSTR)szModuleName.c_str(),
				&dwHandle);
			if (dwDataSize == 0)
			{
				break;
			}
			//std::nothrow:���ڴ治��ʱ��new (std::nothrow)�����׳��쳣�����ǽ�ָ����NULL��
			m_lpVersionData = new (std::nothrow) BYTE[dwDataSize];// ���仺����
			if (NULL == m_lpVersionData)
			{
				break;
			}
			// ������Ϣ
			//��֧�ְ汾��ǵ�һ��ģ�����ȡ�ļ��汾��Ϣ
			if (!::GetFileVersionInfoA((LPCSTR)szModuleName.c_str(), dwHandle, dwDataSize,
				(void*)m_lpVersionData))
			{
				break;
			}
			UINT nQuerySize;
			DWORD* pTransTable;
			if (!::VerQueryValueA(m_lpVersionData, "\\VarFileInfo\\Translation",
				(void**)&pTransTable, &nQuerySize))
			{
				break;
			}
			//MAKELONG ������16λ�������ϳ�һ���޷��ŵ�32λ��
			m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
			if (m_lpVersionData == NULL)
			{
				break;
			}
			tmpstr = new (std::nothrow) CHAR[128];// ���仺����
			if (NULL == tmpstr)
			{
				break;
			}
			sprintf_s(tmpstr, 128, "\\StringFileInfo\\%08lx\\%s", m_dwLangCharset,
				ValueName.c_str());
			LPVOID lpData;
			// ���ô˺�����ѯǰ��Ҫ�����ε��ú���GetFileVersionInfoSize��GetFileVersionInfo
			if (::VerQueryValueA((void*)m_lpVersionData, tmpstr, &lpData, &nQuerySize))
			{
				RetStr = (char*)lpData;
			}
			bSuccess = TRUE;
		} while (FALSE);
		// ���ٻ�����
		if (m_lpVersionData)
		{
			delete[] m_lpVersionData;
			m_lpVersionData = NULL;
		}
		if (tmpstr)
		{
			delete[] tmpstr;
			tmpstr = NULL;
		}
		return bSuccess;
	}

	//��ȡ�ļ�˵��
	std::string GetFileDescription(const std::string szModuleName)
	{
		std::string ret{};
		return QueryValue("FileDescription", szModuleName, ret) ? ret : "";
	}

	//��ȡ�ļ��汾
	std::string GetFileVersion(const std::string szModuleName)
	{
		std::string ret{};
		return QueryValue("FileVersion", szModuleName, ret) ? ret : "";
	}
	//��ȡ�ڲ�����
	std::string  GetInternalName(const std::string szModuleName)
	{
		std::string ret{};
		return QueryValue("InternalName", szModuleName, ret) ? ret : "";
	}

	//��ȡ��˾����
	std::string  GetCompanyName(const std::string szModuleName)
	{
		std::string ret{};
		return QueryValue("CompanyName", szModuleName, ret) ? ret : "";
	}

	//��ȡ��Ȩ
	std::string GetLegalCopyright(const std::string szModuleName)
	{
		std::string ret{};
		return QueryValue("LegalCopyright", szModuleName, ret) ? ret : "";
	}

	//��ȡԭʼ�ļ���
	std::string GetOriginalFilename(const std::string szModuleName)
	{
		std::string ret{};
		return QueryValue("OriginalFilename", szModuleName, ret) ? ret : "";
	}

	//��ȡ��Ʒ����
	std::string  GetProductName(const std::string szModuleName)
	{
		std::string ret{};
		return QueryValue("ProductName", szModuleName, ret) ? ret : "";
	}

	//��ȡ��Ʒ�汾
	std::string GetProductVersion(const std::string szModuleName)
	{
		std::string ret{};
		return QueryValue("ProductVersion", szModuleName, ret)?ret:"";
	}

	void OpenFilePropertyDlg(const char* path)
	{
		SHELLEXECUTEINFOA shell;
		shell.hwnd = NULL;
		shell.lpVerb = "properties";
		shell.lpFile = path;
		shell.lpDirectory = NULL;
		shell.lpParameters = NULL;
		shell.nShow = SW_SHOWNORMAL;
		shell.fMask = SEE_MASK_INVOKEIDLIST;
		shell.lpIDList = NULL;
		shell.cbSize = sizeof(SHELLEXECUTEINFOW);
		ShellExecuteExA(&shell);
	}
}

