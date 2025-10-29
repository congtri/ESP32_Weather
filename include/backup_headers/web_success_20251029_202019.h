#ifndef WEB_SUCCESS_H
#define WEB_SUCCESS_H

#include <Arduino.h>

const char SUCCESS_HTML[] PROGMEM = R"(
<!DOCTYPE html>
<html>
<head>
    <title>WiFi Configuration Saved</title>
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
        .success { 
            color: #4CAF50; 
            font-size: 18px; 
            margin-bottom: 20px; 
        }
        .countdown {
            font-size: 24px;
            font-weight: bold;
            color: #2196F3;
            margin: 20px 0;
        }
        .progress-bar {
            width: 100%;
            height: 10px;
            background-color: #e0e0e0;
            border-radius: 5px;
            overflow: hidden;
            margin: 20px 0;
        }
        .progress-fill {
            height: 100%;
            background-color: #4CAF50;
            width: 0%;
            transition: width 0.1s ease;
        }
        .details {
            background-color: #f8f9fa;
            padding: 15px;
            border-radius: 5px;
            margin: 20px 0;
            border-left: 4px solid #28a745;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>✅ Configuration Saved!</h1>
        
        <div class="success">
            WiFi credentials have been saved successfully.<br>
            The device will restart and connect to your network.
        </div>
        
        <div class="details">
            <p><strong>Saved SSID:</strong> {{SAVED_SSID}}</p>
            <p><strong>Status:</strong> Credentials stored in EEPROM</p>
        </div>
        
        <div class="countdown" id="countdown">3</div>
        
        <div class="progress-bar">
            <div class="progress-fill" id="progressFill"></div>
        </div>
        
        <p style="color: #666; font-size: 14px;">
            The device will automatically restart and attempt to connect to your network.<br>
            If connection fails, press the setup button during startup to reconfigure.
        </p>
    </div>

    <script>
        let countdown = 3;
        const countdownElement = document.getElementById('countdown');
        const progressFill = document.getElementById('progressFill');
        
        function updateCountdown() {
            countdownElement.textContent = countdown;
            progressFill.style.width = ((3 - countdown) / 3 * 100) + '%';
            
            if (countdown > 0) {
                countdown--;
                setTimeout(updateCountdown, 1000);
            } else {
                countdownElement.textContent = 'Restarting...';
                progressFill.style.width = '100%';
                window.location.href = '/restart';
            }
        }
        
        // Start countdown
        setTimeout(updateCountdown, 1000);
    </script>
</body>
</html>
)";

#endif