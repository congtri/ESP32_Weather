#ifndef WEB_RESTART_H
#define WEB_RESTART_H

#include <Arduino.h>

const char RESTART_HTML[] PROGMEM = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Device Restart</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body { 
            font-family: Arial, sans-serif; 
            margin: 40px; 
            background-color: #f0f0f0; 
            text-align: center; 
        }
        .container { 
            max-width: 400px; 
            margin: 0 auto; 
            background: white; 
            padding: 30px; 
            border-radius: 10px; 
            box-shadow: 0 0 10px rgba(0,0,0,0.1); 
        }
        .spinner {
            border: 4px solid #f3f3f3;
            border-top: 4px solid #3498db;
            border-radius: 50%;
            width: 50px;
            height: 50px;
            animation: spin 1s linear infinite;
            margin: 20px auto;
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        .status {
            color: #666;
            font-size: 16px;
            margin: 20px 0;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🔄 Restarting Device</h1>
        
        <div class="spinner"></div>
        
        <div class="status">
            The ESP32 Weather Station is restarting...<br>
            Please wait while the device applies your settings.
        </div>
        
        <p style="color: #666; font-size: 14px;">
            The device will now connect to your configured WiFi network.<br>
            If you need to reconfigure, press the setup button during startup.
        </p>
    </div>

    <script>
        // Auto-refresh every 10 seconds to check if device is back online
        setTimeout(function() {
            location.reload();
        }, 10000);
    </script>
</body>
</html>
)";

#endif