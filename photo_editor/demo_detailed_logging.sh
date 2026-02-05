#!/bin/bash

# Detailed Logging Demo for Photo Editor
echo "🔍 Photo Editor with Detailed Logging System"
echo "============================================="

# Build the project
echo "📦 Building project with detailed logging..."
cd /Users/igor/Desktop/GameAppsAi/FastEngine/photo_editor/build
make -j4

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    
    echo ""
    echo "🚀 Starting Photo Editor with Detailed Logging..."
    echo ""
    echo "📋 Detailed Logging Features:"
    echo "   • Tool selection with previous tool tracking"
    echo "   • Drawing strokes with coordinates and properties"
    echo "   • Mouse events with widget/image coordinates"
    echo "   • Keyboard shortcuts and modifiers"
    echo "   • Zoom operations with percentage changes"
    echo "   • Pan operations with start/end tracking"
    echo "   • Property changes with before/after values"
    echo "   • Image adjustments with value tracking"
    echo "   • File operations with success/failure status"
    echo ""
    echo "🎯 Test Detailed Logging:"
    echo "   1. Select different tools (watch tool change logs)"
    echo "   2. Draw with brush (watch stroke coordinates and properties)"
    echo "   3. Adjust brush size/opacity (watch property change logs)"
    echo "   4. Change brush color (watch color change logs)"
    echo "   5. Use mouse wheel to zoom (watch zoom percentage logs)"
    echo "   6. Press Shift+Left mouse for panning (watch pan logs)"
    echo "   7. Press keyboard shortcuts (watch key press logs)"
    echo "   8. Adjust image brightness/contrast (watch adjustment logs)"
    echo "   9. Open/save images (watch file operation logs)"
    echo ""
    echo "📊 Log Categories:"
    echo "   • TOOLS - Tool usage and property changes"
    echo "   • UI - Mouse, keyboard, and interface events"
    echo "   • IMAGE - Image operations and adjustments"
    echo "   • FILE - File operations and status"
    echo "   • APP - Application lifecycle events"
    echo ""
    echo "🔧 Log Panel Controls:"
    echo "   • Level Filter - Debug, Info, Warning, Error"
    echo "   • Auto Scroll - Follow new messages"
    echo "   • File Logging - Save to ~/Library/Application Support/PhotoEditor/"
    echo "   • Console Logging - Display in terminal"
    echo ""
    
    # Start the application
    ./PhotoEditor &
    
    echo "📱 Photo Editor with detailed logging started!"
    echo "🔍 Check the Logs panel for comprehensive activity tracking"
    echo ""
    echo "📝 Example log entries you'll see:"
    echo "   [TOOLS] Tool: Brush | Action: DRAWING | Details: Stroke from (100,200) to (150,250) | Size: 15 | Color: #ff0000 | Opacity: 80%"
    echo "   [UI] Event: MOUSE_PRESS | Details: Widget: (300,400) | Image: (150,200) | Button: Left | Modifiers: Ctrl+"
    echo "   [UI] Event: ZOOM | Details: Zoom: 100% -> 120% | Delta: In | Position: (250,300)"
    echo "   [IMAGE] Operation: BRIGHTNESS_ADJUST | Details: From: 0 to: 25"
    echo ""
    
else
    echo "❌ Build failed!"
    exit 1
fi
