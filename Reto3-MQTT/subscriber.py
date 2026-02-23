from datetime import datetime
import paho.mqtt.client as mqtt
import time
import argparse
import ssl

parser = argparse.ArgumentParser(description='IOT Reader')
parser.add_argument("--host", type=str,
                    default="35.231.39.19", help="MQTT Host")
parser.add_argument("--port", type=int,
                    default=8000, help="MQTT Port")
parser.add_argument("--user", type=str, default="admin", required=False, help="MQTT User")
parser.add_argument("--passwd", type=str, default="admin1234", required=False, help="MQTT Password")
parser.add_argument("--topic", type=str, default="/Colombia/Cundinamarca/Bogota/admin",
                    required=False, help="MQTT Topic")

args = parser.parse_args()

client = mqtt.Client("subscriber-test")
client.tls_set(ca_certs='ca.crt',
               tls_version=ssl.PROTOCOL_TLSv1_2, cert_reqs=ssl.CERT_NONE)

def on_message(client_msg: mqtt.Client, userdata, message: mqtt.MQTTMessage):
    log_date = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    print(log_date, "Data received: \n", message._topic.decode(
        "utf-8"), message.payload.decode("utf-8"))
    pass


def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed!")
    pass


def on_connect(client, userdata, flags, rc):
    if rc != 0:
        print("Failed to connect:", mqtt.connack_string(rc))
    else:
        print("Connected successfully")
        client.subscribe(args.topic, qos=1)

def on_error(client, userdata, rc):
    print("Connection failed!", rc)
    pass


def on_disconnect(client, userdata, rc):
    print("Disconnected!", mqtt.connack_string(rc))
    pass


def on_log(client, userdata, level, buf):
    print("Log: ", buf)
    pass


client.username_pw_set(args.user, args.passwd)
client.on_connect = on_connect
client.on_subscribe = on_subscribe
client.on_message = on_message
client.on_connect_fail = on_error
client.on_disconnect = on_disconnect
client.connect(args.host, args.port, 60)

time.sleep(3)
client.loop_forever()
