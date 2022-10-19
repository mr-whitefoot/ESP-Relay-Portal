# ESP-Relay-Portal-Arduino

WiFi реле на ESP-01/ESP-01s c возможностью управление через web, а так же через MQTT. Присутствует возможность обновления прошивки через web.


Главный экран<br>
<img src="https://user-images.githubusercontent.com/16363451/196786025-64c60964-c8f2-474a-921a-4b0dbea100df.jpg" width="400" alt="Главный экран">

Выбор настроек<br>
<img src="https://user-images.githubusercontent.com/16363451/196786045-36999196-129e-421d-b48d-7a00b394fab5.jpg" width="400">

Настройки реле<br>
<img src="https://user-images.githubusercontent.com/16363451/196786050-6f4aeb0a-c2ae-4253-ac1b-81ab8fa181ec.jpg" width="400">

Обновление прошивки по wifi<br>
<img src="https://user-images.githubusercontent.com/16363451/196786053-1dc90509-0d43-4537-bd3b-61cb8376c4b2.jpg" width="400">


### Первоначальная настройка:
- Подключиться к точке доступа relayAP
- Откроется портал, если не откроется, перейти по адресу 192.168.4.1
- Нажать кнопку Configuration
- Нажать WiFi configuration
- Указать данные вашего Wi-Fi, SSID, пароль;
- Нажать кнопку Save and reboot.

После перезагрузки устройства в адресе браузера перейти по адресу relay.local, либо посмотреть IP адрес на роутере.


### Обновление прошивки через портал:
- Скачать [последнюю версию прошивки](https://github.com/mr-whitefoot/ESP-Relay-Portal-Arduino/releases/latest)
- Перейти на портале устройства Configuration -> Firmware upgrade
- Нажать кнопку OTA firmware
- Выбрать скачанный bin файл
- Дождаться обновления страницы с сообщением Update Success! Rebooting...
- Подождать не менее 15 секунд и нажать кнопку Home, для перехода на главную страницу
- Проверить версию устройства в блоке Information


