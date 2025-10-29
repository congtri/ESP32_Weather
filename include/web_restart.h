#ifndef WEB_RESTART_H
#define WEB_RESTART_H

#include <Arduino.h>

// Auto-generated header file from web/restart.html
// Generated on: Wed Oct 29 20:22:50 +07 2025
// Script: html_to_headers.sh

const char RESTART_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Device Restarting</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
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
            animation: fadeIn 0.6s ease-out;
        }
        @keyframes fadeIn {
            from {
                opacity: 0;
                transform: translateY(20px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
        .loading-spinner {
            width: 80px;
            height: 80px;
            border: 8px solid #f3f3f3;
            border-top: 8px solid #667eea;
            border-radius: 50%;
            margin: 0 auto 30px;
            animation: spin 1s linear infinite;
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        h1 {
            color: #333;
            font-size: 28px;
            margin-bottom: 15px;
        }
        .message {
            color: #666;
            font-size: 18px;
            margin-bottom: 30px;
            line-height: 1.6;
        }
        .status-box {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 25px;
            border-radius: 10px;
            margin-bottom: 25px;
        }
        .status-title {
            font-size: 20px;
            font-weight: 600;
            margin-bottom: 10px;
        }
        .status-text {
            font-size: 16px;
            opacity: 0.9;
        }
        .progress-steps {
            background-color: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 20px;
        }
        .step {
            display: flex;
            align-items: center;
            margin-bottom: 12px;
            font-size: 14px;
        }
        .step:last-child {
            margin-bottom: 0;
        }
        .step-icon {
            width: 24px;
            height: 24px;
            border-radius: 50%;
            display: flex;
            align-items: center;
            justify-content: center;
            margin-right: 15px;
            font-size: 12px;
        }
        .step-completed {
            background-color: #28a745;
            color: white;
        }
        .step-current {
            background-color: #667eea;
            color: white;
            animation: pulse 1.5s infinite;
        }
        .step-pending {
            background-color: #e9ecef;
            color: #6c757d;
        }
        @keyframes pulse {
            0% {
                box-shadow: 0 0 0 0 rgba(102, 126, 234, 0.7);
            }
            70% {
                box-shadow: 0 0 0 10px rgba(102, 126, 234, 0);
            }
            100% {
                box-shadow: 0 0 0 0 rgba(102, 126, 234, 0);
            }
        }
        .info-note {
            background-color: #e7f3ff;
            border-left: 4px solid #0066cc;
            padding: 15px;
            border-radius: 0 8px 8px 0;
            text-align: left;
        }
        .info-note-title {
            font-weight: 600;
            color: #0066cc;
            margin-bottom: 8px;
        }
        .info-note-text {
            color: #333;
            font-size: 14px;
            line-height: 1.5;
        }
        .dots {
            display: inline-block;
        }
        .dots::after {
            content: '';
            animation: dots 1.5s infinite;
        }
        @keyframes dots {
            0%, 20% {
                content: '';
            }
            40% {
                content: '.';
            }
            60% {
                content: '..';
            }
            80%, 100% {
                content: '...';
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="loading-spinner"></div>
        <h1>Device Restarting</h1>
        <p class="message">
            Your ESP32 Weather Station is restarting to apply the new configuration.
        </p>
        
        <div class="status-box">
            <div class="status-title">Current Status</div>
            <div class="status-text">
                Restarting device<span class="dots"></span>
            </div>
        </div>
        
        <div class="progress-steps">
            <div class="step">
                <div class="step-icon step-completed">✓</div>
                <span>Configuration saved successfully</span>
            </div>
            <div class="step">
                <div class="step-icon step-current">⟳</div>
                <span>Restarting device...</span>
            </div>
            <div class="step">
                <div class="step-icon step-pending">📶</div>
                <span>Connecting to WiFi network</span>
            </div>
            <div class="step">
                <div class="step-icon step-pending">🌤️</div>
                <span>Starting weather monitoring</span>
            </div>
        </div>
        
        <div class="info-note">
            <div class="info-note-title">💡 Important Information</div>
            <div class="info-note-text">
                • The device will disconnect from setup mode<br>
                • It will connect to your configured WiFi network<br>
                • Setup mode will be automatically disabled<br>
                • You can access the device on your local network<br>
                • This page will no longer be accessible after restart
            </div>
        </div>
    </div>

    <script>
        // Simulate progress through steps
        let currentStep = 1;
        const steps = document.querySelectorAll('.step');
        
        function updateProgress() {
            if (currentStep < steps.length) {
                // Update current step to completed
                const currentStepIcon = steps[currentStep].querySelector('.step-icon');
                currentStepIcon.className = 'step-icon step-completed';
                currentStepIcon.textContent = '✓';
                
                currentStep++;
                
                // Update next step to current if available
                if (currentStep < steps.length) {
                    const nextStepIcon = steps[currentStep].querySelector('.step-icon');
                    nextStepIcon.className = 'step-icon step-current';
                    nextStepIcon.textContent = '⟳';
                }
            }
        }
        
        // Progress through steps every 2 seconds
        setInterval(updateProgress, 2000);
        
        // Optional: Try to detect when the device actually restarts
        let connectionLost = false;
        function checkConnection() {
            fetch('/status')
                .catch(() => {
                    if (!connectionLost) {
                        connectionLost = true;
                        document.querySelector('.status-text').innerHTML = 'Device has restarted successfully ✓';
                        document.querySelector('.loading-spinner').style.display = 'none';
                        
                        // Complete all remaining steps
                        steps.forEach((step, index) => {
                            if (index > 0) {
                                const icon = step.querySelector('.step-icon');
                                icon.className = 'step-icon step-completed';
                                icon.textContent = '✓';
                            }
                        });
                    }
                });
        }
        
        // Check connection every 3 seconds
        setInterval(checkConnection, 3000);
    </script>
</body>
</html>)rawliteral";

#endif // WEB_RESTART_H
