# ha_mqtt_rgb_light - Home-Assistant
A simple example to control a rgb led connected to a nodeMCU board (esp8266)

## Demo
<iframe title="Home-Assistant + MQTT + RGB light" width="480" height="390" src="http://www.youtube.com/watch?v=7rMpCZ9I2BU?autoplay=1" frameborder="0" allowfullscreen></iframe>

## Configuration
configuration.yaml :
```yaml
light:
  platform: mqtt
  name: "Office RGB light"
  state_topic: "office/rgb1/light/status"
  command_topic: "office/rgb1/light/switch"
  brightness_state_topic: "office/rgb1/brightness/status"
  brightness_command_topic: "office/rgb1/brightness/set"
  rgb_state_topic: "office/rgb1/rgb/status"
  rgb_command_topic: "office/rgb1/rgb/set"
  brightness_scale: 100
  optimistic: false
```