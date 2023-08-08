# ESP-Relay-Portal

WiFi реле на ESP-01/ESP-01s.

### Возможности: 
- Управление через web портал;
- Управление через MQTT;
- Интеграция с [HomeAssistant](https://www.home-assistant.io) через MQTT;
- Обновление прошивки через web портал;
- Поддержка управления реле как по высокому, так и по низкому уровню сигнала;
- Сохранение состояния реле при потере питания;
- Переход в режим точки доступа, при недоступности Wi-Fi указанного в настройках;
- Переход в режим клиента Wi-Fi, при доступности WiFi указанного в настройках;
- Поддержка таймеров.


### Интерфейс web портала:
Главный экран<br>
<img width="430" alt="image" src="https://github.com/mr-whitefoot/ESP-Relay-Portal/assets/16363451/e40898e2-9f7a-4e93-8c04-41359ba0cad9">

Поддержка таймеров<br>
<img width="389" alt="main" src="https://user-images.githubusercontent.com/16363451/254378974-68468eab-a7b1-488c-91c0-90d5e21f072c.png">

Выбор настроек<br>
<img width="401" alt="image" src="https://github.com/mr-whitefoot/ESP-Relay-Portal/assets/16363451/f3f8e971-b77e-4998-bc1a-49f401978e52">

Настройки реле<br>
<img width="389" alt="image" src="https://github.com/mr-whitefoot/ESP-Relay-Portal/assets/16363451/b9f3134a-b1c5-464a-b94d-77f0ce899cd9">

Обновление прошивки по Wi-Fi<br>
<img src="https://user-images.githubusercontent.com/16363451/197058992-d8bc1296-aa61-4ff9-ba36-1ad8a007244e.png" width="400">


### Первоначальная настройка:
- Подключиться к точке доступа relayAP;
- Откроется портал, если не откроется, перейти по адресу 192.168.4.1;
- Нажать кнопку Configuration;
- Нажать WiFi configuration;
- Указать данные вашего Wi-Fi: SSID, пароль;
- Нажать кнопку Save.
- При успешном соединении статус Wi-Fi сменится на зеленый, так же отобразится уровень сигнала и IP адрес. По истечению 3х минут точка доступа будет деактивирована. 
- Подключитесь к вашей точке доступа и перейдите на IP адрес из пункта выше. 


### Обновление прошивки через портал:
- Скачать [последнюю версию прошивки](https://github.com/mr-whitefoot/ESP-Relay-Portal-Arduino/releases/latest);
- Перейти на портале устройства Configuration -> Firmware upgrade;
- Нажать кнопку OTA firmware;
- Выбрать скачанный bin файл;
- Дождаться обновления страницы с сообщением Update Success! Rebooting...;
- Подождать не менее 25 секунд и нажать кнопку Home, для перехода на главную страницу;
- Проверить версию устройства в блоке Information.


