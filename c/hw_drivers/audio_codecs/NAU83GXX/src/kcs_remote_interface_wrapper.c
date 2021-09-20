/*
 *  kcs_remote_interface_wrapper.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"
#include "i2c_api.h"

/** \brief communication stub to read a number of raw bytes from a remote KCS
      *  Setup location
	  * \param[out] data:
	  *   The memory area where the raw KCS Setup data is read to.
	  * \param[in]  offset:
	  *   The memory address offset for reading inside the
	  *    KCS Setup location [in bytes].
	  * \param[in]  len:
	  *     The number of bytes to be read from KCS Setup location.
	  * \return The return value from communication layer.
	  * \ingroup grpKCSRemoteComm */
uint8_t cmd_read_kcs_setup(void *dev, uint8_t *data,
								uint16_t offset, uint16_t len)
{
	struct dev_desc_t *kcs_i2c_dev;
	struct kcs_cmd_get_ioctl_t 	kcs_cmd_get_ioctl;
	uint16_t   recieved_size;
	uint8_t rc = RC_OK;
	uint8_t *recvBuf;

	kcs_i2c_dev = (struct dev_desc_t *)dev;
	kcs_cmd_get_ioctl.offset = offset;
	kcs_cmd_get_ioctl.size_to_read = len;
	kcs_cmd_get_ioctl.recieved_size = &recieved_size;
	kcs_cmd_get_ioctl.recieved_data = &recvBuf;

	rc = DEV_IOCTL_1_PARAMS(
				kcs_i2c_dev, IOCTL_KCS_GET_SETUP_CMD, &kcs_cmd_get_ioctl);

	if(rc == RC_OK) memcpy(data, recvBuf, sizeof(uint8_t) * len);
	DEV_IOCTL_0_PARAMS(kcs_i2c_dev, IOCTL_KCS_EXIT_GET_STATE);

	return rc;
}

/** \brief communication stub to write a number of raw bytes to
      *  a remote KCS Setup location
	  * \param[in] data:
	  *    The memory area where the raw KCS Setup data is copied from.
	  * \param[in] offset:
	  *    The memory address offset for reading inside the
	  *     KCS Setup location [in bytes].
	  * \param[in] len:
	  *     The number of bytes to be written to KCS Setup location.
	  * \return The return value from communication layer.
	  * \ingroup grpKCSRemoteComm */
uint8_t cmd_set_kcs_setup(void *dev, uint8_t const *data,
								uint16_t offset, uint16_t len)
{
	struct dev_desc_t *kcs_i2c_dev;
	struct kcs_start_collect_data_for_send_ioctl_t collect_data_ioctl;
	struct kcs_add_data_for_send_ioctl_t add_data_ioctl;
	int rc = RC_OK;

	kcs_i2c_dev = (struct dev_desc_t *)dev;
	collect_data_ioctl.offset = offset;
	collect_data_ioctl.size_to_collect = len;
	rc = DEV_IOCTL_1_PARAMS(kcs_i2c_dev,
			IOCTL_KCS_START_COLLECT_DATA_FOR_SEND, &collect_data_ioctl);
	if(rc) return rc;

	add_data_ioctl.size = len;
	add_data_ioctl.data = data;

	rc = DEV_IOCTL_1_PARAMS(kcs_i2c_dev,
			IOCTL_KCS_ADD_DATA_FOR_SEND, &add_data_ioctl);
	if(rc) return rc;

	rc = DEV_IOCTL_0_PARAMS(kcs_i2c_dev, IOCTL_KCS_SEND_COLLECTED_SETUP_DATA);

	return rc;
}


/** \brief communication stub to read a number of raw bytes
      *  from a remote KCS Results location
	  * \param[out] data:
	  *    The memory area where the raw KCS Results data is read to.
	  * \param[in]  offset:
	  *    The memory address offset for reading inside the
	  *       KCS Results location [in bytes].
	  * \param[in]  len:
	  *     The number of bytes to be read from KCS Results location.
	  * \return The return value from communication layer.
	  * \ingroup grpKCSRemoteComm */
uint8_t cmd_read_kcs_results(void *dev, uint8_t *data,
								uint16_t offset, uint16_t len)
{
	struct dev_desc_t *kcs_i2c_dev;
	struct kcs_cmd_get_ioctl_t 	kcs_cmd_get_ioctl;
	uint16_t   recieved_size;
	uint8_t rc = RC_OK;
	uint8_t *recvBuf;

	kcs_i2c_dev = (struct dev_desc_t *)dev;
	kcs_cmd_get_ioctl.offset = offset;
	kcs_cmd_get_ioctl.size_to_read = len;
	kcs_cmd_get_ioctl.recieved_size = &recieved_size;
	kcs_cmd_get_ioctl.recieved_data = &recvBuf;

	rc = DEV_IOCTL_1_PARAMS(kcs_i2c_dev,
			IOCTL_KCS_GET_RESULTS_CMD, &kcs_cmd_get_ioctl);

	if(rc == RC_OK) memcpy(data, recvBuf, sizeof(uint8_t) * len);
	DEV_IOCTL_0_PARAMS(kcs_i2c_dev, IOCTL_KCS_EXIT_GET_STATE);

	return rc;
}
