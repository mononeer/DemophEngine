
"""
Demoph Engine Python Bindings
Provides Python scripting interface for the engine
"""

import ctypes
from ctypes import Structure, c_float, c_int, c_bool, c_char_p, POINTER
from typing import List, Tuple, Optional
import os

# Load the engine library
if os.name == 'nt':
    engine_lib = ctypes.CDLL('./libdemoph.dll')
else:
    engine_lib = ctypes.CDLL('./libdemoph.so')

class Vector3(Structure):
    _fields_ = [
        ("x", c_float),
        ("y", c_float),
        ("z", c_float)
    ]
    
    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x = x
        self.y = y
        self.z = z

class Transform(Structure):
    _fields_ = [
        ("position", Vector3),
        ("rotation", Vector3),
        ("scale", Vector3)
    ]

# Engine API bindings
engine_lib.CreateEngine.restype = ctypes.c_void_p
engine_lib.DestroyEngine.argtypes = [ctypes.c_void_p]
engine_lib.InitializeEngine.argtypes = [ctypes.c_void_p]
engine_lib.InitializeEngine.restype = c_bool
engine_lib.UpdateEngine.argtypes = [ctypes.c_void_p, c_float]
engine_lib.ShutdownEngine.argtypes = [ctypes.c_void_p]

# Entity API bindings
engine_lib.CreateEntity.argtypes = [ctypes.c_void_p, c_char_p]
engine_lib.CreateEntity.restype = ctypes.c_void_p
engine_lib.DestroyEntity.argtypes = [ctypes.c_void_p, ctypes.c_void_p]

class EngineBinding:
    """Python wrapper for the Demoph Engine"""
    
    def __init__(self):
        self._engine = engine_lib.CreateEngine()
        self._entities = []
    
    def __del__(self):
        if hasattr(self, '_engine') and self._engine:
            engine_lib.DestroyEngine(self._engine)
    
    def initialize(self) -> bool:
        """Initialize the engine"""
        return engine_lib.InitializeEngine(self._engine)
    
    def update(self, delta_time: float):
        """Update the engine"""
        engine_lib.UpdateEngine(self._engine, delta_time)
    
    def shutdown(self):
        """Shutdown the engine"""
        engine_lib.ShutdownEngine(self._engine)
    
    def create_entity(self, name: str = "Entity"):
        """Create a new entity"""
        entity = engine_lib.CreateEntity(self._engine, name.encode('utf-8'))
        self._entities.append(entity)
        return entity
    
    def destroy_entity(self, entity):
        """Destroy an entity"""
        if entity in self._entities:
            engine_lib.DestroyEntity(self._engine, entity)
            self._entities.remove(entity)

class ScriptComponent:
    """Base class for Python script components"""
    
    def __init__(self):
        self.entity = None
        self.enabled = True
    
    def start(self):
        """Called when the script starts"""
        pass
    
    def update(self, delta_time: float):
        """Called every frame"""
        pass
    
    def on_collision_enter(self, other):
        """Called when collision starts"""
        pass
    
    def on_collision_exit(self, other):
        """Called when collision ends"""
        pass

# Utility functions
def log_info(message: str):
    """Log an info message"""
    print(f"[INFO] {message}")

def log_warning(message: str):
    """Log a warning message"""
    print(f"[WARNING] {message}")

def log_error(message: str):
    """Log an error message"""
    print(f"[ERROR] {message}")

# Example usage
if __name__ == "__main__":
    engine = EngineBinding()
    
    if engine.initialize():
        log_info("Engine initialized successfully")
        
        # Create test entity
        entity = engine.create_entity("TestEntity")
        log_info(f"Created entity: {entity}")
        
        # Run for a few frames
        for i in range(60):
            engine.update(1.0 / 60.0)
        
        engine.shutdown()
        log_info("Engine shutdown complete")
    else:
        log_error("Failed to initialize engine")
