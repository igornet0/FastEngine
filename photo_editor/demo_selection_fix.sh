#!/bin/bash

# Demo script for testing the fixed selection functionality
# This script builds and runs the photo editor to test selection tool

echo "=== Photo Editor - Selection Tool Fix Demo ==="
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
echo "4. You should see a blue dashed rectangle with corner handles"
echo "5. The selection should be visible and interactive"
echo "6. Try different tools to see their cursors change"
echo "7. Check the Logs panel for detailed tool usage information"
echo ""
echo "=== EXPECTED BEHAVIOR ==="
echo "- Selection rectangle should be visible (blue dashed line)"
echo "- Corner handles should be visible (small squares)"
echo "- Selection should update in real-time as you drag"
echo "- Tool cursors should change for each tool"
echo "- Detailed logs should appear in the Logs panel"
echo ""
echo "Press Ctrl+C to exit the application"
echo ""

./PhotoEditor
