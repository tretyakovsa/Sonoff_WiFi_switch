void WIFIAP_Client() {
  Serial.println("");
  Serial.println(_setAP);
  WiFi.mode(WIFI_STA);
    byte tries = 11;
    WiFi.begin(_ssid.c_str(), _password.c_str());
    Serial.println("start in STA mode");
    while (--tries && WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(1000);
    }

    if (WiFi.status() != WL_CONNECTED)
    {
      StartAPMode();
    }
  }

bool StartAPMode()
{
  Serial.println("start in AP mode");
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(_ssidAP.c_str(), _passwordAP.c_str());
  dnsServer.start(DNS_PORT, "*", apIP);
  return true;
}
