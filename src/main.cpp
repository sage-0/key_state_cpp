#include <WiFi.h> //Wifi library
#include <esp_wpa2.h> //wpa2 library for connections to Enterprise networks
#include <WiFiClient.h>
#include <HTTPClient.h>

#include "env.h" // wifiパスワードなどの環境変数を読み込む。discordのトークンなどもここに書く。githubに上げないこと！！！
/* src/env.h
#include <string>

std::string userName = "<KOKADAI-WNETのユーザー名>";
std::string password = "<OKADAI-WNETのパスワード>";
std::string discordUrl = "<WebhookのURL>";
*/



#define EAP_IDENTITY "anonymous" //if connecting from another corporation, use
#define EAP_USERNAME userName.c_str()
#define EAP_PASSWORD  password.c_str() //your Eduroam password
const char* ssid = "KOKADAI-WNETg"; // (Eduroam) SSID
const char* host = "teu.ac.jp"; //external server domain for HTTP connection after authentification Example"arduino.php5.sk"

const unsigned key_state = 33; // センサーのピン番号
char state_text; // 状態のテキスト
bool current_state = NULL; // 現在の状態
bool last_state = NULL; // 最後の状態

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
    delay(1000);
    ESP.restart(); // エラーが発生した場合は再起動
  }
  Serial.println(WiFi.localIP()); // IPアドレスを表示
  digitalWrite(2, HIGH); // LEDを点灯
  return;
}

//セットアップ
void setup() {
  Serial.begin(115200); // シリアル通信を初期化
  pinMode(key_state, INPUT); // センサーからの入力を設定
  last_state = digitalRead(key_state); // 現在の状態で初期化
  pinMode(2, OUTPUT); // LEDを設定
  wifiConnect(); // WiFi接続
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){ // WiFi接続が切れた場合
    digitalWrite(2, LOW); // LEDを消灯
    wifiConnect(); // WiFi接続
  }

  current_state = digitalRead(key_state); // 現在の状態を取得
  HTTPClient http; // HTTPクライアントを初期化
  // 状態が変化した場合
  if (current_state != last_state) {
    last_state = current_state; // 最後の状態を更新
    String state_text = current_state ? "Opened" : "Closed"; // 状態のテキストを設定
    Serial.println("State: " + state_text); // 状態を表示

    if (WiFi.status() == WL_CONNECTED) { // WiFi接続を確認
      http.begin(discordUrl.c_str()); // HTTP接続を初期化
      http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // ヘッダーを追加
      String payload = "content=" + state_text; // ペイロードを設定
      int httpResponseCode = http.POST(payload); // POSTリクエストを送信
      Serial.println("HTTP Response code: " + String(httpResponseCode)); // HTTPレスポンスコードを表示
      // レスポンスコードが204の場合は成功
      if(httpResponseCode == 204){
        Serial.println("Success");
      } else { // 失敗した場合は再送信
        while (!http.connected())
        {
          http.POST(payload);
          delay(3000);
        }
      }
    }
  }
  http.end(); // HTTP接続を閉じる
  delay(10000); // 10秒待機
}