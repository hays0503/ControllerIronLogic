#pragma once

#ifndef ZGuardClassControler_H

#define ZGuardClassControler_H

	#include "pch.h"
	#include <boost/beast/core.hpp> 
	#include <boost/beast/http.hpp> 
	#include <boost/beast/version.hpp> 
	#include <boost/asio/ip/tcp.hpp>
	#include <boost/json.hpp>	
	#include <string>
	#include <iostream>
	#include <locale.h>
	#include "ZGuard.h"
	#include "ZPort.h"
	#include "Utils.h"

	#if !defined(ZGUARD_LINKONREQUEST)
	#pragma comment(lib, "./ZGuard.lib")
	#elif !defined(ZPORT_LINKONREQUEST)
	#pragma comment(lib, "./ZGuard.lib")
	#endif // !ZGUARD_LINKONREQUEST

	namespace beast = boost::beast;         // from <boost/beast.hpp>
	namespace http = beast::http;           // from <boost/beast/http.hpp>
	namespace net = boost::asio;            // from <boost/asio.hpp>
	using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

	using namespace boost;             // from <boost/json.hpp>
	using namespace boost::json;             // from <boost/json.hpp>

	static LPCTSTR EvTypeStrs[] = {
		TEXT(""),
		TEXT("Open button"),				// ������� ������� �������
		TEXT("Key not found"),				// ���� �� ������ � ����� ������
		TEXT("Open key"),					// ���� ������, ����� �������
		TEXT("Key unscheduled"),			// ���� ������, ������ �� ��������
		TEXT("Open RS485"),					// ������� ���������� �� ����
		TEXT("Door is blocked (key)"),		// ���� ������, ����� �������������
		TEXT("Door is blocked (button)"),	// ������� ������� ��������������� ����� �������
		TEXT("Door is broken"),				// ����� ��������
		TEXT("Door is left open"),			// ����� ��������� �������� (timeout)
		TEXT("Passage"),					// ������ ���������
		TEXT("Sensor 1"),					// �������� ������ 1
		TEXT("Sensor 2"),					// �������� ������ 2
		TEXT("Controller is rebooted"),		// ������������ �����������
		TEXT("Button is disabled"),			// ������������� ������ ����������
		TEXT("Double pass"),				// ������� �������� �������
		TEXT("Door opened"),				// ����� ������� ������
		TEXT("Door closed"),				// ����� �������
		TEXT("Power off"),					// ������� �������
		TEXT("Power is on (ElectroControl)"),	// ��������� ��������������
		TEXT("Power is off (ElectroControl)"),	// ��������� ��������������
		TEXT("Enable the lock (trigger)"),		// ��������� ����� (�������)
		TEXT("Disable the lock (trigger)"),	// ���������� ����� (�������)
		TEXT("Changed state Mode"),			// ��������� ��������� �����
		TEXT("Changed state Fire"),			// ��������� ��������� ������
		TEXT("Changed state Security"),		// ��������� ��������� ������
		TEXT("Unknown key"),				// ����������� ����
		TEXT("Passage in the gateway"),		// �������� ���� � ����
		TEXT("Blocked the entrance to the gateway (busy)"),		// ������������ ���� � ���� (�����)
		TEXT("Allowed to enter the gateway"),	// �������� ���� � ����
		TEXT("AntiPassBack"),				// ������������ ������ (�����������)
		TEXT("Hotel40"),
		TEXT("Hotel41")
	};

	static LPCTSTR DirectStrs[] = {
		TEXT(""),
		TEXT("IN"),   // ����
		TEXT("OUT")   // �����
	};

	static LPCTSTR EcSubEvStrs[] = {
		TEXT(""),
		TEXT("CARD_DELAY"),			// ��������� �������� ����� � ������ ������� (��� �����) �������� ��������
		TEXT("RESERVED1"),			// (���������������)
		TEXT("ON_NET"),				// �������� �������� �� ����
		TEXT("OFF_NET"),			// ��������� �������� �� ����
		TEXT("ON_SCHED"),			// �������� �� ��������� ����
		TEXT("OFF_SHED"),			// ��������� �� ��������� ����
		TEXT("CARD"),				// ��������� �������� ����� � ������������ ����������
		TEXT("RESERVED2"),			// (���������������)
		TEXT("OFF_TIMEOUT"),		// ��������� ����� ��������� ��������
		TEXT("OFF_EXIT")			// ��������� �� ������������ ������� ������
	};

	static LPCTSTR FireSubEvStrs[] = {
		TEXT(""),
		TEXT("OFF_NET"),			// ��������� �� ����
		TEXT("ON_NET"),				// �������� �� ����
		TEXT("OFF_INPUT_F"),		// ��������� �� ����� FIRE
		TEXT("ON_INPUT_F"),			// �������� �� ����� FIRE
		TEXT("OFF_TEMP"),			// ��������� �� ������� �����������
		TEXT("ON_TEMP")				// �������� �� ������� �����������
	};

	static LPCTSTR SecurSubEvStrs[] = {
		TEXT(""),
		TEXT("OFF_NET"),			// ��������� �� ����
		TEXT("ON_NET"),				// �������� �� ����
		TEXT("OFF_INPUT_A"),		// ��������� �� ����� ALARM
		TEXT("ON_INPUT_A"),			// �������� �� ����� ALARM
		TEXT("OFF_TAMPERE"),		// ��������� �� �������
		TEXT("ON_TAMPERE"),			// �������� �� �������
		TEXT("OFF_DOOR"),			// ��������� �� ������� �����
		TEXT("ON_DOOR")				// �������� �� ������� �����
	};

	static LPCTSTR ModeSubEvStrs[] = {
		TEXT(""),
		TEXT("RS485 allow"),		// ��������� �������� �� ����
		TEXT("RS485 denied"),		// �������� ��������� �� ����
		TEXT("Timezone start"),			// �������� ��������� ����
		TEXT("Timezone finish"),			// ���������� ��������� ����
		TEXT("Card allow"),			// ��������� ������
		TEXT("Card denied")			// �������� ��������� ������
	};

	static LPCTSTR ModeStrs[] = {
		TEXT(""),
		TEXT("Norm"),				// �����
		TEXT("Block"),				// ����������
		TEXT("Free"),				// ���������
		TEXT("Wait")				// ��������
	};

	static LPCTSTR HModeStrs[] = {
		TEXT(""),
		TEXT("Norm"),				// �����
		TEXT("Block"),				// ����������
		TEXT("Free"),				// ���������
		TEXT("???")					// ���������������
	};

	static LPCTSTR HotelSubEvStrs[] = {
		TEXT(""),
		TEXT("FreeCard"),			// ����� ��������
		TEXT("BlockCard"),			// ����� �����������
		TEXT("DopFunc"),			// �������������� �������
		TEXT("NewRCard"),			// ������� ��������� �����
		TEXT("Network"),
		TEXT("Timezone"),
		TEXT("Counter"),			// �������� �������
		TEXT("CryptoKey"),			// �������� ����������
		TEXT("Pulse Z"),			// ����������� ������� � ������� 2� ������
		TEXT("State Change")		// ��������� ������� -���� ������ ����� � ��������� ����� ��� ����� 2 ������� 
	};

	using namespace boost::json;

	//C ������� ���� ��������� ����������� ������ � �����
	//(������ ����������� ���)
	struct dtoThread
	{
		HANDLE& g_hEvent;
		HANDLE& g_hCtr;
		BOOL& g_fThreadActive;
		INT& g_nCtrMaxEvents;
		json::array& ControlerEventsJson;//C������ �� ���������� ����� ����{JSON}
		vector<_ZG_CTR_KEY>* KeyInController;
		vector<int>* EmptyKeyIndexInController;
		bool& isSendData;
		std::string& LastKey;
	};

	struct CartDto
	{
		vector<_ZG_CTR_KEY>* KeyInController;
		vector<int>* EmptyKeyIndexInController;
	};

	std::string readKeyInBufferHex(Z_KEYNUM& pNum);

	class ZGuardClassController {

	private:
		CZGuardLoader* oZGL;

		const ZP_PORT_TYPE CvtPortType = ZP_PORT_COM;

		LPCWSTR CvtPortName = L"COM6";

		dtoThread* dto;

		CartDto* _PointCartDto;

		json::array ControlerEventsJson;//C������ �� ���������� ����� ����{JSON}

		bool isSendData;

		std::string LastKey;

		HANDLE hCvt = 0;

	public:

		INT CtrAddr = NULL;

		_ZG_CTR_INFO rCtrInfo;

		vector<_ZG_CTR_KEY>* KeyInController;
		vector<int>* EmptyKeyIndexInController;



		bool isSendDataGet() {
			return dto->isSendData;
		};

		std::string LastKeyGet() {
			return dto->LastKey;
		};

		std::string LastControllerEventsJson() {
			//�������� ������ �������
			json::object Event;
			Event.emplace("id", "123456");
			Event.emplace("operation", "events");
			Event.emplace("events", dto->ControlerEventsJson);
			//�������� ���������
			json::object MessagesJson;

			std::wstring wString;
			wString.append(&CtrTypeStrs[rCtrInfo.nType][0]);
			std::string convertedString(wString.begin(), wString.end());

			MessagesJson.emplace("type", convertedString);
			MessagesJson.emplace("sn", rCtrInfo.nSn);

			json::array Events;
			Events.emplace_back(Event);

			MessagesJson.emplace("messages", Events);
			//������ �� json ������
			std::stringstream ss;
			ss << MessagesJson;
			std::string data(ss.str());			
			dto->ControlerEventsJson = {};
			if (!((!data.empty()) && (data != "{}") && (data != "null"))) {
				return "";
			}
			else {
				dto->ControlerEventsJson.clear();
				return data;
			}
			
		};

		std::string AllControllerEventsJson() {
			//�������� ������ ��� �������
			//DoShowAllEvents();
			//DoShowNewEvents();

			//�������� ������ �������
			json::object Events;
			Events.emplace("id", "123456");
			Events.emplace("operation", "events");
			Events.emplace("events", dto->ControlerEventsJson);
			//�������� ���������
			json::object MessagesJson;
			MessagesJson.emplace("type", "Z5R");
			MessagesJson.emplace("sn", 99999);
			MessagesJson.emplace("messages", Events);
			//������ �� json ������
			std::stringstream ss;
			ss << MessagesJson;
			std::string data(ss.str());
			dto->ControlerEventsJson = {};
			if (!((!data.empty()) && (data != "{}") && (data != "null"))) {
				return "";
			}
			else {
				dto->ControlerEventsJson.clear();
				return data;
			}

		};

		INT g_nCtrMaxEvents;
		BOOL g_fProximity;
		UINT g_nCtrFlags;
		BOOL g_fCtrNotifyEnabled;
		INT g_nAppReadEventIdx;	
		HANDLE g_hThread = NULL;

		HANDLE g_hCtr = NULL;
		HANDLE g_hEvent = NULL;
		BOOL g_fThreadActive;

		bool readFunc(INT nPos, INT nMax, PVOID pUserData);

		/*std::string readKeyInBuffer(Z_KEYNUM& pNum);

		

		std::string DecodeKeyInBufferHex(BYTE aData[8]);

		BOOL TryHexToDec(TCHAR chHex, INT* pDec);

		BOOL ParseKeyNum(Z_KEYNUM& rKeyNum, LPCTSTR pszText);

		bool readAllKeyFunc(INT nIdx, PZG_CTR_KEY pKey, INT nPos, INT nMax, PVOID pUserData);

		BOOL fnEnumProc(INT nIdx, PZG_CTR_EVENT pEvent, INT nPos, INT nMax, PVOID pUserData);*/

		ZGuardClassController(LPCWSTR _CvtPortName);

		value getControlerEventsJson();

		bool UpdateBankKey(int bank);

		bool Init();

		void ChangeContext(int _CtrAddr);

		int DoCtrEventsMenu(int _CtrAddr, int LastEventIndexInControler);

		void ShowEvents(int nStart, int nCount, dtoThread &dto);

		HRESULT CheckNotifyMsgs();

		static DWORD WINAPI staticNotifyThreadProc(LPVOID lpParameter);

		DWORD NotifyThreadProc(dtoThread dto);

		void StartNotifyThread();

		void StopNotifyThread();

		void ClearAllKey();

		void Clear�urrentKeyBank(int Index, int Count);

		int DoShowNewEvents(int Index);

		void getCard();

		void DoShowAllEvents();

		void EnableNotification(BOOL fEnable = TRUE, BOOL fReport = TRUE);

		void SetControllerMode(int mode);

		void SetTimeLock(DWORD nLetMs, DWORD nMaxMs, INT nBankN);

		void OpenDoor(int sideLock);

		bool AddCart(std::string stringKeyCart);
		bool AddCart(std::string stringKeyCart, vector<int> EmptyIndexCart);

		void DoRestoreFactorySettings();

		vector<int> DeleteCard(std::string stringKeyCart);



	};

#endif