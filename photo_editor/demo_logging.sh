#!/bin/bash

# Demo script for Photo Editor with Logging System
echo "🎨 Photo Editor with Logging System Demo"
echo "========================================"

# Build the project
echo "📦 Building project with logging system..."
cd /Users/igor/Desktop/GameAppsAi/FastEngine/photo_editor/build
make -j4

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    
    echo ""
    echo "🚀 Starting Photo Editor with Logging..."
    echo ""
    echo "📋 Logging Features:"
    echo "   • Real-time log panel at the bottom"
    echo "   • Tool usage tracking"
    echo "   • Property changes logging"
    echo "   • Image operations logging"
    echo "   • File operations logging"
    echo "   • UI events logging"
    echo ""
    echo "🎯 Test the logging by:"
    echo "   1. Open the Logs panel at the bottom"
    echo "   2. Select different tools (watch tool selection logs)"
    echo "   3. Adjust brush properties (watch property change logs)"
    echo "   4. Draw with brush tool (watch drawing logs)"
    echo "   5. Open/save images (watch file operation logs)"
    echo "   6. Use image adjustments (watch image operation logs)"
    echo ""
    echo "🔧 Log Panel Controls:"
    echo "   • Clear - Clear all logs"
    echo "   • Level - Filter by log level (Debug, Info, Warning, Error)"
    echo "   • Auto Scroll - Auto scroll to new messages"
    echo "   • File Logging - Enable/disable file logging"
    echo "   • Console Logging - Enable/disable console logging"
    echo ""
    
    # Start the application
    ./PhotoEditor &
    
    echo "📱 Photo Editor with logging system started!"
    echo "🔍 Check the Logs panel to see real-time logging"
    echo ""
    echo "📝 Log files are saved to: ~/Library/Application Support/PhotoEditor/photo_editor.log"
    
else
    echo "❌ Build failed!"
    exit 1
fi
