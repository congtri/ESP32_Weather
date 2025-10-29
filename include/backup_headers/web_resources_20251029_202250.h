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
