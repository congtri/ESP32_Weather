#!/bin/bash

# HTML to Header Converter Script
# Converts HTML files in web/ directory to C++ header files with PROGMEM constants
# Author: ESP32 Weather Station Project
# Version: 1.0

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
WEB_DIR="web"
INCLUDE_DIR="include"
BACKUP_DIR="include/backup_headers"

# Print header
echo -e "${BLUE}================================${NC}"
echo -e "${BLUE}  HTML to Header Converter${NC}"
echo -e "${BLUE}================================${NC}"
echo ""

# Check if web directory exists
if [ ! -d "$WEB_DIR" ]; then
    echo -e "${RED}Error: $WEB_DIR directory not found!${NC}"
    echo "Please make sure you have HTML files in the $WEB_DIR directory."
    exit 1
fi

# Create include directory if it doesn't exist
mkdir -p "$INCLUDE_DIR"

# Create backup directory
mkdir -p "$BACKUP_DIR"

# Function to convert HTML file to header
convert_html_to_header() {
    local html_file="$1"
    local filename=$(basename "$html_file" .html)
    local header_file="$INCLUDE_DIR/web_${filename}.h"
    local constant_name="$(echo ${filename} | tr '[:lower:]' '[:upper:]')_HTML"  # Convert to uppercase
    
    echo -e "${YELLOW}Converting: ${html_file} -> ${header_file}${NC}"
    
    # Backup existing header if it exists
    if [ -f "$header_file" ]; then
        echo "  Backing up existing header..."
        cp "$header_file" "$BACKUP_DIR/web_${filename}_$(date +%Y%m%d_%H%M%S).h"
    fi
    
    # Start writing the header file
    cat > "$header_file" << EOF
#ifndef WEB_$(echo ${filename} | tr '[:lower:]' '[:upper:]')_H
#define WEB_$(echo ${filename} | tr '[:lower:]' '[:upper:]')_H

#include <Arduino.h>

// Auto-generated header file from ${html_file}
// Generated on: $(date)
// Script: $(basename "$0")

const char ${constant_name}[] PROGMEM = R"rawliteral(
EOF
    
    # Append the HTML content
    cat "$html_file" >> "$header_file"
    
    # Close the raw string and header guard
    cat >> "$header_file" << EOF
)rawliteral";

#endif // WEB_$(echo ${filename} | tr '[:lower:]' '[:upper:]')_H
EOF
    
    # Calculate file sizes
    local html_size=$(wc -c < "$html_file")
    local header_size=$(wc -c < "$header_file")
    
    echo -e "  ${GREEN}✓ Generated successfully${NC}"
    echo "  HTML size: ${html_size} bytes"
    echo "  Header size: ${header_size} bytes"
    echo "  Constant name: ${constant_name}"
    echo ""
}

# Function to generate web_resources.h
generate_web_resources() {
    local resources_file="$INCLUDE_DIR/web_resources.h"
    
    echo -e "${YELLOW}Generating: web_resources.h${NC}"
    
    # Backup existing file if it exists
    if [ -f "$resources_file" ]; then
        echo "  Backing up existing web_resources.h..."
        cp "$resources_file" "$BACKUP_DIR/web_resources_$(date +%Y%m%d_%H%M%S).h"
    fi
    
    cat > "$resources_file" << 'EOF'
#ifndef WEB_RESOURCES_H
#define WEB_RESOURCES_H

#include <Arduino.h>
#include <WString.h>

// Auto-generated web resources manager
// This file includes all web page headers and provides a unified interface

// Include all web page headers
#include "web_setup.h"
#include "web_success.h"
#include "web_restart.h"

/**
 * WebResources class - Manages HTML content stored in Flash memory (PROGMEM)
 * 
 * This class provides static methods to access HTML content efficiently from Flash memory.
 * All HTML content is stored as PROGMEM constants to save RAM and improve security.
 * 
 * Features:
 * - Memory efficient: Content stored in Flash, not RAM
 * - Secure: Content compiled into firmware, cannot be modified
 * - Fast access: Direct Flash memory reading
 * - Template support: Dynamic content replacement
 * 
 * Usage:
 *   String html = WebResources::getSetupPage();
 *   String customized = WebResources::getSuccessPage("MyNetwork");
 */
class WebResources {
private:
    static const size_t CHUNK_SIZE = 1024;  // Read chunks of 1KB for memory efficiency
    
    /**
     * Read PROGMEM string efficiently with chunked approach
     * @param progmemData Pointer to PROGMEM data
     * @return String containing the content
     */
    static String readProgmemString(const char* progmemData) {
        size_t length = strlen_P(progmemData);
        String result;
        result.reserve(length + 1);
        
        // Read in chunks to avoid large stack allocations
        char buffer[CHUNK_SIZE + 1];
        size_t pos = 0;
        
        while (pos < length) {
            size_t chunkLen = min(CHUNK_SIZE, length - pos);
            memcpy_P(buffer, progmemData + pos, chunkLen);
            buffer[chunkLen] = '\0';
            result += buffer;
            pos += chunkLen;
        }
        
        return result;
    }
    
public:
    /**
     * Get the WiFi setup page HTML
     * @return String containing the complete setup page HTML
     */
    static String getSetupPage() {
        return readProgmemString(SETUP_HTML);
    }
    
    /**
     * Get the configuration success page HTML with dynamic content
     * @param ssid WiFi network name to display (optional)
     * @return String containing the complete success page HTML
     */
    static String getSuccessPage(const String& ssid = "") {
        String html = readProgmemString(SUCCESS_HTML);
        
        // Replace placeholder with actual SSID if provided
        if (ssid.length() > 0) {
            html.replace("Loading...", ssid);
        }
        
        return html;
    }
    
    /**
     * Get the device restart page HTML
     * @return String containing the complete restart page HTML
     */
    static String getRestartPage() {
        return readProgmemString(RESTART_HTML);
    }
    
    /**
     * Get content type for HTML responses
     * @return String containing the content type
     */
    static String getContentType() {
        return "text/html; charset=utf-8";
    }
    
    /**
     * Get estimated total Flash memory usage for all web content
     * @return Size in bytes
     */
    static size_t getTotalSize() {
        return strlen_P(SETUP_HTML) + strlen_P(SUCCESS_HTML) + strlen_P(RESTART_HTML);
    }
    
    /**
     * Get individual page sizes for debugging/monitoring
     * @param setupSize Reference to store setup page size
     * @param successSize Reference to store success page size  
     * @param restartSize Reference to store restart page size
     */
    static void getPageSizes(size_t& setupSize, size_t& successSize, size_t& restartSize) {
        setupSize = strlen_P(SETUP_HTML);
        successSize = strlen_P(SUCCESS_HTML);
        restartSize = strlen_P(RESTART_HTML);
    }
    
    /**
     * Validate that all resources are available
     * @return true if all resources are properly loaded
     */
    static bool validateResources() {
        return (strlen_P(SETUP_HTML) > 0 && 
                strlen_P(SUCCESS_HTML) > 0 && 
                strlen_P(RESTART_HTML) > 0);
    }
};

#endif // WEB_RESOURCES_H
EOF
    
    echo -e "  ${GREEN}✓ Generated web_resources.h${NC}"
    echo ""
}

