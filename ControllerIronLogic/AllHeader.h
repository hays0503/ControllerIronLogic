//#pragma once
#ifndef AllHeader_H
#define AllHeader_H
//
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
//#include <boost/beast/core.hpp>
//#include <boost/beast/http.hpp>
//#include <boost/beast/version.hpp>
//#include <boost/asio/dispatch.hpp>
//#include <boost/asio/strand.hpp>
//#include <boost/config.hpp>
//#include <boost/asio/ip/tcp.hpp>
//#include <boost/json.hpp>
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>
//#include <iostream>
//#include <algorithm>
//#include <cstdlib>
//#include <functional>
//#include <iostream>
//#include <memory>
//#include <string>
//#include <thread>
//#include <vector>
//#include <Windows.h>
//#include <conio.h>
//#include <stdio.h>
//#include <typeinfo>
//
//namespace beast = boost::beast;         // from <boost/beast.hpp>
//namespace http = beast::http;           // from <boost/beast/http.hpp>
//namespace net = boost::asio;            // from <boost/asio.hpp>
//using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//
//using namespace boost;             // from <boost/json.hpp>
//using namespace boost::json;             // from <boost/json.hpp>
//
//// Report a failure
//// Сообщить об ошибке
//void
//fail(beast::error_code ec, char const* what)
//{
//    std::cerr << what << ": " << ec.message() << "\n";
//}
//
//template <class Body, class Allocator>
//http::message_generator
//handle_requestCustom(
//    beast::string_view doc_root,
//    http::request<Body, http::basic_fields<Allocator>>&& req)
//{
//    // Returns a bad request response
//    // Возвращает неверный ответ на запрос
//    auto const bad_request =
//        [&req](beast::string_view why)
//    {
//        http::response<http::string_body> res{ http::status::bad_request, req.version() };
//        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//        res.set(http::field::content_type, "text/html");
//        res.keep_alive(req.keep_alive());
//        res.body() = std::string(why);
//        res.prepare_payload();
//        return res;
//    };
//
//    // Returns a not found response
//    // Возвращает не найденный ответ
//    auto const not_found =
//        [&req](beast::string_view target)
//    {
//        http::response<http::string_body> res{ http::status::not_found, req.version() };
//        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//        res.set(http::field::content_type, "text/html");
//        res.keep_alive(req.keep_alive());
//        res.body() = "The resource '" + std::string(target) + "' was not found.";
//        res.prepare_payload();
//        return res;
//    };
//
//    // Returns a server error response
//    // Возвращает ответ об ошибке сервера
//    auto const server_error =
//        [&req](beast::string_view what)
//    {
//        http::response<http::string_body> res{ http::status::internal_server_error, req.version() };
//        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//        res.set(http::field::content_type, "text/html");
//        res.keep_alive(req.keep_alive());
//        res.body() = "An error occurred: '" + std::string(what) + "'";
//        res.prepare_payload();
//        return res;
//    };
//
//    // Make sure we can handle the method
//    // Убедитесь, что мы можем обработать метод
//    if (req.method() != http::verb::get &&
//        req.method() != http::verb::head)
//        return bad_request("Unknown HTTP-method");
//
//    // Request path must be absolute and not contain "..".
//    // Путь запроса должен быть абсолютным и не содержать "..".
//    if (req.target().empty() ||
//        req.target()[0] != '/' ||
//        req.target().find("..") != beast::string_view::npos)
//        return bad_request("Illegal request-target");
//
//    beast::error_code ec;
//
//    // Handle an unknown error
//    // Обработка неизвестной ошибки
//    if (ec)
//        return server_error(ec.message());
//
//    // Respond to GET request
//    // Ответ на GET-запрос
//    http::response<http::string_body> res{ http::status::ok, req.version() };
//    std::string resultContent = "{ 'data':123 }";
//    auto const size = resultContent.size();
//    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//    res.set(http::field::content_type, beast::string_view("application/json"));
//    res.content_length(size);
//    res.body() = resultContent;
//    res.keep_alive(req.keep_alive());
//
//    return res;
//}
//
//
//
//
//
#endif
