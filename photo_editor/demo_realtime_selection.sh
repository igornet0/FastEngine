#!/bin/bash

# Demo script for testing the REAL-TIME selection functionality
# This script builds and runs the photo editor to test selection tool with live preview

echo "=== Photo Editor - Real-Time Selection Fix Demo ==="
echo ""

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: Please run this script from the photo_editor directory"
    exit 1
fi

# Build the project
echo "Building photo editor..."
if [ ! -d "build" ]; then
    mkdir build
fi

cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"
echo ""

# Run the application
echo "Starting Photo Editor..."
echo ""
echo "=== TESTING INSTRUCTIONS ==="
echo "1. The application should start with a white canvas"
echo "2. Select the 'Select' tool (should be selected by default)"
echo "3. Click and drag on the canvas to create a selection"
echo "4. You should see a blue dashed rectangle appearing IMMEDIATELY while dragging"
echo "5. The selection should update in real-time as you move the mouse"
echo "6. When you release the mouse, corner handles should appear"
echo "7. Try different tools to see their cursors change"
echo "8. Check the Logs panel for detailed tool usage information"
echo ""
echo "=== EXPECTED BEHAVIOR (FIXED) ==="
echo "✅ Selection rectangle appears IMMEDIATELY when you start dragging"
echo "✅ Selection updates in real-time as you move the mouse"
echo "✅ Blue dashed rectangle is visible during the entire drag operation"
echo "✅ Corner handles appear only after completing the selection"
echo "✅ Tool cursors change for each tool"
echo "✅ Detailed logs appear in the Logs panel"
echo ""
echo "=== KEY IMPROVEMENTS ==="
echo "• Added m_isSelecting state to track selection in progress"
echo "• Selection rectangle now shows during dragging (not just after completion)"
echo "• Real-time visual feedback during mouse drag operations"
echo "• Corner handles only appear for completed selections"
echo "• Better user experience with immediate visual feedback"
echo ""
echo "Press Ctrl+C to exit the application"
echo ""

./PhotoEditor
