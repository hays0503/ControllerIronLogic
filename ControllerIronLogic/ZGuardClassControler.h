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
		TEXT("Open button"),				// Открыто кнопкой изнутри
		TEXT("Key not found"),				// Ключ не найден в банке ключей
		TEXT("Open key"),					// Ключ найден, дверь открыта
		TEXT("Key unscheduled"),			// Ключ найден, доступ не разрешен
		TEXT("Open RS485"),					// Открыто оператором по сети
		TEXT("Door is blocked (key)"),		// Ключ найден, дверь заблокирована
		TEXT("Door is blocked (button)"),	// Попытка открыть заблокированную дверь кнопкой
		TEXT("Door is broken"),				// Дверь взломана
		TEXT("Door is left open"),			// Дверь оставлена открытой (timeout)
		TEXT("Passage"),					// Проход состоялся
		TEXT("Sensor 1"),					// Сработал датчик 1
		TEXT("Sensor 2"),					// Сработал датчик 2
		TEXT("Controller is rebooted"),		// Перезагрузка контроллера
		TEXT("Button is disabled"),			// Заблокирована кнопка открывания
		TEXT("Double pass"),				// Попытка двойного прохода
		TEXT("Door opened"),				// Дверь открыта штатно
		TEXT("Door closed"),				// Дверь закрыта
		TEXT("Power off"),					// Пропало питание
		TEXT("Power is on (ElectroControl)"),	// Включение электропитания
		TEXT("Power is off (ElectroControl)"),	// Включение электропитания
		TEXT("Enable the lock (trigger)"),		// Включение замка (триггер)
		TEXT("Disable the lock (trigger)"),	// Отключение замка (триггер)
		TEXT("Changed state Mode"),			// Изменение состояния Режим
		TEXT("Changed state Fire"),			// Изменение состояния Пожара
		TEXT("Changed state Security"),		// Изменение состояния Охраны
		TEXT("Unknown key"),				// Неизвестный ключ
		TEXT("Passage in the gateway"),		// Совершен вход в шлюз
		TEXT("Blocked the entrance to the gateway (busy)"),		// Заблокирован вход в шлюз (занят)
		TEXT("Allowed to enter the gateway"),	// Разрешен вход в шлюз
		TEXT("AntiPassBack"),				// Заблокирован проход (Антипассбек)
		TEXT("Hotel40"),
		TEXT("Hotel41")
	};

	static LPCTSTR DirectStrs[] = {
		TEXT(""),
		TEXT("IN"),   // Вход
		TEXT("OUT")   // Выход
	};

	static LPCTSTR EcSubEvStrs[] = {
		TEXT(""),
		TEXT("CARD_DELAY"),			// Поднесена валидная карта с другой стороны (для входа) запущена задержка
		TEXT("RESERVED1"),			// (зарезервировано)
		TEXT("ON_NET"),				// Включено командой по сети
		TEXT("OFF_NET"),			// Выключено командой по сети
		TEXT("ON_SCHED"),			// Включено по временной зоне
		TEXT("OFF_SHED"),			// Выключено по временной зоне
		TEXT("CARD"),				// Поднесена валидная карта к контрольному устройству
		TEXT("RESERVED2"),			// (зарезервировано)
		TEXT("OFF_TIMEOUT"),		// Выключено после отработки таймаута
		TEXT("OFF_EXIT")			// Выключено по срабатыванию датчика выхода
	};

	static LPCTSTR FireSubEvStrs[] = {
		TEXT(""),
		TEXT("OFF_NET"),			// Выключено по сети
		TEXT("ON_NET"),				// Включено по сети
		TEXT("OFF_INPUT_F"),		// Выключено по входу FIRE
		TEXT("ON_INPUT_F"),			// Включено по входу FIRE
		TEXT("OFF_TEMP"),			// Выключено по датчику температуры
		TEXT("ON_TEMP")				// Включено по датчику температуры
	};

	static LPCTSTR SecurSubEvStrs[] = {
		TEXT(""),
		TEXT("OFF_NET"),			// Выключено по сети
		TEXT("ON_NET"),				// Включено по сети
		TEXT("OFF_INPUT_A"),		// Выключено по входу ALARM
		TEXT("ON_INPUT_A"),			// Включено по входу ALARM
		TEXT("OFF_TAMPERE"),		// Выключено по тамперу
		TEXT("ON_TAMPERE"),			// Включено по тамперу
		TEXT("OFF_DOOR"),			// Выключено по датчику двери
		TEXT("ON_DOOR")				// Включено по датчику двери
	};

	static LPCTSTR ModeSubEvStrs[] = {
		TEXT(""),
		TEXT("RS485 allow"),		// Установка командой по сети
		TEXT("RS485 denied"),		// Отказано оператору по сети
		TEXT("Timezone start"),			// Началась временная зона
		TEXT("Timezone finish"),			// Окончилась временная зона
		TEXT("Card allow"),			// Установка картой
		TEXT("Card denied")			// Отказано изменению картой
	};

	static LPCTSTR ModeStrs[] = {
		TEXT(""),
		TEXT("Norm"),				// Норма
		TEXT("Block"),				// Блокировка
		TEXT("Free"),				// Свободный
		TEXT("Wait")				// Ожидание
	};

	static LPCTSTR HModeStrs[] = {
		TEXT(""),
		TEXT("Norm"),				// Норма
		TEXT("Block"),				// Блокировка
		TEXT("Free"),				// Свободный
		TEXT("???")					// Зарезервировано
	};

	static LPCTSTR HotelSubEvStrs[] = {
		TEXT(""),
		TEXT("FreeCard"),			// Карта открытия
		TEXT("BlockCard"),			// Карта блокирующая
		TEXT("DopFunc"),			// Дополнительная функция
		TEXT("NewRCard"),			// создана резервная карта
		TEXT("Network"),
		TEXT("Timezone"),
		TEXT("Counter"),			// обновлен счетчик
		TEXT("CryptoKey"),			// обновлен криптоключ
		TEXT("Pulse Z"),			// измененение защелки в течении 2х секунд
		TEXT("State Change")		// состояние защелки -если нажали ручку и отпустили более чем через 2 секунды 
	};

	using namespace boost::json;

	//C помощью этой структуры прокидываем данные в поток
	//(только определение тут)
	struct dtoThread
	{
		HANDLE& g_hEvent;
		HANDLE& g_hCtr;
		BOOL& g_fThreadActive;
		INT& g_nCtrMaxEvents;
		json::array& ControlerEventsJson;//Cобытие из контролера пишеи сюда{JSON}
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

		json::array ControlerEventsJson;//Cобытие из контролера пишеи сюда{JSON}

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

		vector<int> DeleteCard(std::string stringKeyCart);



	};

#endif