#pragma once


namespace utils
{
	namespace conver
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
	}
	
	namespace process
	{
		std::string GetFullProcessName(DWORD pid) {
			HANDLE hProcess(::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid));
			if (hProcess) {
				WCHAR path[MAX_PATH];
				DWORD size = MAX_PATH;
				if (::QueryFullProcessImageName(hProcess, 0, path, &size))
					return conver::wstring_to_stirng(path);
			}
			return "";
		}
	}

	namespace image
	{
		HICON GetWindowIcon(HWND hwnd) {
			HICON hIcon{ nullptr };
			SendMessageTimeout(hwnd, WM_GETICON, ICON_SMALL2, 0, SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT, 100, (DWORD_PTR*)&hIcon);
			if (!hIcon) {
				hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM);
			}
			return hIcon;
		}

		HICON GetProcessIcon(std::string path)
		{
			HICON hIcon{};
			ExtractIconExA(path.c_str(), 0, nullptr, &hIcon, 1);
			return hIcon;
		}

		HICON GetProcessIcon(HWND hwnd)
		{
			auto hIcon = GetWindowIcon(hwnd);
			if (!hIcon) {
				DWORD pid = 0;
				GetWindowThreadProcessId(hwnd, &pid);
				if (pid) {
					ExtractIconExA(process::GetFullProcessName(pid).c_str(), 0, nullptr, &hIcon, 1);
				}
			}

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
	}

	namespace file
	{
		bool QueryValue(const std::string& ValueName, const std::string& szModuleName, std::string& RetStr)
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
				DWORD dwDataSize = GetFileVersionInfoSizeA((LPCSTR)szModuleName.c_str(),
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
				if (!GetFileVersionInfoA((LPCSTR)szModuleName.c_str(), dwHandle, dwDataSize,
					(void*)m_lpVersionData))
				{
					break;
				}
				UINT nQuerySize;
				DWORD* pTransTable;
				if (!VerQueryValueA(m_lpVersionData, "\\VarFileInfo\\Translation",
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
			return QueryValue("ProductVersion", szModuleName, ret) ? ret : "";
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

		void OpenFolderAndSelectFile(const char* filepath)
		{
			LPSHELLFOLDER pDesktopFolder;
			CoInitialize(NULL);

			if (FAILED(SHGetDesktopFolder(&pDesktopFolder)))
			{
				CoUninitialize();
				return;
			}

			LPITEMIDLIST pidl;
			ULONG chEaten;
			HRESULT hr = pDesktopFolder->ParseDisplayName(NULL, 0, (LPWSTR)conver::string_to_wstirng(filepath).c_str(), &chEaten, &pidl, NULL);
			if (FAILED(hr))
			{
				pDesktopFolder->Release();
				CoUninitialize();
				return;
			}
			SHOpenFolderAndSelectItems(pidl, 0, NULL, 0);
			pDesktopFolder->Release();
			CoUninitialize();
		}

		std::string openFileDlg()
		{
			OPENFILENAMEA ofn;
			char szFile[MAX_PATH];
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = render::get_instasnce()->GetHwnd();
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = _T('\0');
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "Dll\0*.Dll\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if (GetOpenFileNameA(&ofn))
			{
				return  ofn.lpstrFile;
			}
			else
			{
				return "";
			}
		}
	}

	namespace mem
	{
		std::string GetProtectByValue(uint32_t value)
		{
			std::string ret{};

			if (value & PAGE_NOACCESS)
				ret += "NoAccess|";
			if (value & PAGE_READONLY)
				ret += "ReadOnly|";
			if (value & PAGE_WRITECOPY)
				ret += "WriteCopy|";
			if (value & PAGE_EXECUTE)
				ret += "Execute|";
			if (value & PAGE_EXECUTE_READ)
				ret += "ExecuteRead|";
			if (value & PAGE_EXECUTE_READWRITE)
				ret += "ExecuteReadWrite|";
			if (value & PAGE_EXECUTE_WRITECOPY)
				ret += "ExecuteReadWriteCopy|";
			if (value & PAGE_NOCACHE)
				ret += "NoCache|";

			if (!ret.empty())
				ret = ret.substr(0, ret.length() - 1);
			else
				ret = "-";

			return ret;
		}

		std::string GetStateByValue(uint32_t value)
		{
			std::string ret{};

			if (value & MEM_COMMIT)
				ret += "Commit|";
			if (value & MEM_RESERVE)
				ret += "Reserve|";
			if (value & MEM_FREE)
				ret += "Free|";

			if (!ret.empty())
				ret = ret.substr(0, ret.length() - 1);
			else
				ret = "-";

			return ret;
		}

		std::string GetTypeByValue(uint32_t value)
		{
			std::string ret{};

			if (value & MEM_PRIVATE	)
				ret += "Private";
			if (value & MEM_MAPPED)
				ret += "Image";
			if (value & MEM_FREE)
				ret += "MEM_IMAGE";

			if (!ret.empty())
				ret = ret.substr(0, ret.length() - 1);
			else
				ret = "-";

			return ret;
		}

		bool InvokePluginFunction(DWORD pid, ControlCmd cmd)
		{
			return Mem::RemoteCallFunction(pid, config::global::lpPluginDispatch, &cmd, sizeof(ControlCmd));
		}
	}

	namespace normal
	{
		void CopyStringToClipboard(const char* str)
		{
			if (OpenClipboard(render::get_instasnce()->GetHwnd())) {
				EmptyClipboard();
				HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, strlen(str)*2+2);
				LPWSTR pData = (LPWSTR)GlobalLock(hData);
				lstrcpyW(pData,conver::string_to_wstirng(str).c_str());
				GlobalUnlock(hData);
				SetClipboardData(CF_UNICODETEXT, hData);
				CloseClipboard();
			}
		}
	}

	namespace window
	{
		HWND GetHwnd(DWORD dwProcessID)
		{
			HWND hTop = GetTopWindow(0);
			HWND retHwnd = NULL;
			while (hTop)
			{
				DWORD pid = 0;
				DWORD dwTheardId = GetWindowThreadProcessId(hTop, &pid);
				if (dwTheardId != 0)
				{
					if (pid == dwProcessID && GetParent(hTop) == NULL && ::IsWindowVisible(hTop))
					{
						retHwnd = hTop;    //���ж�����ֵ
					}
				}
				hTop = GetNextWindow(hTop, GW_HWNDNEXT);
			}
			return retHwnd;
		}

		std::string GetTitleName(HWND hwnd)
		{
			char title[256]{};
			GetWindowTextA(hwnd,title,256);
			return title;
		}

		std::string GetKlassName(HWND hwnd)
		{
			char klassname[256]{};
			GetClassNameA(hwnd, klassname, 256);
			return klassname;
		}
	}
}

