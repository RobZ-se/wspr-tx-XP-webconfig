#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <WString.h>
#include <esp8266_web_settings.h>

// Forward declarations
static void wifi_setup();
static std::optional<DynamicJsonDocument> LoadConfig();

// Forward declarations for the three web_settings callbacks.

static void on_factory_reset(::grmcdorman::WebSettings &);
static void on_restart(::grmcdorman::WebSettings &);
static void on_save(::grmcdorman::WebSettings &);

///////////////////////////////////////////////////////////////////////////////////
// The WiFi settings.

// These macros allow declaration of the settings at the top level, outside of
// blocks (the F() macro is not allowed by the compiler in this context). The
// strings are in PROGMEM.
#define DECLARE_SETTING(name, text, type) \
static const char * PROGMEM name##_text = text; \
static const char * PROGMEM name##_id = #name; \
static ::grmcdorman::type name(FPSTR(name##_text), FPSTR(name##_id));

#define DECLARE_INFO_SETTING(name, text) DECLARE_SETTING(name, text, InfoSettingHtml)
#define DECLARE_STRING_SETTING(name, text) DECLARE_SETTING(name, text, StringSetting)
#define DECLARE_PASSWORD_SETTING(name, text) DECLARE_SETTING(name, text, PasswordSetting)
#define DECLARE_TOGGLE_SETTING(name, text) DECLARE_SETTING(name, text, ToggleSetting)
#define DECLARE_UNSIGNED_SETTING(name, text) DECLARE_SETTING(name, text, UnsignedIntegerSetting)

DECLARE_STRING_SETTING(hostname, "Name of this unit:(Hostname)");
DECLARE_STRING_SETTING(ssid, "Connect to this WiFi network:(WiFi Name)(SSID)");
DECLARE_PASSWORD_SETTING(password, "WiFi password");
DECLARE_TOGGLE_SETTING(use_dhcp, "Use DHCP (Set IP address automatic");
DECLARE_STRING_SETTING(ip_address, "IP address");
DECLARE_STRING_SETTING(subnet_mask, "Subnet mask");
DECLARE_STRING_SETTING(default_gateway, "Default gateway");
DECLARE_UNSIGNED_SETTING(connection_timeout, "Connection timeout (seconds)");
DECLARE_INFO_SETTING(rssi, "WiFi Signal strength");
DECLARE_INFO_SETTING(uptime, "Uptime"
    // This bit sets the periodic update to include both signal strength and uptime.
    "<script>periodicUpdateList.push(\"wifi_settings&setting=uptime&setting=rssi\");</script>");

// The list of settings.
static ::grmcdorman::SettingInterface::settings_list_t
        settings{&hostname, &ssid, &password, &use_dhcp, &ip_address, &subnet_mask, &default_gateway,
            &connection_timeout,
            &rssi, &uptime};

// Path to the config file
static const char config_path[] PROGMEM = "/config.json";

static grmcdorman::WebSettings web_settings;    //!< The settings web server. Default port (80).

// Misc variables
static bool factory_reset_next_loop = false;    //!< Set to true when a factory reset has been requested.
static bool restart_next_loop = false;          //!< Set to true when a simple reset has been requested.
static uint32_t restart_reset_when = 0;         //!< The time the factory reset/reset was requested.
static constexpr uint32_t restart_reset_delay = 2000;    //!< How long after the request for factory reset/reset to actually perform the function

static std::unique_ptr<DNSServer> dns_server;   //!< The DNS server for SoftAP mode.

void WiFi_init()
{
    //Serial.begin(115200);
    // Set defaults that differ from initial values.
    use_dhcp.set(true);
    connection_timeout.set(60);
    hostname.set("WSPR-TX-XP");

    // Arrange for RSSI fetch.
    rssi.set_request_callback([] (const ::grmcdorman::InfoSettingHtml &) {
        if (WiFi.RSSI() != 0)
        {
            // This creates a poor man's bar graph for the signal strength.
            // The values are, according to Google U, equivalent to the bars
            // on most phones.
            static constexpr size_t bar_sequence_size = sizeof("◾") - 1;
            static const char bars_string[] PROGMEM = "◾◾◾◾ ";
            static const char dbm[] PROGMEM = " dBm";
            auto signal = WiFi.RSSI();
            const char *bars;

            if (signal < -89 || signal == 0)
            {
                bars = &bars_string[4 * bar_sequence_size + 1]; // empty; also skip space.
            }
            else if (signal < -78)  // -78 to -88
            {
                bars = &bars_string[3 * bar_sequence_size]; // 1 bar
            }
            else if (signal < -67) // -67 to -77
            {
                bars = &bars_string[2 * bar_sequence_size]; // 2 bars
            }
            else if (signal < -56) // -56 to -66
            {
                bars = &bars_string[1 * bar_sequence_size]; // 3 bars
            }
            else // -55 or higher
            {
                bars = &bars_string[0];
            }

            // The 'message' array is sized to the maximum size of 'bars',
            // the size of the 'dbm' string, plus 4 characters for the signal value.
            // There will also be two extra characters from the null bytes on
            // `bars_string` and `dbm`.
            //
            // This C-style assembly - instead of concatenating String objects - is used
            // for memory efficiency, which matters on the ESP.
            char message[sizeof(bars_string) + sizeof(dbm) + 4];  // reserves about 4 characters for the signal value, which can only range from -128 to +127.
            strcpy_P(message, bars);
            itoa(signal, &message[strlen(message)], 10);
            strcat_P(message, dbm);
            rssi.set(message);
        }
        else
        {
            rssi.set("No signal, we are probably in soft AP mode.");
        }
    });
    // Arrange for uptime fetch.
    uptime.set_request_callback([] (const ::grmcdorman::InfoSettingHtml &) {
        auto now = millis();    // This wraps around after about 50 days. That's 1200 hours.
        auto hours = now / 1000 / 60 /60;
        auto minutes = now / 1000 / 60  % 60;
        auto seconds = now / 1000 % 60;
        char message[strlen("00000:00:00")]; // This allows hours up to 99999, which is larger than 1200 hours.
        snprintf(message, sizeof (message) -1, "%ld:%02ld:%02ld", hours, minutes, seconds);
        uptime.set(message);
    });

    // Load settings, if they exist.
    auto config = LoadConfig();
    if (config)
    {
        for (auto &setting: settings)
        {
            if (setting->is_persistable() && !(*config)[setting->name()].isNull())
            {
                setting->set_from_string((*config)[setting->name()]);
            }
        }
    }

    Serial.println("Starting with host name " + hostname.get());

    // Apply loaded WiFi settings
    wifi_setup();

    // Add our one tab to the web server.
    web_settings.add_setting_set(F("WiFi"), F("wifi_settings"), settings);

    // Add an extra handler.
    web_settings.get_server().on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
    });

    // Set up the web server and page.
    web_settings.setup(on_save, on_restart, on_factory_reset);
    // If we are connected to an AP, set credentials.
    if (WiFi.status() == WL_CONNECTED)
    {
        web_settings.set_credentials("admin", "password");
    }

    Serial.print(F("IP: "));
    Serial.println(WiFi.localIP().toString());

}

