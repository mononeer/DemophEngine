
/**
 * Demoph Engine Web Interface
 * Provides web-based controls and visualization for the engine
 */

class DemophWebInterface {
    constructor() {
        this.canvas = null;
        this.gl = null;
        this.engineStats = {
            fps: 0,
            frameTime: 0,
            entities: 0,
            drawCalls: 0
        };
        
        this.controls = {
            running: false,
            debugMode: false,
            wireframe: false
        };
        
        this.init();
    }
    
    init() {
        this.setupCanvas();
        this.setupUI();
        this.setupEventListeners();
        this.startUpdateLoop();
    }
    
    setupCanvas() {
        this.canvas = document.createElement('canvas');
        this.canvas.id = 'demoph-canvas';
        this.canvas.width = 1280;
        this.canvas.height = 720;
        this.canvas.style.border = '1px solid #333';
        this.canvas.style.backgroundColor = '#1a1a1a';
        
        // Try to get WebGL context
        this.gl = this.canvas.getContext('webgl2') || this.canvas.getContext('webgl');
        if (!this.gl) {
            console.error('WebGL not supported');
            return;
        }
        
        // Add canvas to page
        document.body.appendChild(this.canvas);
    }
    
    setupUI() {
        // Create control panel
        const controlPanel = document.createElement('div');
        controlPanel.id = 'control-panel';
        controlPanel.innerHTML = `
            <div class="panel-header">
                <h2>🎮 Demoph Engine Controls</h2>
            </div>
            
            <div class="panel-section">
                <h3>Engine Status</h3>
                <div class="status-grid">
                    <div class="status-item">
                        <label>FPS:</label>
                        <span id="fps-display">0</span>
                    </div>
                    <div class="status-item">
                        <label>Frame Time:</label>
                        <span id="frametime-display">0ms</span>
                    </div>
                    <div class="status-item">
                        <label>Entities:</label>
                        <span id="entities-display">0</span>
                    </div>
                    <div class="status-item">
                        <label>Draw Calls:</label>
                        <span id="drawcalls-display">0</span>
                    </div>
                </div>
            </div>
            
            <div class="panel-section">
                <h3>Controls</h3>
                <button id="start-btn" class="control-btn">Start Engine</button>
                <button id="stop-btn" class="control-btn" disabled>Stop Engine</button>
                <br><br>
                <label>
                    <input type="checkbox" id="debug-mode"> Debug Mode
                </label>
                <br>
                <label>
                    <input type="checkbox" id="wireframe-mode"> Wireframe
                </label>
            </div>
            
            <div class="panel-section">
                <h3>Scene Objects</h3>
                <div id="entity-list">
                    <div class="entity-item">Main Camera</div>
                    <div class="entity-item">Demo Cube</div>
                    <div class="entity-item">Ground Plane</div>
                    <div class="entity-item">Particle System</div>
                </div>
            </div>
            
            <div class="panel-section">
                <h3>Rendering Settings</h3>
                <label>
                    Ambient Light: <input type="range" id="ambient-light" min="0" max="1" step="0.1" value="0.3">
                </label>
                <br>
                <label>
                    Exposure: <input type="range" id="exposure" min="0.1" max="3.0" step="0.1" value="1.0">
                </label>
            </div>
        `;
        
        // Style the control panel
        controlPanel.style.cssText = `
            position: fixed;
            top: 20px;
            right: 20px;
            width: 300px;
            background: linear-gradient(135deg, #2a2a2a, #1a1a1a);
            border: 1px solid #444;
            border-radius: 8px;
            padding: 15px;
            color: #fff;
            font-family: 'Consolas', 'Monaco', monospace;
            box-shadow: 0 4px 20px rgba(0,0,0,0.5);
            z-index: 1000;
        `;
        
        document.body.appendChild(controlPanel);
        
        // Add CSS styles
        const style = document.createElement('style');
        style.textContent = `
            body {
                margin: 0;
                padding: 20px;
                background: #0a0a0a;
                color: #fff;
                font-family: 'Consolas', monospace;
            }
            
            .panel-header h2 {
                margin: 0 0 15px 0;
                color: #00ff88;
                text-align: center;
            }
            
            .panel-section {
                margin-bottom: 20px;
                padding-bottom: 15px;
                border-bottom: 1px solid #333;
            }
            
            .panel-section h3 {
                margin: 0 0 10px 0;
                color: #88aaff;
                font-size: 14px;
            }
            
            .status-grid {
                display: grid;
                grid-template-columns: 1fr 1fr;
                gap: 5px;
            }
            
            .status-item {
                display: flex;
                justify-content: space-between;
                font-size: 12px;
            }
            
            .control-btn {
                background: linear-gradient(135deg, #4a90e2, #357abd);
                border: none;
                color: white;
                padding: 8px 16px;
                border-radius: 4px;
                cursor: pointer;
                margin-right: 5px;
                font-size: 12px;
            }
            
            .control-btn:hover {
                background: linear-gradient(135deg, #357abd, #2a5d8f);
            }
            
            .control-btn:disabled {
                background: #555;
                cursor: not-allowed;
            }
            
            .entity-item {
                background: #333;
                padding: 5px 10px;
                margin: 2px 0;
                border-radius: 3px;
                font-size: 12px;
                cursor: pointer;
            }
            
            .entity-item:hover {
                background: #444;
            }
            
            input[type="range"] {
                width: 100%;
                margin: 5px 0;
            }
            
            label {
                font-size: 12px;
                display: block;
                margin: 5px 0;
            }
        `;
        document.head.appendChild(style);
    }
    
