# Web Interface Development

This directory contains the source HTML files for the ESP32 Weather Station web interface.

## Files

- **setup.html** - WiFi configuration page with network scanning
- **success.html** - Configuration success confirmation with animations  
- **restart.html** - Device restart notification with progress indicators

## Development Workflow

1. **Edit HTML files** in this directory for UI development
2. **Test in browser** - Open files directly to preview changes
3. **Convert to headers** - Run `../html_to_headers.sh` to generate C++ headers
4. **Deploy to ESP32** - Compile and upload firmware with new web content

## Quick Commands

```bash
# Convert HTML to headers
../html_to_headers.sh

# Quick deployment  
../deploy_web.sh

# Test in browser
open setup.html
open success.html  
open restart.html
```

## Features

### setup.html
- Responsive WiFi configuration interface
- Real-time network scanning
- Professional styling with animations
- Mobile-friendly design

### success.html  
- Animated success confirmation
- Countdown timer with progress bar
- Network information display
- Sparkle effects for user engagement

### restart.html
- Device restart progress tracking
- Status monitoring
- Connection loss detection
- Multi-step progress indicators

## Notes

- HTML files are converted to PROGMEM constants for Flash storage
- Total size: ~25KB for all three pages
- Content is compiled into firmware for security
- No external dependencies required at runtime

For detailed documentation, see `../docs/WEB_DEVELOPMENT_WORKFLOW.md`