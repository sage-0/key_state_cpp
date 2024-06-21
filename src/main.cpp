#include <WiFi.h> //Wifi library
#include <esp_wpa2.h> //wpa2 library for connections to Enterprise networks
#include <WiFiClient.h>
#include <HTTPClient.h>

//IDとかの定義
#include "env.h"

#define EAP_IDENTITY "anonymous" //if connecting from another corporation, use
#define EAP_USERNAME userName.c_str()
#define EAP_PASSWORD  password.c_str() //your Eduroam password
const char* ssid = "KOKADAI-WNETg"; // (Eduroam) SSID
const char* host = "teu.ac.jp"; //external server domain for HTTP connection after authentification Example"arduino.php5.sk"

const unsigned key_state = 33;
char state_text;
bool current_state = NULL;
bool last_state = NULL;

void wifiConnect(){
  //
  Serial.println();
  Serial.print("Connecting to network: ");
  Serial.println(ssid);
  //WiFi.disconnectはtrueを入れないと切ってくれない
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  //WiFi.modeは必須
  WiFi.mode(WIFI_STA); //init wifi mode
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    //provide identity
   esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_USERNAME, strlen(EAP_USERNAME));
    //provide username --> identity and username is same
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  //provide password
  esp_wifi_sta_wpa2_ent_enable();
  WiFi.begin(ssid); //connect to wifi
  delay(1000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);//after 30 seconds timeout - reset board
    ESP.restart();
  }
  Serial.println(WiFi.localIP());
  digitalWrite(2, HIGH);
  return;
}

//セットアップ
void setup() {
  Serial.begin(115200);
  pinMode(key_state, INPUT);
  last_state = digitalRead(key_state);
  pinMode(2, OUTPUT);
  wifiConnect();
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    digitalWrite(2, LOW);
    wifiConnect();
  }

  current_state = digitalRead(key_state);
  HTTPClient http; // HTTPクライアントを初期化
  // 状態が変化した場合
  if (current_state != last_state) {
    last_state = current_state;
    String state_text = current_state ? "Opened" : "Closed";
    Serial.println("State: " + state_text);

    if (WiFi.status() == WL_CONNECTED) { // WiFi接続を確認
      http.begin(discordUrl.c_str()); // HTTP接続を初期化
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      String payload = "content=" + state_text;
      int httpResponseCode = http.POST(payload); // POSTリクエストを送信
      Serial.println("HTTP Response code: " + String(httpResponseCode)); // HTTPレスポンスコードを表示

      if(httpResponseCode == 204){
        Serial.println("Success");
      } else {
        while (!http.connected())
        {
          http.POST(payload);
          delay(3000);
        }
      }
    }
  }
  http.end(); // HTTP接続を閉じる
  delay(10000);
}