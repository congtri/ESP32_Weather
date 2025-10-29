#ifndef WEB_RESOURCES_H
#define WEB_RESOURCES_H

#include <Arduino.h>

// Include individual web page headers
#include "web_setup.h"
#include "web_success.h"
#include "web_restart.h"

// Web resource management class
class WebResources {
public:
    // Get HTML content from PROGMEM
    static String getHTML(const char* htmlTemplate) {
        String html = "";
        html.reserve(strlen_P(htmlTemplate) + 200); // Reserve space for template + variables
        
        // Read from PROGMEM efficiently
        char buffer[256];
        const char* ptr = htmlTemplate;
        
        while (pgm_read_byte(ptr) != 0) {
            int len = 0;
            // Read in chunks of 255 bytes max
            while (len < 255 && pgm_read_byte(ptr + len) != 0) {
                buffer[len] = pgm_read_byte(ptr + len);
                len++;
            }
            buffer[len] = '\0';
            html += buffer;
            ptr += len;
        }
        
        return html;
    }
    
    // Template replacement utility
    static String replaceTemplate(String html, const String& placeholder, const String& value) {
        html.replace(placeholder, value);
        return html;
    }
    
    // Get setup page HTML
    static String getSetupHTML() {
        return getHTML(SETUP_HTML);
    }
    
    // Get success page HTML
    static String getSuccessHTML() {
        return getHTML(SUCCESS_HTML);
    }
    
    // Get restart page HTML
    static String getRestartHTML() {
        return getHTML(RESTART_HTML);
    }
    
    // Get memory usage info for debugging
    static void printMemoryInfo() {
        Serial.printf("Web Resources Memory Usage:\n");
        Serial.printf("  Setup HTML: %d bytes\n", strlen_P(SETUP_HTML));
        Serial.printf("  Success HTML: %d bytes\n", strlen_P(SUCCESS_HTML));
        Serial.printf("  Restart HTML: %d bytes\n", strlen_P(RESTART_HTML));
        Serial.printf("  Total: %d bytes\n", 
            strlen_P(SETUP_HTML) + strlen_P(SUCCESS_HTML) + strlen_P(RESTART_HTML));
    }
};

#endif