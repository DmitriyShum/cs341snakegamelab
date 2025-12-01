#define DATA_PIN 6 // Pin D6 (PD6)
#define NUM_PIXELS 256
#define GRID_SIZE 256

void WS2812B_Send_Bit(bool bit) {
  // In an assembly implementation, the 'nop' instruction is used for precise timing.
  // In C/C++, compiler optimizations must be disabled (e.g., using volatile or 'no_inline') 
  // and the code tuned by counting clock cycles.

  if (bit) {
    // 1-Bit (800ns HIGH, 450ns LOW)
    PORTD |= (1 << DATA_PIN); 
    // Wait ~13 clock cycles (800ns) for HIGH
    __asm__ __volatile__ (
      "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"
      "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"
      "nop\n\t" "nop\n\t" "nop\n\t"
    ); 
    PORTD &= ~(1 << DATA_PIN); 
    // Wait ~7 clock cycles (450ns) for LOW
    __asm__ __volatile__ (
      "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"
      "nop\n\t" "nop\n\t"
    ); 
  } else {
    // 0-Bit (400ns HIGH, 850ns LOW)
    PORTD |= (1 << DATA_PIN);
    // Wait ~6 clock cycles (400ns) for HIGH
    __asm__ __volatile__ (
      "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"
      "nop\n\t"
    );
    PORTD &= ~(1 << DATA_PIN);
    // Wait ~14 clock cycles (850ns) for LOW
    __asm__ __volatile__ (
      "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"
      "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"
      "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"
    );
  }
}

// Function to send one 24-bit (GRB) color
void WS2812B_Send_Color(uint32_t color) {
  // WS2812B uses GRB order
  uint8_t green = (color >> 16) & 0xFF;
  uint8_t red   = (color >> 8)  & 0xFF;
  uint8_t blue  = (color >> 0)  & 0xFF;
  
  // Send 8 bits of Green, then 8 Red, then 8 Blue
  for (int i = 7; i >= 0; i--) WS2812B_Send_Bit(green & (1 << i));
  for (int i = 7; i >= 0; i--) WS2812B_Send_Bit(red   & (1 << i));
  for (int i = 7; i >= 0; i--) WS2812B_Send_Bit(blue  & (1 << i));
}

// Main function to transmit the entire buffer
void WS2812B_Show(uint32_t *color_buffer, int num_pixels) {
  noInterrupts(); // Disable interrupts for critical timing

  for (int i = 0; i < num_pixels; i++) {
    WS2812B_Send_Color(color_buffer[i]);
  }
  
  interrupts(); // Re-enable interrupts

  // Reset pulse: Must be LOW for at least 50us
  // We can use a slow digitalWrite for the reset pulse as it's not time critical
  digitalWrite(DATA_PIN, LOW);
  delayMicroseconds(60); 
}

uint32_t color_buffer[GRID_SIZE];

void setup() {
  // put your setup code here, to run once:
  DDRD |= (1 << DATA_PIN);
  for (int i = 0; i < GRID_SIZE; i++) {
      color_buffer[i] = 0xFF0000;
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  WS2812B_Show(color_buffer, GRID_SIZE);
}
