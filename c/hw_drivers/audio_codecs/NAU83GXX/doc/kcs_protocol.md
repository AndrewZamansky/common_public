# Command list

- [get_kcs_devices](#get_kcs_devices)
- [cmd_kcs_get_status](#cmd_kcs_get_status)
- [cmd_kcs_clkstart](#cmd_kcs_clkstart)
- [cmd_kcs_clkstop](#cmd_kcs_clkstop)
- [cmd_kcs_get_counter](#cmd_kcs_get_counter)
- [cmd_kcs_get_data](#cmd_kcs_get_data)
- [cmd_kcs_get_info](#cmd_kcs_get_info)
- [cmd_kcs_get_setup](#cmd_kcs_get_setup)
- [cmd_kcs_init_nau83gxx_chip](#cmd_kcs_init_nau83gxx_chip)
- [cmd_kcs_read_i2c_reg](#cmd_kcs_read_i2c_reg)
- [cmd_kcs_set_add_data](#cmd_kcs_set_add_data)
- [cmd_kcs_set_send](#cmd_kcs_set_send)
- [cmd_kcs_set_start](#cmd_kcs_set_start)
- [cmd_kcs_swrev](#cmd_kcs_swrev)

# Error status code

```
  NAU83GXX_RC_OK                            = 0x00,

  //Messages for KCS Device
  NAU83GXX_RC_COMMUNICATION_ERROR           = 0x01,
  NAU83GXX_RC_PREAMBLE_NOT_FOUND            = 0x02,
  NAU83GXX_RC_REPLY_LEN_TOO_LONG            = 0x03,
  NAU83GXX_RC_REPLY_DATA_INTEGRITY_ERROR    = 0x04,
  NAU83GXX_RC_TIMEDOUT_FOR_IDLE_WORD        = 0x05,
  NAU83GXX_RC_ALC_SEGMENT_TOO_BIG           = 0x06,
  NAU83GXX_RC_ALC_WRONG_VBAT_MIN_MAX_LEVELS = 0x07,
  NAU83GXX_RC_REPLY_STATUS_ERR              = 0x10,

  NAU83GXX_RC_BUFFER_OVERFLOW_ERR           = 0x20,
  NAU83GXX_RC_INPUT_SIZE_TOO_LARGE_ERR      = 0x21,
  NAU83GXX_RC_BUFFER_NOT_EQUAL_TO_SIZE      = 0x22,
  NAU83GXX_RC_UNEXPECTED_REPLY              = 0x23,
  NAU83GXX_RC_DRIVER_IN_WRONG_STATE         = 0x24,
  NAU83GXX_RC_I2C_DATA_TOO_BIG              = 0x25,

  NAU83GXX_RC_SYNTAX_ERROR                  = 0x30,
  NAU83GXX_RC_DEVICE_DOES_NOT_EXIST         = 0x31,
  NAU83GXX_RC_PARAMETERS_OUT_OF_RANGE       = 0x32,
  NAU83GXX_RC_NOT_SUPPORTED_DEVICE_ID       = 0x33,
  NAU83GXX_RC_NOT_SUPPORTED_IOCTL           = 0x34,
  NAU83GXX_RC_NOT_INTERNAL_DEVICE           = 0x35,

  NAU83GXX_RC_I2C_ERROR                     = 0x43,

  //Messages for Driver State (muxed with state)
  NAU83GXX_RC_DRIVER_NOT_READY              = 0xA0,
  NAU83GXX_RC_DRIVER_BUSY                   = 0xB0,
  NAU83GXX_RC_OS_QUEUE_TIMEOUT              = 0xEF
```

## get_kcs_devices

**Description**: get the list of kcs devices

**Syntax**: `get_kcs_devices`

**Return**: the list of kcs devices in the following format - `<NAME>.<TYPE>.<CORE>`

- NAME – device name that is used to access for accessing kcs
- TYPE – chip type. for example G10/G20/G60
- CORE – core number. 0 for G10/G20. 0/1 for G60

## cmd_kcs_get_status

**Description**: command to read status data from kcs

**Syntax**: `kcs_get_status <device_name>`

- device_name - name of device as received from get_kcs_devices command

**Return**:

- error status in format `rcXX`. For example, rc00 means no error
- status data - interpreted by dbLab

## cmd_kcs_clkstart

**Description**:

**Syntax**: `kcs_clkstart <device_name>`

- device_name - name of device as received from get_kcs_devices command

**Return**:

- error status in format `rcXX`. For example, rc00 means no error
- status data - interpreted by dbLab

## cmd_kcs_clkstop

**Description**:

**Syntax**: `kcs_clkstop <device_name>`

- device_name - name of device as received from get_kcs_devices command

**Return**:

- error status in format `rcXX`. For example, rc00 means no error
- status data - interpreted by dbLab

## cmd_kcs_get_counter

**Description**: command to test if kcs is alive, counter increase by 1 every read

**Syntax**: `kcs_get_counter <device_name>`

- device_name - name of device as received from get_kcs_devices command

**Return**:

- error status in format `rcXX`. For example, rc00 means no error
- value of counter

## cmd_kcs_get_data

**Description**: command to read data section from kcs

**Syntax**: `kcs_get_data <device_name> <off_set> <size>`

- device_name - name of device as received from get_kcs_devices command
- off_set - offset to read from in bytes
- size - size of data to read in bytes

**Return**:

- error status in format `rcXX`. For example, rc00 means no error
- data organized in words of 4 bytes on each lines, display in little-endian format \
  if size is not align by 4, the remaining bytes printed at the end

## cmd_kcs_get_info

**Description**: command to read the information from kcs

**Syntax**: `kcs_get_info <device_name>`

- device_name - name of device as received from get_kcs_devices command

**Return**:

- Information \
  gain = \
  u limit (value) = \
  u limit (shift) = \
  u limit [SI unit] =
- error status in format `rcXX`. For example, rc00 means no error

## cmd_kcs_get_setup

**Description**: command to read data section from kcs

**Syntax**: `kcs_get_setup <device_name> <off_set> <size>`

- device_name - name of device as received from get_kcs_devices command
- off_set - offset to read from in bytes
- size - size of data to read in bytes

**Return**:

- error status in format `rcXX`. For example, rc00 means no error
- data organized in words of 4 bytes on each lines, display in little-endian format \
  if size is not align by 4, the remaining bytes printed at the end

## cmd_kcs_init_nau83gxx_chip

**Description**: command to reset the chip and load all the registers into NAU83GXX stored in firmware

**Syntax**: `kcs_init_nau83gxx_chip <device_name>`

- device_name - name of device as received from get_kcs_devices command

**Return**:

- error status in format `rcXX`. For example, rc00 means no error

## cmd_kcs_read_i2c_reg

**Description**: command to read the data of i2c at specific address from kcs

**Syntax**: `kcs_read_i2c_reg <device_name> <reg_address>`

- device_name - name of device as received from get_kcs_devices command
- reg_address - address of register to read

**Return**:

- error status in format `rcXX`. For example, rc00 means no error
- 4-byte data at registers 0xF000 and 0xF002, 2-byte data at others

## cmd_kcs_write_i2c_reg

**Description**: command to write the data of i2c at specific address to kcs

**Syntax**: `kcs_write_i2c_reg <device_name> <reg_address> <data>`

- device_name - name of device as received from get_kcs_devices command
- reg_address - address of register
- data - data to write

**Return**:

- error status in format `rcXX`. For example, rc00 means no error

## cmd_kcs_set_add_data

**Description**: command to write 4 bytes to the buffer of the kcs_i2c device and increments the counter \
 by 4 to proceed to the next memory address

**Syntax**: `kcs_set_add_data <device_name> <data>`

- device_name - name of device as received from get_kcs_devices command
- data - 4-byte data being stored

**Return**:

- error status in format `rcXX`. For example, rc00 means no error

## cmd_kcs_set_send

**Description**: command to send collected setup data to kcs

**Syntax**: `kcs_set_send <device_name>`

- device_name - name of device as received from get_kcs_devices command

**Return**:

- error status in format `rcXX`. For example, rc00 means no error

## cmd_kcs_set_start

**Description**: command to start the process for kcs setup

**Syntax**: `kcs_set_start <device_name> <offset> <size>`

- device_name - name of device as received from get_kcs_devices command
- off_set - offset of the address to start in hex
- size - size of data being received

**Return**:

- error status in format `rcXX`. For example, rc00 means no error

## cmd_kcs_swrev

**Description**:

**Syntax**: `kcs_swrev <device_name>`

- device_name - name of device as received from get_kcs_devices command

**Return**:

- error status in format `rcXX`. For example, rc00 means no error
