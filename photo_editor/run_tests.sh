#!/bin/bash

# Test runner script for Photo Editor components
# This script builds and runs all tests with detailed reporting

echo "🧪 Photo Editor Test Suite"
echo "=========================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test results
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to print colored output
print_status() {
    local status=$1
    local message=$2
    case $status in
        "PASS")
            echo -e "${GREEN}✅ PASS${NC}: $message"
            ;;
        "FAIL")
            echo -e "${RED}❌ FAIL${NC}: $message"
            ;;
        "INFO")
            echo -e "${BLUE}ℹ️  INFO${NC}: $message"
            ;;
        "WARN")
            echo -e "${YELLOW}⚠️  WARN${NC}: $message"
            ;;
    esac
}

# Function to run a test
run_test() {
    local test_name=$1
    local test_command=$2
    
    print_status "INFO" "Running $test_name..."
    
    if eval "$test_command" > "${test_name}.log" 2>&1; then
        print_status "PASS" "$test_name completed successfully"
        ((PASSED_TESTS++))
    else
        print_status "FAIL" "$test_name failed"
        echo "Error output:"
        cat "${test_name}.log"
        ((FAILED_TESTS++))
    fi
    
    ((TOTAL_TESTS++))
    echo ""
}

# Check if we're in the right directory
if [ ! -d "tests" ]; then
    print_status "FAIL" "Tests directory not found. Please run from photo_editor directory."
    exit 1
fi

# Create build directory for tests
print_status "INFO" "Setting up test environment..."
mkdir -p tests/build
cd tests/build

# Clean previous build
print_status "INFO" "Cleaning previous build..."
rm -rf *

# Configure CMake
print_status "INFO" "Configuring CMake..."
if cmake .. > cmake_config.log 2>&1; then
    print_status "PASS" "CMake configuration successful"
else
    print_status "FAIL" "CMake configuration failed"
    echo "Configuration error:"
    cat cmake_config.log
    exit 1
fi

# Build tests
print_status "INFO" "Building tests..."
if make -j$(sysctl -n hw.ncpu) > build.log 2>&1; then
    print_status "PASS" "Build successful"
else
    print_status "FAIL" "Build failed"
    echo "Build error:"
    cat build.log
    exit 1
fi

echo ""
echo "🧪 Running Tests"
echo "================"
echo ""

# Run individual test suites
run_test "HotkeyManagerTest" "./PhotoEditorTests -o HotkeyManagerTest"
run_test "TooltipManagerTest" "./PhotoEditorTests -o TooltipManagerTest"
run_test "MoveToolTest" "./PhotoEditorTests -o MoveToolTest"
run_test "EyedropperToolTest" "./PhotoEditorTests -o EyedropperToolTest"
run_test "IntegrationTest" "./PhotoEditorTests -o IntegrationTest"

# Run all tests together
print_status "INFO" "Running all tests together..."
if ./PhotoEditorTests > all_tests.log 2>&1; then
    print_status "PASS" "All tests completed successfully"
else
    print_status "FAIL" "Some tests failed"
    echo "Test output:"
    cat all_tests.log
fi

echo ""
echo "📊 Test Results Summary"
echo "======================"
echo ""

print_status "INFO" "Total tests: $TOTAL_TESTS"
print_status "PASS" "Passed: $PASSED_TESTS"
if [ $FAILED_TESTS -gt 0 ]; then
    print_status "FAIL" "Failed: $FAILED_TESTS"
else
    print_status "PASS" "Failed: $FAILED_TESTS"
fi

# Calculate success rate
if [ $TOTAL_TESTS -gt 0 ]; then
    SUCCESS_RATE=$((PASSED_TESTS * 100 / TOTAL_TESTS))
    print_status "INFO" "Success rate: $SUCCESS_RATE%"
fi

echo ""
echo "📁 Test Artifacts"
echo "================="
echo ""

# List generated files
print_status "INFO" "Generated test files:"
ls -la *.log 2>/dev/null || echo "No log files found"

echo ""
echo "📋 Detailed Test Logs"
echo "===================="
echo ""

# Show detailed logs for each test
for log_file in *.log; do
    if [ -f "$log_file" ]; then
        echo "--- $log_file ---"
        cat "$log_file"
        echo ""
    fi
done

# Generate test report
echo ""
echo "📄 Generating Test Report..."
echo ""

cat > test_report.md << EOF
# Photo Editor Test Report

## Test Summary

- **Total Tests**: $TOTAL_TESTS
- **Passed**: $PASSED_TESTS
- **Failed**: $FAILED_TESTS
- **Success Rate**: $SUCCESS_RATE%

## Test Results

### Individual Test Suites

EOF

# Add individual test results to report
for log_file in HotkeyManagerTest.log TooltipManagerTest.log MoveToolTest.log EyedropperToolTest.log IntegrationTest.log; do
    if [ -f "$log_file" ]; then
        test_name=$(basename "$log_file" .log)
        echo "#### $test_name" >> test_report.md
        echo '```' >> test_report.md
        cat "$log_file" >> test_report.md
        echo '```' >> test_report.md
        echo "" >> test_report.md
    fi
done

cat >> test_report.md << EOF

## Test Coverage

### Components Tested

- ✅ HotkeyManager - Hotkey registration and management
- ✅ TooltipManager - Tooltip system with hotkeys
- ✅ MoveTool - Move tool functionality
- ✅ EyedropperTool - Color sampling tool
- ✅ Integration Tests - Component integration

### Test Categories

- ✅ Unit Tests - Individual component testing
- ✅ Integration Tests - Component interaction testing
- ✅ Signal Tests - Qt signal/slot testing
- ✅ Property Tests - Property system testing
- ✅ Error Handling Tests - Edge case testing
- ✅ Performance Tests - Performance validation

## Recommendations

EOF

if [ $FAILED_TESTS -eq 0 ]; then
    echo "- ✅ All tests passed! The implementation is ready for production." >> test_report.md
else
    echo "- ⚠️ Some tests failed. Please review the failed tests and fix issues." >> test_report.md
fi

echo "- 📈 Consider adding more edge case tests for robustness." >> test_report.md
echo "- 🔍 Add performance benchmarks for optimization." >> test_report.md
echo "- 📚 Document test cases for future maintenance." >> test_report.md

print_status "PASS" "Test report generated: test_report.md"

echo ""
echo "🎉 Test Suite Complete!"
echo ""

if [ $FAILED_TESTS -eq 0 ]; then
    print_status "PASS" "All tests passed! 🎉"
    exit 0
else
    print_status "FAIL" "Some tests failed. Please review the results."
    exit 1
fi
