void WIFIAP_Client() {
  WiFi.mode(WIFI_STA);
    byte tries = 11;
    WiFi.begin(_ssid.c_str(), _password.c_str());
    while (--tries && WiFi.status() != WL_CONNECTED)
    {
      //Мигаем сетодиодом при попытке подключится к роутеру
      digitalWrite(led, HIGH);
      Serial.print(".");
      delay(500);
      digitalWrite(led, LOW);
      delay(500);
    }
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