# Main conversion process
echo "Starting HTML to Header conversion..."
echo "Source directory: $WEB_DIR"
echo "Target directory: $INCLUDE_DIR"
echo ""

# Convert each HTML file
html_files=()
total_size=0

for html_file in "$WEB_DIR"/*.html; do
    if [ -f "$html_file" ]; then
        convert_html_to_header "$html_file"
        html_files+=("$html_file")
        size=$(wc -c < "$html_file")
        total_size=$((total_size + size))
    fi
done

# Generate the unified web_resources.h
generate_web_resources

# Generate deployment script
deploy_script="deploy_web.sh"
echo -e "${YELLOW}Generating: ${deploy_script}${NC}"

cat > "$deploy_script" << 'EOF'
#!/bin/bash

# Web Content Deployment Script
# Auto-generated script to convert and deploy web content

echo "🚀 Deploying web content to Flash memory..."
echo ""

# Run the HTML to header converter
if [ -f "html_to_headers.sh" ]; then
    ./html_to_headers.sh
else
    echo "❌ html_to_headers.sh not found!"
    exit 1
fi

echo "✅ Web content successfully deployed!"
echo ""
echo "📊 Summary:"
echo "- All HTML files converted to header files"
echo "- Content stored in Flash memory (PROGMEM)"
echo "- Ready for compilation and deployment"
echo ""
echo "🔧 Next steps:"
echo "1. Compile and upload your ESP32 firmware"
echo "2. The web interface will be available in setup mode"
echo "3. HTML files in web/ directory are preserved for development"
EOF

chmod +x "$deploy_script"

# Summary
echo -e "${GREEN}================================${NC}"
echo -e "${GREEN}  Conversion Complete!${NC}"
echo -e "${GREEN}================================${NC}"
echo ""
echo "📊 Summary:"
echo "  HTML files processed: ${#html_files[@]}"
echo "  Total HTML size: ${total_size} bytes"
echo "  Header files generated in: $INCLUDE_DIR/"
echo "  Backup directory: $BACKUP_DIR/"
echo ""
echo "📁 Generated files:"
for html_file in "${html_files[@]}"; do
    filename=$(basename "$html_file" .html)
    echo "  - include/web_${filename}.h"
done
echo "  - include/web_resources.h"
echo "  - ${deploy_script}"
echo ""
echo -e "${GREEN}✅ All web content is ready for Flash memory deployment!${NC}"
echo ""
echo "🔧 Next steps:"
echo "1. Use the generated header files in your ESP32 project"
echo "2. Include web_resources.h in your main.cpp"
echo "3. Use WebResources::getSetupPage() etc. to access content"
echo "4. Run ./${deploy_script} to redeploy after HTML changes"
echo ""
echo -e "${BLUE}Development workflow:${NC}"
echo "• Edit HTML files in web/ directory for UI development"
echo "• Run this script to convert HTML to headers"
echo "• Test and iterate as needed"
echo "• Deploy to ESP32 when ready"