void web_check()
{
    // Handle DNS requests. This will make our captive portal work.
    if (dns_server)
    {
        dns_server->processNextRequest();
    }

    // Not presently essential; might be needed in future.
    web_settings.loop();

    if (factory_reset_next_loop && millis() - restart_reset_when > restart_reset_delay)
    {
        // Clear file system.
        LittleFS.format();
        // Erase configuration
        ESP.eraseConfig();
        // Reset (not reboot, that may save current state)
        ESP.reset();
    }

    if (restart_next_loop && millis() - restart_reset_when > restart_reset_delay)
    {
        ESP.restart();
    }
}

// Set up WiFi.
static void wifi_setup()
{
    String target_hostname(hostname.get().isEmpty()? "arbitrary name" : hostname.get());

    if (ip_address.get().isEmpty() ||
        subnet_mask.get().isEmpty())
    {
        use_dhcp.set(true);
    }

    if (!ssid.get().isEmpty())
    {
        Serial.print(F("Connecting to "));
        Serial.println(ssid.get());
        // Do not use persistent WiFi settings, we manage those ourselves.
        WiFi.persistent(false);
        if (!WiFi.mode(WIFI_STA))
        {
            Serial.println(F("Unable to set STA mode"));
        }
        if (!WiFi.hostname(target_hostname))
        {
            Serial.println(F("Unable to set host name"));
        }

        WiFi.begin(ssid.get(), password.get());

        if (use_dhcp.get())
        {
            if (!WiFi.config(0u, 0u, 0u))
            {
                Serial.println(F("Config for DHCP failed"));
            }
        }
        else
        {
            // Note: This may behave badly if the user hasn't set things correctly.
            // In particular, there's no verification that a valid IP address was
            // entered for any of these values; it will just silently use strange values.
            // Some of these may result in an unusable system that must be manually reset.
            IPAddress host_ip;
            IPAddress gateway_ip;
            IPAddress subnet_mask_ip;
            if (!host_ip.fromString(ip_address.get()))
            {
                Serial.print(F("Host IP address '"));
                Serial.print(ip_address.get());
                Serial.println(F("' could not be converted to IP."));
            }
            if (!subnet_mask_ip.fromString(subnet_mask.get()))
            {
                Serial.print(F("Subnet mask '"));
                Serial.print(subnet_mask.get());
                Serial.println(F("' could not be converted to IP."));
            }
            gateway_ip.fromString(default_gateway.get());

            WiFi.config(host_ip, gateway_ip, subnet_mask_ip, 0UL, 0UL);
        }

        // Loop continuously while WiFi is not connected
        unsigned int tries = 0;
        // Each poll is 100ms, so 10 polls = 1 second.
        auto status = WiFi.status();
        while (status != WL_CONNECTED && status != WL_CONNECT_FAILED && tries < connection_timeout.get() * 10)
        {
            // delay 100ms.
            delay(100);
            ++tries;
            status = WiFi.status();
        }
        if (status != WL_CONNECTED)
        {
            Serial.print(F("Unable to connect to the access point, status =") + String(status));
        }
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        // No SSID. Start in AP.
        Serial.println(F("Starting in AP mode"));
#ifdef ESP8266
// @bug workaround for bug #4372 https://github.com/esp8266/Arduino/issues/4372
        WiFi.enableAP(true);
        delay(500); // workaround delay
#endif
        WiFi.mode(WIFI_AP);
        // "target_hostname" is the SSID.
        WiFi.softAP(target_hostname);
        delay(500);
        Serial.print(F("Soft AP started at address "));
        Serial.println(WiFi.softAPIP().toString());
        dns_server = std::make_unique<DNSServer>();
        dns_server->setErrorReplyCode(DNSReplyCode::NoError);
        dns_server->start(53, "*", WiFi.softAPIP());
    }
}

