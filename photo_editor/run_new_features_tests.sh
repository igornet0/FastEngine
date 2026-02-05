#!/bin/bash

# Script to run new features tests for Photo Editor

echo "Building and running new features tests..."

# Create build directory
mkdir -p build_tests_new_features
cd build_tests_new_features

# Copy CMakeLists.txt
cp ../tests/CMakeLists_new_features.txt ./CMakeLists.txt

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Debug .

# Build tests
make -j4

# Run tests
echo "Running DocumentManager tests..."
./test_document_manager

echo "Running GridOverlay tests..."
./test_grid_overlay

echo "Running AutoSaveManager tests..."
./test_auto_save_manager

echo "Running BatchProcessor tests..."
./test_batch_processor

echo "Running ThemeManager tests..."
./test_theme_manager

echo "Running MetadataViewer tests..."
./test_metadata_viewer

echo "Running ExportManager tests..."
./test_export_manager

echo "Running ClipboardHistory tests..."
./test_clipboard_history

echo "All new features tests completed!"

# Run with CTest for better reporting
echo "Running tests with CTest..."
ctest --output-on-failure

echo "Test run complete!"