    setupEventListeners() {
        const startBtn = document.getElementById('start-btn');
        const stopBtn = document.getElementById('stop-btn');
        const debugMode = document.getElementById('debug-mode');
        const wireframeMode = document.getElementById('wireframe-mode');
        
        startBtn.addEventListener('click', () => {
            this.startEngine();
            startBtn.disabled = true;
            stopBtn.disabled = false;
        });
        
        stopBtn.addEventListener('click', () => {
            this.stopEngine();
            startBtn.disabled = false;
            stopBtn.disabled = true;
        });
        
        debugMode.addEventListener('change', (e) => {
            this.controls.debugMode = e.target.checked;
            this.sendControlUpdate();
        });
        
        wireframeMode.addEventListener('change', (e) => {
            this.controls.wireframe = e.target.checked;
            this.sendControlUpdate();
        });
        
        // Canvas mouse controls
        this.canvas.addEventListener('mousedown', (e) => this.handleMouseDown(e));
        this.canvas.addEventListener('mousemove', (e) => this.handleMouseMove(e));
        this.canvas.addEventListener('mouseup', (e) => this.handleMouseUp(e));
        this.canvas.addEventListener('wheel', (e) => this.handleWheel(e));
    }
    
    startEngine() {
        console.log('🚀 Starting Demoph Engine');
        this.controls.running = true;
        
        // In a real implementation, this would communicate with the C++ engine
        // via WebAssembly or a local server
        this.simulateEngineStart();
    }
    
    stopEngine() {
        console.log('🛑 Stopping Demoph Engine');
        this.controls.running = false;
    }
    
    simulateEngineStart() {
        // Simulate engine statistics
        let frameCount = 0;
        const startTime = performance.now();
        
        const updateStats = () => {
            if (!this.controls.running) return;
            
            frameCount++;
            const currentTime = performance.now();
            const elapsedTime = (currentTime - startTime) / 1000;
            
            // Simulate varying performance
            this.engineStats.fps = Math.floor(58 + Math.random() * 4);
            this.engineStats.frameTime = (1000 / this.engineStats.fps).toFixed(2);
            this.engineStats.entities = 4 + Math.floor(Math.random() * 3);
            this.engineStats.drawCalls = 12 + Math.floor(Math.random() * 8);
            
            this.updateStatsDisplay();
            
            requestAnimationFrame(updateStats);
        };
        
        updateStats();
    }
    
    updateStatsDisplay() {
        document.getElementById('fps-display').textContent = this.engineStats.fps;
        document.getElementById('frametime-display').textContent = this.engineStats.frameTime + 'ms';
        document.getElementById('entities-display').textContent = this.engineStats.entities;
        document.getElementById('drawcalls-display').textContent = this.engineStats.drawCalls;
    }
    
    sendControlUpdate() {
        // In a real implementation, send control updates to the engine
        console.log('Control update:', this.controls);
    }
    
    handleMouseDown(e) {
        // Handle camera controls
        console.log('Mouse down:', e.offsetX, e.offsetY);
    }
    
    handleMouseMove(e) {
        // Handle camera rotation
        if (e.buttons === 1) { // Left mouse button
            console.log('Camera rotation:', e.movementX, e.movementY);
        }
    }
    
    handleMouseUp(e) {
        console.log('Mouse up');
    }
    
    handleWheel(e) {
        // Handle camera zoom
        console.log('Zoom:', e.deltaY);
        e.preventDefault();
    }
    
    startUpdateLoop() {
        const update = () => {
            // Update WebGL rendering here if needed
            if (this.gl && this.controls.running) {
                this.renderFrame();
            }
            
            requestAnimationFrame(update);
        };
        
        update();
    }
    
    renderFrame() {
        const gl = this.gl;
        
        // Clear the canvas
        gl.viewport(0, 0, this.canvas.width, this.canvas.height);
        gl.clearColor(0.1, 0.1, 0.15, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        
        // In a real implementation, this would render the actual scene
        // For now, just draw a simple gradient background
        this.drawGradientBackground();
    }
    
    drawGradientBackground() {
        const gl = this.gl;
        
        // Simple background rendering
        const time = performance.now() * 0.001;
        const r = 0.1 + 0.05 * Math.sin(time * 0.5);
        const g = 0.1 + 0.05 * Math.sin(time * 0.3);
        const b = 0.15 + 0.05 * Math.sin(time * 0.7);
        
        gl.clearColor(r, g, b, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT);
    }
}

// Initialize the web interface when the page loads
document.addEventListener('DOMContentLoaded', () => {
    console.log('🎮 Initializing Demoph Engine Web Interface');
    window.demophInterface = new DemophWebInterface();
});

// Export for module systems
if (typeof module !== 'undefined' && module.exports) {
    module.exports = DemophWebInterface;
}
