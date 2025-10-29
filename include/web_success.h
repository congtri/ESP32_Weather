#ifndef WEB_SUCCESS_H
#define WEB_SUCCESS_H

#include <Arduino.h>

// Auto-generated header file from web/success.html
// Generated on: Wed Oct 29 20:22:50 +07 2025
// Script: html_to_headers.sh

const char SUCCESS_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Configuration Successful</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 15px;
            box-shadow: 0 15px 35px rgba(0,0,0,0.1);
            padding: 40px;
            width: 100%;
            max-width: 500px;
            text-align: center;
            animation: bounceIn 0.8s ease-out;
        }
        @keyframes bounceIn {
            0% {
                opacity: 0;
                transform: scale(0.3);
            }
            50% {
                opacity: 1;
                transform: scale(1.05);
            }
            70% {
                transform: scale(0.9);
            }
            100% {
                opacity: 1;
                transform: scale(1);
            }
        }
        .success-icon {
            font-size: 80px;
            color: #28a745;
            margin-bottom: 20px;
            animation: pulse 2s infinite;
        }
        @keyframes pulse {
            0% {
                transform: scale(1);
            }
            50% {
                transform: scale(1.1);
            }
            100% {
                transform: scale(1);
            }
        }
        h1 {
            color: #333;
            font-size: 32px;
            margin-bottom: 15px;
        }
        .message {
            color: #666;
            font-size: 18px;
            margin-bottom: 30px;
            line-height: 1.6;
        }
        .countdown {
            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
            color: white;
            padding: 20px;
            border-radius: 10px;
            margin-bottom: 20px;
        }
        .countdown-title {
            font-size: 18px;
            margin-bottom: 10px;
        }
        .countdown-timer {
            font-size: 36px;
            font-weight: bold;
            margin-bottom: 10px;
        }
        .progress-bar {
            width: 100%;
            height: 8px;
            background-color: rgba(255,255,255,0.3);
            border-radius: 4px;
            overflow: hidden;
        }
        .progress-fill {
            height: 100%;
            background-color: white;
            width: 100%;
            border-radius: 4px;
            animation: progressCountdown 10s linear;
        }
        @keyframes progressCountdown {
            from {
                width: 100%;
            }
            to {
                width: 0%;
            }
        }
        .info-box {
            background-color: #f8f9fa;
            border-left: 4px solid #28a745;
            padding: 20px;
            border-radius: 0 8px 8px 0;
            text-align: left;
        }
        .info-title {
            font-weight: 600;
            color: #333;
            margin-bottom: 10px;
            font-size: 16px;
        }
        .info-text {
            color: #666;
            font-size: 14px;
            line-height: 1.5;
        }
        .status-details {
            background-color: #e7f5e7;
            border: 1px solid #28a745;
            border-radius: 8px;
            padding: 15px;
            margin-bottom: 20px;
        }
        .status-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 8px;
        }
        .status-item:last-child {
            margin-bottom: 0;
        }
        .status-label {
            color: #333;
            font-weight: 500;
        }
        .status-value {
            color: #28a745;
            font-weight: 600;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="success-icon">✅</div>
        <h1>Configuration Successful!</h1>
        <p class="message">
            Your ESP32 Weather Station has been configured successfully.<br>
            The device will now restart and connect to your WiFi network.
        </p>
        
        <div class="status-details">
            <div class="status-item">
                <span class="status-label">WiFi Network:</span>
                <span class="status-value" id="wifiNetwork">Loading...</span>
            </div>
            <div class="status-item">
                <span class="status-label">Device Status:</span>
                <span class="status-value">Configured ✓</span>
            </div>
            <div class="status-item">
                <span class="status-label">Next Step:</span>
                <span class="status-value">Automatic Restart</span>
            </div>
        </div>
        
        <div class="countdown">
            <div class="countdown-title">Device restarting in:</div>
            <div class="countdown-timer" id="countdown">10</div>
            <div class="progress-bar">
                <div class="progress-fill"></div>
            </div>
        </div>
        
        <div class="info-box">
            <div class="info-title">🚀 What happens next?</div>
            <div class="info-text">
                • The device will restart automatically<br>
                • It will connect to your WiFi network<br>
                • Weather data collection will begin<br>
                • You can find the device on your local network<br>
                • Setup mode will be disabled
            </div>
        </div>
    </div>

    <script>
        // Get WiFi network from URL parameters
        const urlParams = new URLSearchParams(window.location.search);
        const wifiNetwork = urlParams.get('ssid') || 'Your Network';
        document.getElementById('wifiNetwork').textContent = wifiNetwork;
        
        // Countdown timer
        let countdownValue = 10;
        const countdownElement = document.getElementById('countdown');
        
        const countdownInterval = setInterval(() => {
            countdownValue--;
            countdownElement.textContent = countdownValue;
            
            if (countdownValue <= 0) {
                clearInterval(countdownInterval);
                countdownElement.textContent = '0';
                // Optionally redirect or show completion message
                setTimeout(() => {
                    window.location.href = '/restart';
                }, 500);
            }
        }, 1000);
        
        // Optional: Add some sparkle effects
        function createSparkle() {
            const sparkle = document.createElement('div');
            sparkle.innerHTML = '✨';
            sparkle.style.position = 'fixed';
            sparkle.style.left = Math.random() * window.innerWidth + 'px';
            sparkle.style.top = Math.random() * window.innerHeight + 'px';
            sparkle.style.fontSize = '20px';
            sparkle.style.pointerEvents = 'none';
            sparkle.style.animation = 'sparkleFloat 3s ease-out forwards';
            document.body.appendChild(sparkle);
            
            setTimeout(() => {
                sparkle.remove();
            }, 3000);
        }
        
        // Add sparkle animation CSS
        const style = document.createElement('style');
        style.textContent = `
            @keyframes sparkleFloat {
                0% {
                    opacity: 1;
                    transform: translateY(0) rotate(0deg);
                }
                100% {
                    opacity: 0;
                    transform: translateY(-100px) rotate(360deg);
                }
            }
        `;
        document.head.appendChild(style);
        
        // Create sparkles periodically
        setInterval(createSparkle, 800);
    </script>
</body>
</html>)rawliteral";

#endif // WEB_SUCCESS_H
