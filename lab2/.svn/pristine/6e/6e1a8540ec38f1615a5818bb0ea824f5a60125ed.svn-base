#include <lcom/lcf.h>

#include <stdint.h>
// @brief gets 16 bits and grabs the least significant byte and puts it into lsb
// @param val original 2 byte value that we want to shorted to only the LSB
// @param *lsb memory space where the 1 byte we achieved from the val will be stored
int(util_get_LSB)(uint16_t val, uint8_t *lsb) {

  val = val & 0x00ff;  // mask zero's msb leaving only the 8 lsbits
  *lsb = (uint8_t) val; // tranforms 16 to 8 grabbing only the 8 lsb bits
  return 0;

}


// @brief gets 16 bits and grabs the most significant byte and puts it into msb
// @param val original 2 byte value that we want to shorted to only the MSB
// @param *msb memory space where the 1 byte we achieved from the val will be stored
int(util_get_MSB)(uint16_t val, uint8_t *msb) {

    val = (val >> 8) & 0x00ff; // shifts msb to lsb  and masks makes sure that everything behind is 0
    *msb = (uint8_t) val;      // tranforms 16 to 8 grabbing only the 8 lsb bits
    return 0;

}

//function that takes the 32 bit output from sys_inb() and casts it to 8 bits
// sys_inb read information from the port
// @port is where we real the data from
// @value, outside variable that gets updated with the 8 bit output of sys_inb
int (util_sys_inb)(int port, uint8_t *value) {
  
  uint32_t v;
  int response = sys_inb(port, &v);
  *value = (uint8_t) v;
  return response;

}
