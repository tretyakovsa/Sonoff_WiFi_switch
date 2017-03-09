void Movement_init() {
  HTTP.on("/pir", handle_pir);        // Установить время на которое будет срабатывать датчик движения
}

// Установить время на которое будет срабатывать датчик движения
void handle_pir() {
  pirTime = HTTP.arg("t").toInt();
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}
