#include <string>
#include <Windows.h>
#include <vmmdll.h>
#include <array>
#include <unordered_map>
#include <deque>
#include "iostream"

#pragma comment(lib, "vmm.lib")
#pragma comment(lib, "leechcore.lib")
#pragma comment(lib, "FTD3XX.lib")

// set to FALSE if you dont want to track the total read size of the DMA
#define COUNT_TOTAL_READSIZE FALSE
class DMAHandler
{

	// Static variables, shared over all instances

	struct LibModules
	{
		HMODULE VMM = nullptr;
		HMODULE FTD3XX = nullptr;
		HMODULE LEECHCORE = nullptr;
	};

	static inline LibModules modules{};

	static inline VMM_HANDLE DMA_HANDLE = nullptr;

	// Counts the size of the reads in total. Reset every frame preferrably for memory tracking
	static inline DWORD64 readSize = 0;

	// Nonstatic variables, different for each class object on purpose, in case the user tries to access
	// multiple processes

	struct BaseProcessInfo
	{
		DWORD pid = 0;
		std::string name;
		const wchar_t* wname;
		ULONG64 base = 0;
		ULONG64 dtb = 0;
		ULONG64 pml4[512];
	};
	

	BOOLEAN PROCESS_INITIALIZED = FALSE;


	// Private log function used by the DMAHandler class
	static void log(const char* fmt, ...);

	// Will always throw a runtime error if PROCESS_INITIALIZED or DMA_INITIALIZED is false
	void assertNoInit() const;

	// Wow we have friends
	template<typename> friend class DMAScatter;

	static void retrieveScatter(VMMDLL_SCATTER_HANDLE handle, void* buffer, void* target, SIZE_T size);

	static bool DumpMemoryMap();

	std::string GameName;
	std::wstring WGameName;

	std::unordered_map<std::wstring, ULONG64> Modules;

	DWORD winlogonPid = 0;
	uintptr_t gafAsyncKeyStateBase = 0;

	std::array<uint8_t, 64> key_state;

	bool isKeyDown(std::array<uint8_t, 64>& keyState, uint8_t vkKey)
	{
		size_t index = vkKey * 2 / 8;
		size_t bitOffset = vkKey % 4 * 2;
		return (keyState[index] & (1 << bitOffset)) != 0;
	}

public:

	BaseProcessInfo processInfo{};

	uint64_t gafAsyncKeyStateExport = 0;
	uint8_t state_bitmap[64]{ };
	uint8_t previous_state_bitmap[256 / 8]{ };
	uint64_t win32kbase = 0;

	DWORD win_logon_pid = 0;

	enum class e_registry_type
	{
		none = REG_NONE,
		sz = REG_SZ,
		expand_sz = REG_EXPAND_SZ,
		binary = REG_BINARY,
		dword = REG_DWORD,
		dword_little_endian = REG_DWORD_LITTLE_ENDIAN,
		dword_big_endian = REG_DWORD_BIG_ENDIAN,
		link = REG_LINK,
		multi_sz = REG_MULTI_SZ,
		resource_list = REG_RESOURCE_LIST,
		full_resource_descriptor = REG_FULL_RESOURCE_DESCRIPTOR,
		resource_requirements_list = REG_RESOURCE_REQUIREMENTS_LIST,
		qword = REG_QWORD,
		qword_little_endian = REG_QWORD_LITTLE_ENDIAN
	};

	std::string QueryValue(const char* path, e_registry_type type)
	{

		BYTE buffer[0x128];
		DWORD _type = static_cast<DWORD>(type);
		DWORD size = sizeof(buffer);

		if (!VMMDLL_WinReg_QueryValueExU(DMA_HANDLE, const_cast<LPSTR>(path), &_type, buffer, &size))
		{
			return "";
		}

		if (type == e_registry_type::dword)
		{
			DWORD dwordValue = *reinterpret_cast<DWORD*>(buffer);
			return std::to_string(dwordValue);
		}

		std::wstring wstr = std::wstring(reinterpret_cast<wchar_t*>(buffer));
		return std::string(wstr.begin(), wstr.end());
	}

	std::vector<int> GetPidListFromName(std::string name) {
		PVMMDLL_PROCESS_INFORMATION process_info = NULL;
		DWORD total_processes = 0;
		std::vector<int> list = { };

		if (!VMMDLL_ProcessGetInformationAll(DMA_HANDLE, &process_info, &total_processes))
		{
			return list;
		}

		for (size_t i = 0; i < total_processes; i++)
		{
			auto process = process_info[i];
			if (strstr(process.szNameLong, name.c_str()))
				list.push_back(process.dwPID);
		}

		return list;
	}

	/**
	 * \brief Constructor takes a wide string of the process.
	 * Expects that all the libraries are in the root dir
	 * \param wname process name
	 * \param memMap whether the memory map should get dumped to file.
	 */
	int Init(const wchar_t* wname, bool memMap = true);
	int FixDTB();
	void RefreshLight();

	// Whether the DMA and Process are initialized
	bool IsInitialized() const;

	// Gets the PID of the process
	DWORD GetPID() const;

	// Gets the Base address of the process
	ULONG64 GetBaseAddress();

	ULONG64 GetModuleAddress(std::wstring modulename);
	void Read(ULONG64 address, ULONG64 buffer, SIZE_T size);

	bool cachePML4()
	{
		bool success = true;

		DWORD readsize;
		success = VMMDLL_MemReadEx(DMA_HANDLE, -1, processInfo.dtb, reinterpret_cast<PBYTE>(processInfo.pml4), sizeof(processInfo.pml4), (PDWORD)&readsize, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
		success = VMMDLL_MemReadEx((VMM_HANDLE)-666, 333, (ULONG64)processInfo.pml4, 0, 0, 0, 0);
		success = VMMDLL_ConfigSet(this->DMA_HANDLE, VMMDLL_OPT_PROCESS_DTB | this->GetPID(), 666);

		return success;
	}

	VMMDLL_SCATTER_HANDLE hS = NULL;
	VMMDLL_SCATTER_HANDLE hS2 = NULL;
	VMMDLL_SCATTER_HANDLE hS3 = NULL;
	VMMDLL_SCATTER_HANDLE hS4 = NULL;
	VMMDLL_SCATTER_HANDLE hS5 = NULL;

	bool SCreate()
	{
		hS = VMMDLL_Scatter_Initialize(DMA_HANDLE, processInfo.pid, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_SCATTER_PREPAREEX_NOMEMZERO | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
		if (hS == NULL) {
			std::cout << hue::red << "(!) " << hue::white << "Failed to create scatter handle 1" << std::endl;
			return false;
		}

		hS2 = VMMDLL_Scatter_Initialize(DMA_HANDLE, processInfo.pid, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_SCATTER_PREPAREEX_NOMEMZERO | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
		if (hS2 == NULL) {
			std::cout << hue::red << "(!) " << hue::white << "Failed to create scatter handle 2" << std::endl;
			return false;
		}

		hS3 = VMMDLL_Scatter_Initialize(DMA_HANDLE, processInfo.pid, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_SCATTER_PREPAREEX_NOMEMZERO | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
		if (hS3 == NULL) {
			std::cout << hue::red << "(!) " << hue::white << "Failed to create scatter handle 3" << std::endl;
			return false;
		}

		hS4 = VMMDLL_Scatter_Initialize(DMA_HANDLE, processInfo.pid, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_SCATTER_PREPAREEX_NOMEMZERO | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
		if (hS4 == NULL) {
			std::cout << hue::red << "(!) " << hue::white << "Failed to create scatter handle 4" << std::endl;
			return false;
		}

		hS5 = VMMDLL_Scatter_Initialize(DMA_HANDLE, processInfo.pid, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_SCATTER_PREPAREEX_NOMEMZERO | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
		if (hS5 == NULL) {
			std::cout << hue::red << "(!) " << hue::white << "Failed to create scatter handle 5" << std::endl;
			return false;
		}

		return true;
	}

	bool SPrepareEx(VMMDLL_SCATTER_HANDLE handle, uint64_t va, DWORD cb, void* pb) {
		if (!VMMDLL_Scatter_PrepareEx(handle, va, cb, (PBYTE)pb, NULL)) {
			//std::cout << "Scatter prepare failed." << std::endl;
			return false;
		}
		return true;
	}

	bool SPrepare(VMMDLL_SCATTER_HANDLE handle, uint64_t va, DWORD cb) {
		if (!VMMDLL_Scatter_Prepare(handle, va, cb)) {
			//std::cout << "Scatter prepare failed." << std::endl;
			return false;
		}
		return true;
	}

	bool ExecuteMemoryReads(VMMDLL_SCATTER_HANDLE handle) {
		if (!VMMDLL_Scatter_ExecuteRead(handle)) {
			//std::cout << "Scatter execute read failed." << std::endl;
			return false;
		}
		return true;
	}

	template<typename T>
	T SReads(VMMDLL_SCATTER_HANDLE handle, uint64_t ptr) {
		T buff;
		DWORD pcbRead;
		if (!VMMDLL_Scatter_Read(handle, ptr, sizeof(T), reinterpret_cast<PBYTE>(&buff), &pcbRead)) {
			//std::cout << "Scatter read failed." << std::endl;
		}
		if (pcbRead != sizeof(T)) {
			//std::cout << "Partial Scatter read." << std::endl;
		}
		return buff;
	}

	template<typename T>
	T SReadsSuccess(VMMDLL_SCATTER_HANDLE handle, uint64_t ptr, T value) {
		T buff;
		DWORD pcbRead;
		if (!VMMDLL_Scatter_Read(handle, ptr, sizeof(T), reinterpret_cast<PBYTE>(&buff), &pcbRead)) {
			//std::cout << "Scatter read failed." << std::endl;
		}
		if (pcbRead == sizeof(T)) {
			//std::cout << "Partial Scatter read." << std::endl;
			value = buff;
		}
		return value;
	}

	template<typename T>
	void SReadArray(VMMDLL_SCATTER_HANDLE handle, uint64_t ptr, T* buff, size_t size) {
		DWORD pcbRead;
		if (!VMMDLL_Scatter_Read(handle, ptr, size, reinterpret_cast<PBYTE>(buff), &pcbRead)) {
			//std::cout << "Scatter read failed." << std::endl;
		}
		if (pcbRead != size) {
			//std::cout << "Partial Scatter read." << std::endl;
		}
	}

	bool SClear(VMMDLL_SCATTER_HANDLE handle)
	{
		return  VMMDLL_Scatter_Clear(handle, processInfo.pid, VMMDLL_FLAG_NOCACHE);
	}

	template <typename T>
	T Read(void* address)
	{
		T buffer{};
		memset(&buffer, 0, sizeof(T));
		Read(reinterpret_cast<ULONG64>(address), reinterpret_cast<ULONG64>(&buffer), sizeof(T));

		return buffer;
	}

	void* ReadA(ULONG64 address, void* buf, uint64_t length)
	{

		Read(address, reinterpret_cast<ULONG64>(buf), length);

		return buf;
	}

	template <typename T>
	T Read(ULONG64 address)
	{
		return Read<T>(reinterpret_cast<void*>(address));
	}
	bool ReadBool(const ULONG64 address, const ULONG64 buffer, const SIZE_T size) const;
	bool Write(ULONG64 address, ULONG64 buffer, SIZE_T size) const;

	template <typename T>
	bool Write(ULONG64 address, T* buffer)
	{
		return Write(address, reinterpret_cast<ULONG64>(buffer), sizeof(T));
	}

	template <typename T>
	bool Write(void* address, T* buffer)
	{
		return Write(reinterpret_cast<ULONG64>(address), reinterpret_cast<ULONG64>(buffer), sizeof(T));
	}

	template <typename T>
	bool Write(ULONG64 address, T value)
	{
		return Write(address, reinterpret_cast<ULONG64>(&value), sizeof(T));
	}

	template <typename T>
	bool Write(void* address, T value)
	{
		return Write(reinterpret_cast<ULONG64>(address), reinterpret_cast<ULONG64>(&value), sizeof(T));
	}
	template <typename T>
	bool QueueScatterWriteEx(VMMDLL_SCATTER_HANDLE handle, uint64_t addr, T value) const
	{
		assertNoInit();

		bool ret = VMMDLL_Scatter_PrepareWrite(handle, addr, reinterpret_cast<PBYTE>(&value), sizeof(value));
		if (!ret)
		{
			log("failed to prepare scatter write at 0x%p\n", addr);
		}
		return ret;
	}

	//Handle Scatter
	void QueueScatterReadEx(VMMDLL_SCATTER_HANDLE handle, uint64_t addr, void* bffr, size_t size) const;
	void ExecuteScatterRead(VMMDLL_SCATTER_HANDLE handle) const;

	void QueueScatterWriteEx(VMMDLL_SCATTER_HANDLE handle, uint64_t addr, void* bffr, size_t size) const;
	void ExecuteScatterWrite(VMMDLL_SCATTER_HANDLE handle) const;

	VMMDLL_SCATTER_HANDLE CreateScatterHandle() const;
	void CloseScatterHandle(VMMDLL_SCATTER_HANDLE& handle) const;


	/**
	 * \brief pattern scans the text section and returns 0 if unsuccessful
	 * \param pattern the pattern
	 * \param mask the mask
	 * \param returnCSOffset in case your pattern leads to a xxx, cs:offset, it will return the address of the global variable instead
	 * \return the address
	 */
	ULONG64 PatternScan(const char* pattern, const std::string& mask, bool returnCSOffset = true);

	/**
	 * \brief closes the DMA and sets DMA_INITIALIZED to FALSE. Do not call on every object, only at the end of your program.
	 */
	static void CloseDMA();

	DWORD GetPidFromName(std::string process_name)
	{
		DWORD pid = 0;
		VMMDLL_PidGetFromName(DMA_HANDLE, (LPSTR)process_name.c_str(), &pid);
		return pid;
	}

	template <typename T>
	T Read(ULONG64 address, DWORD pid)
	{
		T buff{};
		DWORD size;
		VMMDLL_MemReadEx(DMA_HANDLE, pid, address, reinterpret_cast<PBYTE>(&buff), sizeof(T), &size, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
		return buff;
	}

	bool InitKeyboard()
	{
		//std::string win = QueryValue("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\CurrentBuild", e_registry_type::sz);
		//int Winver = 0;
		//if (!win.empty())
		//	Winver = std::stoi(win);
		//else
		//	return false;

		//VMMDLL_PidGetFromName(DMA_HANDLE, const_cast<char*>(std::string("winlogon.exe").c_str()), &this->win_logon_pid);

		//if (Winver > 22000)
		//{
		//	auto pids = GetPidListFromName("csrss.exe");


		//	for (size_t i = 0; i < pids.size(); i++)
		//	{
		//		auto pid = pids[i];
		//		uintptr_t tmp = VMMDLL_ProcessGetModuleBaseU(DMA_HANDLE, pid, const_cast<LPSTR>("win32ksgd.sys"));
		//		uintptr_t g_session_global_slots = tmp + 0x3110;
		//		DWORD size;
		//		uintptr_t addy1;
		//		VMMDLL_MemReadEx(DMA_HANDLE, pid, g_session_global_slots, reinterpret_cast<PBYTE>(&addy1), 8, &size, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
		//		uintptr_t addy2;
		//		VMMDLL_MemReadEx(DMA_HANDLE, pid, addy1, reinterpret_cast<PBYTE>(&addy2), 8, &size, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
		//		uintptr_t user_session_state;
		//		VMMDLL_MemReadEx(DMA_HANDLE, pid, addy2, reinterpret_cast<PBYTE>(&user_session_state), 8, &size, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
		//		gafAsyncKeyStateExport = user_session_state + 0x3690;
		//		if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
		//			break;
		//	}
		//	if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
		//		return true;
		//	return false;
		//}
		//else
		//{
		//	PVMMDLL_MAP_EAT eat_map = NULL;
		//	PVMMDLL_MAP_EATENTRY eat_map_entry;
		//	DWORD winlogpid;
		//	VMMDLL_PidGetFromName(DMA_HANDLE, const_cast<char*>(std::string("winlogon.exe").c_str()), &winlogpid);
		//	bool result = VMMDLL_Map_GetEATU(DMA_HANDLE, winlogpid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, const_cast<LPSTR>("win32kbase.sys"), &eat_map);
		//	if (!result)
		//		return false;

		//	if (eat_map->dwVersion != VMMDLL_MAP_EAT_VERSION)
		//	{
		//		VMMDLL_MemFree(eat_map);
		//		eat_map_entry = NULL;
		//		return false;
		//	}

		//	for (int i = 0; i < eat_map->cMap; i++)
		//	{
		//		eat_map_entry = eat_map->pMap + i;
		//		if (strcmp(eat_map_entry->uszFunction, "gafAsyncKeyState") == 0)
		//		{
		//			gafAsyncKeyStateExport = eat_map_entry->vaFunction;

		//			break;
		//		}
		//	}

		//	VMMDLL_MemFree(eat_map);
		//	eat_map = NULL;
		//	if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
		//		return true;
		//	return false;
		//}

		std::string win = QueryValue("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\CurrentBuild", e_registry_type::sz);
		int Winver = 0;
		if (!win.empty())
			Winver = std::stoi(win);
		else
			return false;
		std::string ubr = QueryValue("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\UBR", e_registry_type::dword);
		int Ubr = 0;
		if (!ubr.empty())
			Ubr = std::stoi(ubr);
		else
			return false;
		this->win_logon_pid = GetPidFromName("winlogon.exe");
		if (Winver > 22000)
		{
			auto pids = GetPidListFromName("csrss.exe");
			for (size_t i = 0; i < pids.size(); i++)
			{
				auto pid = pids[i];
				uintptr_t tmp = VMMDLL_ProcessGetModuleBaseU(DMA_HANDLE, pid, const_cast<LPSTR>("win32ksgd.sys"));
				uintptr_t g_session_global_slots;
				if (!tmp) {
					tmp = VMMDLL_ProcessGetModuleBaseU(DMA_HANDLE, pid, const_cast<LPSTR>("win32k.sys"));
					g_session_global_slots = tmp + 0x82538;
				}
				else {
					g_session_global_slots = tmp + 0x3110;
				}

				uintptr_t user_session_state = 0;
				for (int i = 0; i < 4; i++)
				{
					user_session_state = Read<uintptr_t>(Read<uintptr_t>(Read<uintptr_t>(g_session_global_slots, pid) + 8 * i, pid), pid);
					if (user_session_state > 0x7FFFFFFFFFFF)
						break;
				}

				if (Winver >= 26100) {
					gafAsyncKeyStateExport = user_session_state + (Ubr >= 2314 ? 0x3828 : 0x3820);
				}
				else if (Winver >= 22631 && Ubr >= 3810) {
					gafAsyncKeyStateExport = user_session_state + 0x36A8;
				}
				else {
					gafAsyncKeyStateExport = user_session_state + 0x3690;
				}

				if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF) break;
			}
			if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
				return true;
			return false;
		}
		else
		{
			PVMMDLL_MAP_EAT eat_map = NULL;
			PVMMDLL_MAP_EATENTRY eat_map_entry;
			bool result = VMMDLL_Map_GetEATU(DMA_HANDLE, GetPidFromName("winlogon.exe") | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, const_cast<LPSTR>("win32kbase.sys"), &eat_map);
			if (!result)
				return false;

			if (eat_map->dwVersion != VMMDLL_MAP_EAT_VERSION)
			{
				VMMDLL_MemFree(eat_map);
				eat_map_entry = NULL;
				return false;
			}

			for (int i = 0; i < eat_map->cMap; i++)
			{
				eat_map_entry = eat_map->pMap + i;
				if (strcmp(eat_map_entry->uszFunction, "gafAsyncKeyState") == 0)
				{
					gafAsyncKeyStateExport = eat_map_entry->vaFunction;

					break;
				}
			}

			VMMDLL_MemFree(eat_map);
			eat_map = NULL;
			if (gafAsyncKeyStateExport < 0x7FFFFFFFFFFF)
			{
				PVMMDLL_MAP_MODULEENTRY module_info;
				auto result = VMMDLL_Map_GetModuleFromNameW(DMA_HANDLE, GetPidFromName("winlogon.exe") | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, static_cast<LPCWSTR>(L"win32kbase.sys"), &module_info, VMMDLL_MODULE_FLAG_NORMAL);
				if (!result)
				{
					return false;
				}

				char str[32];
				if (!VMMDLL_PdbLoad(DMA_HANDLE, GetPidFromName("winlogon.exe") | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, module_info->vaBase, str))
				{
					return false;
				}

				uintptr_t gafAsyncKeyState;
				if (!VMMDLL_PdbSymbolAddress(DMA_HANDLE, str, const_cast<LPSTR>("gafAsyncKeyState"), &gafAsyncKeyState))
				{
					return false;
				}
			}
			if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
				return true;
			return false;
		}
	}

	void UpdateKeys()
	{
		uint8_t previous_key_state_bitmap[64] = { 0 };
		memcpy(previous_key_state_bitmap, state_bitmap, 64);

		VMMDLL_MemReadEx(DMA_HANDLE, this->win_logon_pid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, gafAsyncKeyStateExport, reinterpret_cast<PBYTE>(&state_bitmap), 64, NULL, VMMDLL_FLAG_NOCACHE);
		for (int vk = 0; vk < 256; ++vk)
			if ((state_bitmap[(vk * 2 / 8)] & 1 << vk % 4 * 2) && !(previous_key_state_bitmap[(vk * 2 / 8)] & 1 << vk % 4 * 2))
				previous_state_bitmap[vk / 8] |= 1 << vk % 8;
	}
	
	bool IsKeyDown(uint32_t virtual_key_code)
	{
		if (gafAsyncKeyStateExport < 0x7FFFFFFFFFFF)
			return false;
		return state_bitmap[(virtual_key_code * 2 / 8)] & 1 << virtual_key_code % 4 * 2;
	}

#if COUNT_TOTAL_READSIZE

	static DWORD64 GetTotalReadSize();

	static void ResetReadSize();

#endif
};

template <typename T>
class DMAScatter
{
	T value;
	void* address;
	DMAHandler* DMA;
	VMMDLL_SCATTER_HANDLE handle;
	bool unknown = true;

	void prepare()
	{
		DMA->queueScatterReadEx(handle, reinterpret_cast<uint64_t>(address), &value, sizeof(T));
	}
public:
	DMAScatter(DMAHandler* DMAHandler, VMMDLL_SCATTER_HANDLE handle, void* address)
		: address(address), DMA(DMAHandler), handle(handle)
	{
		if (!handle) DMAHandler::log("Invalid handle!");

		memset(&value, 0, sizeof(T));

		prepare();
	}

	DMAScatter(DMAHandler* DMAHandler, VMMDLL_SCATTER_HANDLE handle, uint64_t address)
		: address(reinterpret_cast<void*>(address)), DMA(DMAHandler), handle(handle)
	{
		if (!handle) DMAHandler::log("Invalid handle!");

		memset(&value, 0, sizeof(T));

		prepare();
	}


	T& operator*()
	{
		return value;
	}
};

inline DMAHandler mem;