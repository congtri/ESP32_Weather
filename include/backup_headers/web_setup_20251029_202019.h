#ifndef WEB_SETUP_H
#define WEB_SETUP_H

#include <Arduino.h>

const char SETUP_HTML[] PROGMEM = R"(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Weather Station - WiFi Setup</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body { 
            font-family: Arial, sans-serif; 
            margin: 40px; 
            background-color: #f0f0f0; 
        }
        .container { 
            max-width: 400px; 
            margin: 0 auto; 
            background: white; 
            padding: 30px; 
            border-radius: 10px; 
            box-shadow: 0 0 10px rgba(0,0,0,0.1); 
        }
        h1 { 
            text-align: center; 
            color: #333; 
            margin-bottom: 30px; 
        }
        label { 
            display: block; 
            margin-bottom: 5px; 
            font-weight: bold; 
            color: #555; 
        }
        input[type="text"], input[type="password"] { 
            width: 100%; 
            padding: 10px; 
            margin-bottom: 15px; 
            border: 1px solid #ddd; 
            border-radius: 5px; 
            box-sizing: border-box; 
        }
        input[type="submit"] { 
            width: 100%; 
            background-color: #4CAF50; 
            color: white; 
            padding: 12px; 
            border: none; 
            border-radius: 5px; 
            cursor: pointer; 
            font-size: 16px; 
        }
        input[type="submit"]:hover { 
            background-color: #45a049; 
        }
        .info { 
            background-color: #e7f3ff; 
            padding: 15px; 
            border-radius: 5px; 
            margin-bottom: 20px; 
            border-left: 4px solid #2196F3; 
        }
        .current { 
            background-color: #fff3cd; 
            padding: 10px; 
            border-radius: 5px; 
            margin-bottom: 20px; 
            border-left: 4px solid #ffc107; 
        }
        .network-scan {
            background-color: #f8f9fa;
            padding: 15px;
            border-radius: 5px;
            margin-bottom: 20px;
            border-left: 4px solid #6c757d;
        }
        .network-list {
            max-height: 200px;
            overflow-y: auto;
            border: 1px solid #ddd;
            border-radius: 5px;
            padding: 10px;
            margin-top: 10px;
        }
        .network-item {
            padding: 8px;
            cursor: pointer;
            border-radius: 3px;
            margin-bottom: 5px;
            border: 1px solid transparent;
        }
        .network-item:hover {
            background-color: #e9ecef;
            border-color: #dee2e6;
        }
        .signal-strength {
            float: right;
            font-size: 12px;
            color: #666;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌤️ Weather Station Setup</h1>
        
        <div class="info">
            <strong>Setup Mode Active</strong><br>
            Configure your WiFi credentials below. The device will restart and connect to the specified network.
        </div>
        
        <div class="current">
            <strong>Current SSID:</strong> {{CURRENT_SSID}}
        </div>
        
        <div class="network-scan">
            <strong>Available Networks:</strong>
            <div class="network-list" id="networkList">
                <div style="text-align: center; color: #666;">
                    Scanning for networks...
                </div>
            </div>
        </div>
        
        <form action="/save" method="POST">
            <label for="ssid">WiFi Network Name (SSID):</label>
            <input type="text" id="ssid" name="ssid" value="{{CURRENT_SSID}}" required maxlength="31">
            
            <label for="password">WiFi Password:</label>
            <input type="password" id="password" name="password" placeholder="Enter WiFi password" maxlength="63">
            
            <input type="submit" value="Save and Restart">
        </form>
    </div>

    <script>
        // Auto-populate SSID when network is clicked
        function selectNetwork(ssid) {
            document.getElementById('ssid').value = ssid;
            document.getElementById('password').focus();
        }

        // Scan for networks
        function loadNetworks() {
            fetch('/scan')
                .then(response => response.json())
                .then(networks => {
                    const networkList = document.getElementById('networkList');
                    if (networks.length === 0) {
                        networkList.innerHTML = '<div style="text-align: center; color: #666;">No networks found</div>';
                        return;
                    }
                    
                    networkList.innerHTML = '';
                    networks.forEach(network => {
                        const networkItem = document.createElement('div');
                        networkItem.className = 'network-item';
                        networkItem.onclick = () => selectNetwork(network.ssid);
                        
                        const signalBars = '●'.repeat(Math.ceil(network.rssi / 20)) + '○'.repeat(5 - Math.ceil(network.rssi / 20));
                        
                        networkItem.innerHTML = `
                            <span>${network.ssid}</span>
                            <span class="signal-strength">${signalBars}</span>
                        `;
                        networkList.appendChild(networkItem);
                    });
                })
                .catch(error => {
                    document.getElementById('networkList').innerHTML = 
                        '<div style="text-align: center; color: #666;">Network scan failed</div>';
                });
        }

        // Load networks on page load
        setTimeout(loadNetworks, 1000);
    </script>
</body>
</html>
)";

#endif