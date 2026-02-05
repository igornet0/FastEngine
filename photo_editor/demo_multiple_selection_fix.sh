#!/bin/bash

# Demo script for testing the fixed multiple selection issue
# This script builds and runs the photo editor to test selection clearing

echo "=== Photo Editor - Multiple Selection Fix Demo ==="
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
echo "3. Create a selection by clicking and dragging"
echo "4. Create another selection - the previous one should disappear"
echo "5. Switch to another tool (Brush, Eraser, etc.) - selection should clear"
echo "6. Switch back to Select tool and create new selection"
echo "7. Check the Logs panel for detailed tool usage information"
echo ""
echo "=== EXPECTED BEHAVIOR (FIXED) ==="
echo "✅ Only ONE selection rectangle should be visible at a time"
echo "✅ Previous selection clears when creating new one"
echo "✅ Selection clears when switching to other tools"
echo "✅ No multiple overlapping selections"
echo "✅ Clean, professional behavior"
echo ""
echo "=== KEY IMPROVEMENTS ==="
echo "• Added clearSelection() method to properly clear selections"
echo "• Previous selection is cleared when starting new selection"
echo "• Selection is cleared when switching away from Select tool"
echo "• Proper state management prevents multiple selections"
echo "• Better user experience with clean selection behavior"
echo ""
echo "Press Ctrl+C to exit the application"
echo ""

./PhotoEditor
