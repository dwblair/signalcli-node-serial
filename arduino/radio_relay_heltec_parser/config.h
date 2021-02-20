const char *SSID = "pvosPi";
const char *WiFiPassword = "12345678";
const char* bayou_base_url = "192.168.4.1:3000/co2/data";
const char* feed_pubkey = "62be3698ed7917a7da588750de28d0ac2030c1c12a5c1c6d";
const char* feed_privkey = "f02d003674065fb2e9199bf39d602532324fdc1d5a041ab4";
char node_shortkeys[5][4]=
{
    "75f",
    "a3f",
    "a1f",
    "3af",
    "23f"
};
const int interval_sec = 10;
const int forcePPM = 400;
