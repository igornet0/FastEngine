#!/bin/bash

# Test script for Photo Editor
echo "🎨 Testing Photo Editor with Tool System..."

# Build the project
echo "📦 Building project..."
cd /Users/igor/Desktop/GameAppsAi/FastEngine/photo_editor/build
make -j4

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    
    echo "🚀 Starting Photo Editor..."
    echo "📋 Features to test:"
    echo "   • Tool selection (Select, Brush, Eraser, Crop, Text, Shape)"
    echo "   • Cursor changes for each tool"
    echo "   • Brush size adjustment (affects cursor size)"
    echo "   • Color selection"
    echo "   • Image adjustments (brightness, contrast, saturation, hue)"
    echo "   • Zoom and pan functionality"
    echo "   • File operations (open, save, new)"
    echo ""
    echo "🎯 Test the cursor changes by:"
    echo "   1. Click different tool buttons"
    echo "   2. Notice cursor changes"
    echo "   3. Adjust brush size and see cursor size change"
    echo "   4. Try drawing with brush tool"
    echo "   5. Try erasing with eraser tool"
    echo ""
    
    # Start the application
    ./PhotoEditor &
    
    echo "📱 Photo Editor started successfully!"
    echo "🔧 Tool system with custom cursors is now active"
    
else
    echo "❌ Build failed!"
    exit 1
fi
