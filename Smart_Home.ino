

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

const char* ssid = "12345";
const char* password = "987654321";
const char* botToken = "6731230768:AAGSw6qQbex0ETbKGqU2yuAn69k98E_G5V4";
long chatID = 1133662442;  // Replace with your actual chat ID

#define BOT_MESSAGE_OPEN "🆘🆘🆘🆘🆘🆘🆘🆘🆘🆘\n\n⚠️ SMART HOME ⚠️"
#define BOT_MESSAGE_CLOSE "❇️ SMART HOME APPLIANCES ✅"
#define LED_PIN D1  // Change this to the pin where your LED is connected
#define FAN_PIN D0
#define LOCK_PIN D2
const int BUTTON = 0;
bool door_close = false;

bool breq_flag = false;
bool buzz_flag = false;
const int BUZZ_PIN = 10;

const unsigned long BOT_MTBS = 1000;

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(botToken, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

void handleNewMessages(int numNewMessages)
{
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    if (from_name == "")
      from_name = "Guest";

    if (text == "/on" && chat_id == String(chatID))
    {
      digitalWrite(LED_PIN, HIGH); // Turn off the LED
      bot.sendMessage(String(chatID), "LED turned on");
    }
    if (text == "/off" && chat_id == String(chatID))
    {
      digitalWrite(LED_PIN, LOW); // Turn on the LED
      bot.sendMessage(String(chatID), "LED turned off");
    }

    if (text == "/fan_On" && chat_id == String(chatID))
    {
      digitalWrite(FAN_PIN, HIGH); // Turn off the LED
      bot.sendMessage(String(chatID), "Fan turned on");
    }
    if (text == "/fan_Off" && chat_id == String(chatID))
    {
      digitalWrite(FAN_PIN, LOW); // Turn on the LED
      bot.sendMessage(String(chatID), "Fan turned off");
    }
    if (text == "/Close_lock" && chat_id == String(chatID))
    {
      digitalWrite(Lock_pin, HIGH); // Turn off the LED
      bot.sendMessage(String(chatID), "Lock_Closed");
    }
    if (text == "/Open_lock" && chat_id == String(chatID))
    {
      digitalWrite(FAN_PIN, LOW); // Turn on the LED
      bot.sendMessage(String(chatID), "Lock_opened");
    }
  }
}

void setup()
{
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LOCK_PIN, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  
  Serial.begin(115200);

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  IPAddress localIP = WiFi.localIP();
  String ip_address = localIP.toString();
  Serial.println(ip_address);

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot.sendMessage(String(chatID), "𝐈𝐧𝐢𝐭𝐢𝐚𝐭𝐢𝐧𝐠 Smart Home \n\n💠 Trigger Mode : BOOT\n\nBot Started...\n\nBOT  ✅\n\n🌐 IP address : " + ip_address + "", "");
}

void buzz_ntfy(bool bflag)
{
  if (breq_flag)
  {
    if (bflag)
    {
      digitalWrite(BUZZ_PIN, HIGH);
    }
    else
    {
      digitalWrite(BUZZ_PIN, LOW);
    }
  }
}

void door_open()
{
  bot.sendMessage(String(chatID), BOT_MESSAGE_OPEN);
  door_close = false;
  buzz_ntfy(true);
}

void door_closed()
{
  bot.sendMessage(String(chatID), BOT_MESSAGE_CLOSE);
  door_close = true;
  buzz_ntfy(false);
}

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
