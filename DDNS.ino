int ip_wan() {
  HTTPClient http;
http.begin(DDNS); //HTTP

        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
         Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == 302) {
                String payload = http.getString();
         Serial.println(payload);
            }
        } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }



