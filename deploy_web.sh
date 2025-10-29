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
