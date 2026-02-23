from datetime import datetime
import paho.mqtt.client as mqtt
import argparse
import ssl
import threading
import time

import sys
print(f"GIL enabled: {sys._is_gil_enabled()}")

parser = argparse.ArgumentParser(description='IOT Sensor Emulator')
parser.add_argument("--host", type=str,
                    default="35.231.39.19", help="MQTT Host")
parser.add_argument("--port", type=int,
                    default=8000, help="MQTT Port")
parser.add_argument("--user", type=str, default="admin", required=False, help="MQTT User")
parser.add_argument("--passwd", type=str, default="admin1234", required=False, help="MQTT Password")
parser.add_argument("--topic", type=str, default="/Colombia/Cundinamarca/Bogota/admin",
                    required=False, help="MQTT Topic")

args = parser.parse_args()

def send_messages(client):
    topic = args.topic
    message = "MQTT Test"
    res = client.publish(topic, message)
    log_date = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    print(log_date, topic + ": " + message)
    print("\tMsg:", mqtt.connack_string(res[0]))


def on_publish(client, userdata, result):
    print("Publish successful!")
    pass


def on_connect(client, userdata, flags, rc):
    print("Connected: ", rc)
    pass


def on_error(client, userdata, rc):
    print("Connection failed!", rc)
    pass


def on_disconnect(client, userdata, rc):
    print("Disconnected!", mqtt.connack_string(rc))
    pass


def on_log(client, userdata, level, buf):
    print("Log: ", buf)
    pass

def test_clients(N):
    # Basado en: https://docs.python.org/es/3/library/threading.html
    # Start threads for each link
    threads = []
    for i in range(N):
        # Using `args` to pass positional arguments and `kwargs` for keyword arguments
        client = mqtt.Client(f"Pub-test-{i}")
        client.tls_set(ca_certs='ca.crt',
                    tls_version=ssl.PROTOCOL_TLSv1_2, cert_reqs=ssl.CERT_NONE)
        client.username_pw_set(args.user, args.passwd)
        client.on_connect = on_connect
        client.on_publish = on_publish
        client.on_connect_fail = on_error
        client.on_disconnect = on_disconnect
        client.connect(args.host, args.port, 60)
        t = threading.Thread(target=send_messages, args=(client,))
        threads.append(t)

    # Start each thread
    for t in threads:
        t.start()

    # Wait for all threads to finish
    for t in threads:
        t.join()

numThreads = [3, 30, 90, 270, 900, 2700]
for num in numThreads:
    start = time.time()
    print(f"XXXXXXXXXXXXX Testing with {num} clients")
    test_clients(num)
    end = time.time()
    print(f"XXXXXXXXXXXXX Time taken: {end - start} seconds")


