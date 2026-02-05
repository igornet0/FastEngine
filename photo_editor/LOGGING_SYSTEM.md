# 📝 Photo Editor Logging System

## 🎯 Overview

The Photo Editor now includes a comprehensive logging system that tracks all tool usage, property changes, image operations, and UI events in real-time.

## 🏗️ Architecture

### Core Components

1. **Logger** (`Logger.h/cpp`)
   - Singleton pattern for global logging
   - Multiple log levels (Debug, Info, Warning, Error)
   - Console and file output support
   - Thread-safe logging with mutex

2. **LogPanel** (`LogPanel.h/cpp`)
   - Real-time log display widget
   - Color-coded log levels
   - Filtering and control options
   - Auto-scroll functionality

## 📊 Log Categories

### Tool Usage Logging
```cpp
LOG_TOOL_USAGE("Brush", "SELECTED", "Changed from Select");
LOG_TOOL_USAGE("Brush", "BEGIN_DRAWING", "Point: (100, 200)");
LOG_TOOL_PROPERTY("Brush", "brush_size", "15");
LOG_TOOL_PROPERTY("Brush", "brush_color", "#ff0000");
```

### Image Operations Logging
```cpp
LOG_IMAGE_OP("OPEN", "Size: 800x600");
LOG_IMAGE_OP("SAVE", "Format: PNG");
LOG_IMAGE_OP("ROTATE", "Angle: 90");
LOG_IMAGE_OP("FLIP", "Direction: Horizontal");
```

### File Operations Logging
```cpp
LOG_INFO("FILE", "Opening image dialog...");
LOG_INFO("FILE", "Selected file: /path/to/image.png");
LOG_ERROR("FILE", "Failed to load image: /path/to/image.png");
```

### UI Events Logging
```cpp
LOG_UI_EVENT("BUTTON_CLICKED", "Tool: Brush");
LOG_UI_EVENT("SLIDER_CHANGED", "Property: brush_size, Value: 20");
LOG_UI_EVENT("MENU_SELECTED", "Menu: File, Action: Open");
```

## 🎨 Log Panel Features

### Visual Design
- **Dark theme** with syntax highlighting
- **Color-coded levels**:
  - Debug: Gray (#888888)
  - Info: White (#ffffff)
  - Warning: Orange (#ffaa00)
  - Error: Red (#ff4444)

### Controls
- **Clear Button**: Clear all logs
- **Level ComboBox**: Filter by log level
- **Auto Scroll**: Auto-scroll to new messages
- **File Logging**: Enable/disable file output
- **Console Logging**: Enable/disable console output

### Status Information
- Real-time log count
- Current log level
- File logging status

## 🔧 Usage Examples

### Basic Logging
```cpp
// Simple logging
LOG_INFO("APP", "Application started");
LOG_WARNING("TOOLS", "Tool not available");
LOG_ERROR("FILE", "File not found");

// Tool-specific logging
LOG_TOOL_USAGE("Brush", "SELECTED", "");
LOG_TOOL_PROPERTY("Brush", "opacity", "80");
```

### Advanced Logging
```cpp
// Detailed tool usage
LOG_TOOL_USAGE("Brush", "DRAWING", 
    QString("From: (%1,%2) To: (%3,%4)")
    .arg(from.x()).arg(from.y())
    .arg(to.x()).arg(to.y()));

// Image operation with details
LOG_IMAGE_OP("ADJUST", 
    QString("Brightness: %1, Contrast: %2")
    .arg(brightness).arg(contrast));
```

## 📁 Log File Location

Log files are automatically saved to:
```
~/Library/Application Support/PhotoEditor/photo_editor.log
```

## 🎛️ Configuration

### Log Levels
- **Debug**: Detailed debugging information
- **Info**: General information messages
- **Warning**: Warning messages
- **Error**: Error messages

### Output Options
- **Console**: Output to qDebug() (terminal)
- **File**: Output to log file
- **Panel**: Display in LogPanel widget

## 🚀 Benefits

1. **Debugging**: Easy identification of issues
2. **User Behavior**: Track how users interact with tools
3. **Performance**: Monitor operation timing
4. **Development**: Understand code execution flow
5. **Support**: Help users troubleshoot problems

## 🔍 Real-time Monitoring

The LogPanel provides real-time monitoring of:
- Tool selections and changes
- Property modifications
- Drawing operations
- File operations
- UI interactions
- Error conditions

## 📈 Future Enhancements

Potential improvements:
- Log filtering by category
- Log export functionality
- Performance metrics
- User session tracking
- Crash reporting integration
- Remote logging capabilities

---

*This logging system provides comprehensive visibility into the Photo Editor's operation, making development, debugging, and user support significantly easier.*
