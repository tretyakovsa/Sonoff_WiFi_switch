void WIFIAP_Client() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid.c_str(), _password.c_str());
  tries(11);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WL_CONNECTED);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  if (WiFi.status() != WL_CONNECTED)
  {
    StartAPMode();
  }
}

bool StartAPMode()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(_ssidAP.c_str(), _passwordAP.c_str());
  dnsServer.start(DNS_PORT, "*", apIP);
  //Зажигаем светодиод если находимся в режиме AP
  digitalWrite(led, HIGH);
  return true;
}

bool RestartWiFi() {
  //Холодный перезапуск WiFi при первой настройке
  Serial.println("WiFi reconnect");
  // Не отключаясь точки доступа подключаемся к роутеру для получения будущего IP
  WiFi.mode(WIFI_AP_STA );
  WiFi.begin(_ssid.c_str(), _password.c_str());
  delay(1000);
  tries(30);

  Serial.println("");
  Serial.println(WiFi.localIP());
  //Грузим страницу редиректа
  //handleFileRead("/connect.htm");
/*
  String refresh = "<html><head><meta http-equiv=\"refresh\" content=\"10;http://";
  refresh += WiFi.localIP().toString();
  refresh += "\"></head></html>";
  HTTP.send(200, "text/html", refresh);
  */
  HTTP.send(200, "text/html", "ОК");
  // Отключаем точку доступа и переподключаемся к роутеру
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  tries(11);
}

// Попытки подключения к сети
void tries(byte tries) {
  while (--tries && WiFi.status() != WL_CONNECTED)
  {
    //Мигаем сетодиодом при попытке подключится к роутеру
    digitalWrite(led, HIGH);
    Serial.print(".");
    delay(500);
    digitalWrite(led, LOW);
    delay(500);

  }
}

