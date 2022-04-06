
#include <pgmspace.h>
 
#define SECRET
 
const char WIFI_SSID[] = "Jake";               //TAMIM2.4G
const char WIFI_PASSWORD[] = "12345678";           //0544287380
 
#define THINGNAME "esp8266_10E_03"
#define DEVICE "esp8266_10E_03"
#define DEVICENUM 3

#define DJANGOKEY "T3nD1g1t_!"
 
int8_t TIME_ZONE = -5; //NYC(USA): -5 UTC
 
const char MQTT_HOST[] = "ayeaj5vqiepah-ats.iot.us-east-2.amazonaws.com";  

const char AWS_IOT_ENDPOINT[] = "ayeaj5vqiepah-ats.iot.us-east-2.amazonaws.com";  

// Amazon Root CA 1
static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5 
-----END CERTIFICATE-----
)EOF";
 
 
//Device Certificate 
// Copy contents from XXXXXXXX-certificate.pem.crt here ▼
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAM7CuAsxgp5paUk87O5ahT+l3HmqMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjAzMjEwNzA0
MzNaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDABCcb1tqs9IM/Ixgi
3/SHkFgXdGhD/TG/I0It6EnW3T//mB/yEIdpwV0HowpIGOeMKvmhljYmQ56jqed4
SJmqpofincVazjntRlCO/EBggevVku8l2au3to3tBF0ezXHkuNf8dwXiJOyD91ET
JHx7kUUTGbK4dPUqOAHrVH+8DduBZWJykKZxuXl3lWofW54pasKS4bKGWuNV1jMZ
CY1fJ6iA1glsm+vBebSxXuI0VVovC/yCaHsWMibHtXlixDxBm8OC9C0Vpf386ScY
FvBE9mWbVk7Uka1KDBNwpd6FxH88GhB8Vs8pApupg1yGC4iQQUytnEY8avqDYrXG
WvcZAgMBAAGjYDBeMB8GA1UdIwQYMBaAFLKNslmz5JcR2hpdPH50P7vSXuc+MB0G
A1UdDgQWBBRUN/AjEJbW/vMjddR4DSM7JOxaRDAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAD68jeS5m6pLZE9ZZpn1nV9Tj
UbQh0qxnYfS44q3RyL5L3x+5bLwAaYlVGFo105gpqW3px1JaN0OUeCD2u7Jvz4Mx
qsC03Wyb7MMOUhd/LH8/QhK1nxb9at4yw33RU8PcCxAk/BPyGU7CbzzmcjFcddAQ
UZ2NhLQSA1VHOcoWCMUtVI+ANNc2F7qYrQ2q/AuIjp+L6N/a0Sk1Vyqjh2SS2Agg
6bTXTPMzcsT9cdn9XcyfrCt5Jqwq5Xmmc6m4CCBjAm11Er63eEMdTOGXtrUkkEu6
WzwW4DtxeD3zBLHGdWzs733QtPgueVzeNaSljYVLJNsFswFuR/BmsB1oL1WBcQ==
-----END CERTIFICATE-----
 
)KEY";
 
// Device Private Key    
// Copy contents from  XXXXXXXX-private.pem.key here ▼
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAwAQnG9barPSDPyMYIt/0h5BYF3RoQ/0xvyNCLehJ1t0//5gf
8hCHacFdB6MKSBjnjCr5oZY2JkOeo6nneEiZqqaH4p3FWs457UZQjvxAYIHr1ZLv
Jdmrt7aN7QRdHs1x5LjX/HcF4iTsg/dREyR8e5FFExmyuHT1KjgB61R/vA3bgWVi
cpCmcbl5d5VqH1ueKWrCkuGyhlrjVdYzGQmNXyeogNYJbJvrwXm0sV7iNFVaLwv8
gmh7FjImx7V5YsQ8QZvDgvQtFaX9/OknGBbwRPZlm1ZO1JGtSgwTcKXehcR/PBoQ
fFbPKQKbqYNchguIkEFMrZxGPGr6g2K1xlr3GQIDAQABAoIBACxZiTLr6CwkeZ/+
mheMBK2/ECmysFZrx66vFR0HVu6ywZJmb5J4cN5jU/pslPZcEjcUwf2pdS5XVe/V
UWo90ohQ2HfmGuxDT6vXHso3xyLDP1ZgDq5+XSQcnSFObbesmg7LBtAAKbK29o3w
dKBoP/xGbF7iA56JHQs4TnEtXidv4WXod7SFg//Y2l8UPtfxJox2Tv82KlqU5Lur
L3Hr+FIC1hcxzGNBej8G5gy4/B+GBchY81QzPH9cHEfrbMTBM623wEUhh+O/XpL6
WFax9aIFJtEiwM8h7JHgUK86f4xcUiXfxsGrTyIxz8dxou6H2SqS7M8eE7wySFnO
6je1QDUCgYEA6MoNBoNOM5oyIqO73rAMDV6h3hiv5WRxTULif6gvDZjsCdZff9zs
jPJ+dqQ04EvTQiJKIefuZPE4dVIW1TaxhBBTlMXMFIPWT+TLZJBHeNa4WRkfwEye
Z9PgO3aaKJ63Kg1WBgdpv4LAuQayP8oUPo2PGqJk4AFuI0ZqqozbCncCgYEA0yle
lt+yVLcTn0fSv00l9AobP0UntknSMPr0dcbyTrIJfYR2lAds+0E4jD4xJaKXNkys
R4hJZsE73ByMUFGEabRPeND7KO+VxGmVbN6ZSovGVIQUqmeAEgj0RxBVO07yKcQI
vpTA8MayJiDW7HbIKPCmMsDfTmEs1A6f/5y/3u8CgYAmhnnNYCGrDJPLg8ltuzjj
ZYvoU7GYhxzxKmaNtenDmFFH94BqisdjNCaVv+s4S5Q017PpFJcxgXJUiDdbJegO
8RzD/FAf/QmUw0qd4/rQFsj/hdbpXkTA7iGn9aLhohKyFO7tpIBtv1fhxyfswqbD
C25pfAKxx8rQb7DrdNAzVQKBgQCbdMFmYZlwNLgFc2L741glqasVkVwDH6XW2WpP
qv+IYBoMhhCmOjFJaGKxHLgh9OlN2HSc1QD03R2slnfWSa0qBMzdLT06jee8id6w
H9DKyMxiPDramQf4Pw9XofadiylNBj1J0UmZ+S3+U/7mZyxEt6Yjh0re8TuHgpzY
GhZHkQKBgQCQJbYsWCE6ahE/YR6xmxfj7GlHTCxtyPkykWeHSQhmiOYwQGgmj9mj
6raymSxIN0hAm4atK1xi0XGxGZpc4QEpGczOTH0uQWFM18h2/8F8ccQKL+8BxaRV
FL6fvyIEqh5olsMV88m3+My3uH+X1cXQ730+dkZ4PhM8CgRiJYZedg==
-----END RSA PRIVATE KEY-----
 
)KEY";
