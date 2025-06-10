#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#include <Arduino.h>

const char WEB_INTERFACE_HTML[] PROGMEM = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <title>ESP32 Clock Controller</title>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            max-width: 600px; 
            margin: 0 auto; 
            padding: 20px; 
            background: #1a1a1a; 
            color: white; 
        }
        h1 { 
            color: #4a90e2; 
            text-align: center; 
        }
        .status { 
            background: #2a2a2a; 
            padding: 15px; 
            border-radius: 8px; 
            margin: 15px 0; 
        }
        .controls { 
            background: #2a2a2a; 
            padding: 15px; 
            border-radius: 8px; 
            margin: 15px 0; 
        }
        .animation-grid { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); 
            gap: 10px; 
            margin: 15px 0; 
        }
        .anim-btn { 
            padding: 12px; 
            background: #3a3a3a; 
            color: white; 
            border: 1px solid #555; 
            border-radius: 5px; 
            cursor: pointer; 
            text-decoration: none; 
            text-align: center; 
            transition: all 0.2s; 
        }
        .anim-btn:hover { 
            background: #4a4a4a; 
        }
        .anim-btn.active { 
            background: #4a90e2; 
            border-color: #6bb6ff; 
        }
        .toggle-btn { 
            padding: 10px 20px; 
            background: #4a90e2; 
            color: white; 
            border: none; 
            border-radius: 5px; 
            cursor: pointer; 
            margin: 5px; 
        }
        .toggle-btn:hover { 
            background: #6bb6ff; 
        }
        .toggle-btn.active { 
            background: #2a5a2a; 
        }
        .auto-mode { 
            background: #2a5a2a; 
        }
        .manual-mode { 
            background: #5a2a2a; 
        }
        a { 
            color: #4a90e2; 
            text-decoration: none; 
        }
        a:hover { 
            text-decoration: underline; 
        }
        .message { 
            padding: 10px; 
            border-radius: 5px; 
            margin: 10px 0; 
            display: none; 
        }
        .message.success { 
            background: #2a5a2a; 
            color: #90ee90; 
        }
        .message.error { 
            background: #5a2a2a; 
            color: #ffcccb; 
        }
    </style>
</head>
<body>
    <h1>🎨 ESP32 Clock Controller</h1>
    
    <div class="message" id="message"></div>
    
    <div class="status">
        <h3>📊 Status</h3>
        <p><strong>Current Animation:</strong> <span id="currentAnimation">Loading...</span></p>
        <p><strong>Status:</strong> <span id="status">Loading...</span></p>
        <p><strong>Mode:</strong> <span id="mode">Loading...</span></p>
    </div>
    
    <div class="controls">
        <h3>🎛️ Controls</h3>
        
        <p>
            <button class="toggle-btn auto-mode" id="autoBtn" onclick="setMode('auto')">🔄 Auto Mode</button>
            <button class="toggle-btn manual-mode" id="manualBtn" onclick="setMode('manual')">🎯 Manual Mode</button>
        </p>
        
        <h4>Select Animation:</h4>
        <div class="animation-grid" id="animationGrid">
            <!-- Animation buttons will be populated by JavaScript -->
        </div>
    </div>
    
    <div class="status">
        <h3>ℹ️ Information</h3>
        <p>In <strong>Auto Mode</strong>, animations cycle every 3 hours with smooth 2-second fade transitions.</p>
        <p>In <strong>Manual Mode</strong>, you can select any animation and it will stay active.</p>
        <p><a href="/erase" onclick="return confirm('Are you sure you want to erase WiFi settings and restart?')">🗑️ Erase WiFi Settings & Restart</a></p>
    </div>
    
    <script>
        const animations = [
            {id: 0, name: 'Plasma', emoji: '🌈'},
            {id: 1, name: 'Particles', emoji: '✨'},
            {id: 2, name: 'Fire', emoji: '🔥'},
            {id: 3, name: 'Skyline', emoji: '🏙️'},
            {id: 4, name: 'Galaxy', emoji: '🌌'},
            {id: 5, name: 'Lightning', emoji: '⚡'},
            {id: 6, name: 'Pipes', emoji: '🔧'},
            {id: 7, name: 'Orbital', emoji: '🪐'},
            {id: 8, name: 'Stars', emoji: '⭐'},
            {id: 9, name: 'Beach', emoji: '🏖️'}
        ];
        
        let currentState = null;
        
        function showMessage(text, type = 'success') {
            const messageEl = document.getElementById('message');
            messageEl.textContent = text;
            messageEl.className = `message ${type}`;
            messageEl.style.display = 'block';
            setTimeout(() => {
                messageEl.style.display = 'none';
            }, 3000);
        }
        
        async function fetchAPI(url, method = 'GET') {
            try {
                const response = await fetch(url, { method });
                if (!response.ok) {
                    throw new Error(`HTTP ${response.status}`);
                }
                return await response.json();
            } catch (error) {
                showMessage(`Error: ${error.message}`, 'error');
                return null;
            }
        }
        
        async function setMode(mode) {
            const result = await fetchAPI(`/api/mode/${mode}`, 'POST');
            if (result && result.success) {
                showMessage(`Switched to ${mode} mode`);
                updateStatus();
            }
        }
        
        async function setAnimation(animationId) {
            const result = await fetchAPI(`/api/animation/${animationId}`, 'POST');
            if (result && result.success) {
                const animName = animations.find(a => a.id === animationId)?.name || 'Unknown';
                showMessage(`Switched to ${animName} animation`);
                updateStatus();
            }
        }
        
        async function updateStatus() {
            const status = await fetchAPI('/api/status');
            if (!status) return;
            
            currentState = status;
            
            // Update status display
            const currentAnim = animations.find(a => a.id === status.currentAnimation);
            document.getElementById('currentAnimation').textContent = currentAnim ? currentAnim.name : 'Unknown';
            
            let statusText = 'Running';
            if (status.inFade) {
                statusText = `Transitioning (${Math.round(status.fadeProgress)}%)`;
            }
            document.getElementById('status').textContent = statusText;
            document.getElementById('mode').textContent = status.autoMode ? 'Auto-Cycling' : 'Manual Control';
            
            // Update mode buttons
            document.getElementById('autoBtn').classList.toggle('active', status.autoMode);
            document.getElementById('manualBtn').classList.toggle('active', !status.autoMode);
            
            // Update animation grid
            updateAnimationGrid();
        }
        
        function updateAnimationGrid() {
            const grid = document.getElementById('animationGrid');
            grid.innerHTML = '';
            
            animations.forEach(anim => {
                const isActive = currentState && currentState.currentAnimation === anim.id;
                const button = document.createElement('button');
                button.className = `anim-btn${isActive ? ' active' : ''}`;
                button.innerHTML = `${anim.emoji} ${anim.name}`;
                button.onclick = () => setAnimation(anim.id);
                grid.appendChild(button);
            });
        }
        
        // Initial load and periodic updates
        updateStatus();
        setInterval(updateStatus, 2000);  // Update every 2 seconds
    </script>
</body>
</html>
)";

#endif // WEB_INTERFACE_H 