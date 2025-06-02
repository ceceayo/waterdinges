#include <EspMQTTClient.h>
#include <ArduinoJson.h>
#include <ESP_EEPROM.h>

#define sensornumber 1
#define state_topic "jesse/1/state"
#define discovery_topic_moist "homeassistant/sensor/plant_jesse_1_moisture/config"
#define discovery_topic_mark "homeassistant/sensor/plant_jesse_1_mark/config"

EspMQTTClient mqtt(
  "ICT_Lyceum",
  "hoidoei123",
  "192.168.2.215",
  "Jessiewessie",
  "cutekittens22",
  "Jessiewessie"
);

bool connected = false;

int mark;

bool wasButtonPressed = false;

void setup(void)
{
  EEPROM.begin(16);
  Serial.begin(9600);

  EEPROM.get(0, mark);

  pinMode(5, INPUT_PULLUP);
}

void loop(void)
{
  mqtt.loop();
  if (connected) connected_loop();
  else {Serial.print("."); delay(100);}

}

void onConnectionEstablished(void)
{
  Serial.print("\nConnected\n\n");
  connected = true;

  // Discovery payload for temperature
  {
    DynamicJsonDocument doc(512);
    char buffer[256];

    doc["name"] = "Plant " + String(sensornumber) + " Moisture";
    doc["stat_t"] = state_topic;
    doc["unit_of_meas"] = "%";
    doc["dev_cla"] = "humidity";
    doc["val_tpl"] = "{{ value_json.moisture | default(0) }}";
    doc["uniq_id"] = "plant_" + String(sensornumber) + "_moisture";
    doc["frc_upd"] = true;

    serializeJson(doc, buffer);
    mqtt.publish(discovery_topic_moist, buffer, true);
  }

  // Discovery payload for moisture
  {
    DynamicJsonDocument doc(512);
    char buffer[256];

    doc["name"] = "Plant " + String(sensornumber) + " Mark";
    doc["stat_t"] = state_topic;
    doc["unit_of_meas"] = "%";
    doc["dev_cla"] = "humidity";
    doc["val_tpl"] = "{{ value_json.moisture | default(0) }}";
    doc["uniq_id"] = "plant_" + String(sensornumber) + "_mark";
    doc["frc_upd"] = true;

    serializeJson(doc, buffer);
    mqtt.publish(discovery_topic_mark, buffer, true);
  }
}

float get_moisture(void)
{
  int moisture = analogRead(A0);

  return map(moisture, 0, 1024, 0, 100);
}

float get_mark(void)
{
  return map(mark, 0, 1024, 0, 100);
}

void check_button(void)
{
  if (digitalRead(5) == LOW)
  {
    if (wasButtonPressed){}
    else
    {
      wasButtonPressed = true;
      Serial.print("!");
      set_mark();
    }
  }
  else wasButtonPressed = false;
}

void wait(void)
{
  for (int i = 0; i != 50; i++)
  {
    delay(10);
    check_button();
  }
}

void connected_loop(void)
{
  // Example values — replace these with actual sensor readings
  int mark_p = get_mark();
  int moisture_p = get_moisture();



  DynamicJsonDocument doc(256);
  char buffer[128];

  doc["moisture"] = moisture_p;
  doc["mark"] = mark_p;

  size_t n = serializeJson(doc, buffer);
  mqtt.publish(state_topic, buffer, n);

  wait();
}

void set_mark(void)
{
  mark = analogRead(A0);
  EEPROM.put(0, mark);
  boolean ok1 = EEPROM.commit();
  if (ok1) Serial.println("+");
  else Serial.println("-");
}