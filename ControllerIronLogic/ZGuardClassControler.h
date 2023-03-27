#pragma once

#ifndef ZGuardClassControler_H

#define ZGuardClassControler_H

	#include "pch.h"
	#include "AllHeader.h"

	std::string readKeyInBufferHex(Z_KEYNUM& pNum);

	class ZGuardClassController {

	private:
		CZGuardLoader* oZGL;
		const ZP_PORT_TYPE CvtPortType = ZP_PORT_COM;
		LPCWSTR CvtPortName = L"COM6";
		dtoThread* dto;
		CartDto* _PointCartDto;
		json::array ControlerEventsJson;//Cобытие из контролера пишеи сюда{JSON}
		bool isSendData;
		std::string LastKey;
		HANDLE hCvt = 0;

	public:
		
		vector<int> DeleteCard(std::string stringKeyCart);
		vector<_ZG_CTR_KEY>* KeyInController;
		vector<int>* EmptyKeyIndexInController;
		INT g_nCtrMaxEvents;
		BOOL g_fProximity;
		UINT g_nCtrFlags;
		BOOL g_fCtrNotifyEnabled;
		INT g_nAppReadEventIdx;
		//HANDLE g_hThread = NULL;		
		INT CtrAddr = NULL;
		_ZG_CTR_INFO rCtrInfo;
		HANDLE g_hCtr = NULL;
		HANDLE g_hEvent = NULL;
		BOOL g_fThreadActive;
		

		bool isSendDataGet() {
			return dto->isSendData;
		};

		std::string LastKeyGet() {
			return dto->LastKey;
		};

		std::string LastControllerEventsJson() {
			//Собираем массив событий
			json::object Event;
			Event.emplace("id", "123456");
			Event.emplace("operation", "events");
			Event.emplace("events", dto->ControlerEventsJson);
			//Собираем сообщение
			json::object MessagesJson;

			std::wstring wString;
			wString.append(&CtrTypeStrs[rCtrInfo.nType][0]);
			std::string convertedString(wString.begin(), wString.end());

			MessagesJson.emplace("type", convertedString);
			MessagesJson.emplace("sn", rCtrInfo.nSn);

			json::array Events;
			Events.emplace_back(Event);

			MessagesJson.emplace("messages", Events);
			//Делаем из json строку
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
			//Собираем вообще все событие
			//DoShowAllEvents();
			//DoShowNewEvents();

			//Собираем массив событий
			json::object Events;
			Events.emplace("id", "123456");
			Events.emplace("operation", "events");
			Events.emplace("events", dto->ControlerEventsJson);
			//Собираем сообщение
			json::object MessagesJson;
			MessagesJson.emplace("type", "Z5R");
			MessagesJson.emplace("sn", 99999);
			MessagesJson.emplace("messages", Events);
			//Делаем из json строку
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


		void ChangeMode(int Mode);
		bool readFunc(INT nPos, INT nMax, PVOID pUserData);
		ZGuardClassController(LPCWSTR _CvtPortName);
		~ZGuardClassController();
		value getControlerEventsJson();
		bool UpdateBankKey(int bank);
		void SetClock(PZG_CTR_CLOCK pClock);
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
		void ClearСurrentKeyBank(int Index, int Count);
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

	};

#endif