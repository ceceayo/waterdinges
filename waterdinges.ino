#include <arduino-timer.h>
#include <EspMQTTClient.h>
#include <ArduinoJson.h>
//#include <ESP_EEPROM.h>
#include "data.h"

#define sensornumber 1
#define state_topic "jesse/1/state"
#define discovery_topic_moist "homeassistant/sensor/plant_jesse_1_moisture/config"
#define discovery_topic_mark "homeassistant/sensor/plant_jesse_1_mark/config"
#define discovery_topic_alarm "homeassistant/sensor/plant_jesse_1_alarm/config"

EspMQTTClient mqtt(
  "ICT_Lyceum",
  "hoidoei123",
  "192.168.2.215",
  "Jessiewessie",
  "cutekittens22",
  "Jessiewessie"
);

Timer<10> timer; 

bool connected = false;

int mark;

bool wasButtonPressed = false;

void setup(void)
{
  //EEPROM.begin(16);
  Serial.begin(9600);

  //EEPROM.get(4, mark);

  pinMode(2, INPUT_PULLUP);
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

  // Discovery payload for moisture
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

  // Discovery payload for mark
  {
    DynamicJsonDocument doc(512);
    char buffer[256];

    doc["name"] = "Plant " + String(sensornumber) + " Mark";
    doc["stat_t"] = state_topic;
    doc["unit_of_meas"] = "%";
    doc["dev_cla"] = "humidity";
    doc["val_tpl"] = "{{ value_json.mark | default(0) }}";
    doc["uniq_id"] = "plant_" + String(sensornumber) + "_mark";
    doc["frc_upd"] = true;

    serializeJson(doc, buffer);
    mqtt.publish(discovery_topic_mark, buffer, true);
  }

  // Discovery payload for alarm
  {
    DynamicJsonDocument doc(512);
    char buffer[256];

    doc["name"] = "Plant " + String(sensornumber) + " Alarm";
    doc["stat_t"] = state_topic;
    doc["val_tpl"] = "{{value_json.alarm|default(\"off\")}}";
    doc["uniq_id"] = "plant_" + String(sensornumber) + "_alarm";
    doc["dev_cla"] = "binary_sensor";
    
    serializeJson(doc, buffer);
    mqtt.publish(discovery_topic_alarm, buffer, true);
  }

  // Start timers
  {
    timer.every(200, publish_data_to_mqtt);
    timer.every(75, check_button);
    timer.every(4000, run_alarm);
  }
}

float get_moisture(void)
{
  int moisture = analogRead(15);

  return map(moisture, 0, 1024, 0, 100);
}

float get_mark(void)
{
  return map(mark, 0, 1024, 0, 100);
}

bool get_alarm(float mark, float moisture)
{
  return moisture < mark;
}

bool check_button(void*)
{
  if (digitalRead(2) == LOW)
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
  return true;
}




bool publish_data_to_mqtt(void*)
{
  // Example values — replace these with actual sensor readings
  int mark_p = get_mark();
  int moisture_p = get_moisture();
  bool alarm = get_alarm(mark_p, moisture_p);


  DynamicJsonDocument doc(256);
  char buffer[128];

  doc["moisture"] = moisture_p;
  doc["mark"] = mark_p;
  doc["alarm"] = alarm? "on" : "off";

  size_t n = serializeJson(doc, buffer);
  mqtt.publish(state_topic, buffer, n);

  return true;
}

void connected_loop(void)
{
  timer.tick();
}


void set_mark(void)
{
  mark = analogRead(15);
  //EEPROM.put(4, mark);
  //boolean ok1 = EEPROM.commit();
  //if (ok1) Serial.println("+");
  //else Serial.println("-");
}

bool run_alarm(void)
{
  
  return true;
}