static std::optional<DynamicJsonDocument> LoadConfig()
{
    if (!LittleFS.begin()) {
        return std::nullopt;
    }

    if (!LittleFS.exists(FPSTR(config_path))) {
        return std::nullopt;
    }

    File configFile = LittleFS.open(FPSTR(config_path), "r");

    if (!configFile) {
        return std::nullopt;
    }

    DynamicJsonDocument json(configFile.size() * 10);

    auto status = deserializeJson(json, configFile);
    if (DeserializationError::Ok != status) {
        Serial.print(F("Deserialization error: "));
        Serial.println(status.c_str());
        return std::nullopt;
    }
    return std::move(json);
}


static void on_factory_reset(::grmcdorman::WebSettings &)
{
    factory_reset_next_loop = true;
    restart_reset_when = millis();
}

static void on_restart(::grmcdorman::WebSettings &)
{
    restart_next_loop =-true;
    restart_reset_when = millis();
}

static void on_save(::grmcdorman::WebSettings &)
{
    Serial.println("Saving WiFi settings");
    DynamicJsonDocument json(4096);
    for (auto &setting: settings)
    {
        if (strlen_P(reinterpret_cast<const char *>(setting->name())) != 0 && setting->is_persistable())
        {
            json[setting->name()] = setting->as_string();
        }
    }

    File configFile = LittleFS.open(FPSTR(config_path), "w");
    if (!configFile) {
        return;
    }

    serializeJson(json, configFile);
    configFile.close();
}
