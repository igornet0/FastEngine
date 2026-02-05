#!/bin/bash

# Demo script for testing the duplicate rectangle fix
# This script builds and runs the photo editor to test the fixed selection drawing

echo "=== Photo Editor - Duplicate Rectangle Fix Demo ==="
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
echo "4. You should see ONLY ONE blue dashed rectangle"
echo "5. No duplicate rectangles should appear"
echo "6. The selection should update in real-time during dragging"
echo "7. Corner handles should appear only after completing selection"
echo "8. Check the Logs panel for detailed tool usage information"
echo ""
echo "=== EXPECTED BEHAVIOR (FIXED) ==="
echo "✅ Only ONE selection rectangle visible"
echo "✅ No duplicate rectangles"
echo "✅ Clean, single selection display"
echo "✅ Real-time updates during dragging"
echo "✅ Corner handles only after completion"
echo "✅ Professional appearance"
echo ""
echo "=== KEY IMPROVEMENTS ==="
echo "• Removed duplicate selection drawing from drawToolPreview()"
echo "• Selection is now drawn only once in CanvasWidget::paintEvent()"
echo "• Eliminated double rendering issue"
echo "• Clean, single rectangle display"
echo "• Proper separation of concerns between CanvasWidget and ToolManager"
echo ""
echo "=== TECHNICAL FIX ==="
echo "Problem: Selection was drawn twice:"
echo "  1. In CanvasWidget::paintEvent() -> drawSelection()"
echo "  2. In ToolManager::drawToolPreview() -> drawRect()"
echo ""
echo "Solution: Removed duplicate drawing from drawToolPreview()"
echo "  - Selection is now drawn only in CanvasWidget"
echo "  - ToolManager::drawToolPreview() no longer draws selection"
echo "  - Single source of truth for selection rendering"
echo ""
echo "Press Ctrl+C to exit the application"
echo ""

./PhotoEditor
